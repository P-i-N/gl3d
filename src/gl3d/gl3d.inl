#ifndef __GL3D_H_IMPL__
	#define __GL3D_H_IMPL__
#endif

#include "gl3d.h"
#include "gl3d_base.h"

#if defined(_MSC_VER)
	#pragma comment(lib, "opengl32.lib")
#endif

#if defined(WIN32)
	#ifndef VC_EXTRALEAN
		#define VC_EXTRALEAN
	#endif

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#else
	#error Not implemented!
#endif

#include <gl/GL.h>

#include <cassert>

namespace gl3d {

detail::gl_api gl;

namespace detail {

thread_local context *tl_currentContext = nullptr;

//---------------------------------------------------------------------------------------------------------------------
int find_uniform_id( const detail::location_variant &location )
{
	if ( !location.holds_name() ) return location.id();

	int programID;
	glGetIntegerv( +gl_enum::CURRENT_PROGRAM, &programID );
	return gl.GetUniformLocation( programID, location.data );
}

} // namespace gl3d::detail

//---------------------------------------------------------------------------------------------------------------------
void check_gl_error()
{
#if defined(_DEBUG)
	GLenum err = glGetError();
	if ( err != GL_NO_ERROR )
	{
		const char *errName = "(unknown error)";

		switch ( err )
		{
			case GL_NO_ERROR:
				errName = "GL_NO_ERROR";
				break;
			case GL_INVALID_ENUM:
				errName = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				errName = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				errName = "GL_INVALID_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				errName = "GL_OUT_OF_MEMORY";
				break;
			case GL_STACK_UNDERFLOW:
				errName = "GL_STACK_UNDERFLOW";
				break;
			case GL_STACK_OVERFLOW:
				errName = "GL_STACK_OVERFLOW";
				break;
		}

		log::error( "OpenGL error (%d): %s", err, errName );
		assert( 0 );
	}
#endif
}

//---------------------------------------------------------------------------------------------------------------------
buffer::buffer( buffer_usage usage, const void *data, size_t size, bool makeCopy )
	: _usage( usage )
	, _size( size )
	, _owner( ( data != nullptr ) && makeCopy )
{
	if ( _owner )
	{
		_data = new uint8_t[_size];
		memcpy( _data, data, _size );
	}
	else
		_data = static_cast<uint8_t *>( const_cast<void *>( data ) );
}

//---------------------------------------------------------------------------------------------------------------------
buffer::~buffer()
{
	if ( _owner )
		delete[] _data;
}

//---------------------------------------------------------------------------------------------------------------------
void buffer::bind( gl_enum target )
{
	if ( !_id )
	{
		gl.CreateBuffers( 1, &_id );

		if ( _size )
		{
			gl.NamedBufferData( _id, static_cast<int>( _size ), _data, gl_enum::STATIC_DRAW );

			if ( _owner )
			{
				delete[] _data;
				_data = nullptr;
				_owner = false;
			}
		}
	}

	gl.BindBuffer( target, _id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
shader::shader( std::shared_ptr<shader_code> code, std::string_view defines )
	: _shaderCode( code )
	, _defines( defines )
{

}

//---------------------------------------------------------------------------------------------------------------------
shader::~shader()
{
	clear();
}

//---------------------------------------------------------------------------------------------------------------------
void shader::clear()
{
	for ( auto &stageID : _stageIDs )
	{
		gl.DeleteShader( stageID );
		stageID = 0;
	}

	gl.DeleteProgram( _id );
	_id = 0;
}

//---------------------------------------------------------------------------------------------------------------------
bool shader::compile()
{
	for ( size_t i = 0; i < +shader_stage::__count; ++i )
	{
		auto &src = _shaderCode->stage_source( static_cast<shader_stage>( i ) );
		if ( src.empty() )
		{
			if ( _stageIDs[i] )
			{
				gl.DeleteShader( _stageIDs[i] );
				_stageIDs[i] = 0;
			}

			continue;
		}

		if ( !_stageIDs[i] )
		{
			static constexpr gl_enum glTypes[] =
			{ gl_enum::VERTEX_SHADER, gl_enum::GEOMETRY_SHADER, gl_enum::FRAGMENT_SHADER, gl_enum::COMPUTE_SHADER };

			_stageIDs[i] = gl.CreateShader( glTypes[i] );
		}

		auto srcData = src.c_str();
		gl.ShaderSource( _stageIDs[i], 1, &srcData, nullptr );
		gl.CompileShader( _stageIDs[i] );

		int compileStatus;
		gl.GetShaderiv( _stageIDs[i], gl_enum::COMPILE_STATUS, &compileStatus );
		if ( !compileStatus )
		{
			int logLength;
			gl.GetShaderiv( _stageIDs[i], gl_enum::INFO_LOG_LENGTH, &logLength );

			std::unique_ptr<char[]> text = std::make_unique<char[]>( logLength + 1 );
			gl.GetShaderInfoLog( _stageIDs[i], logLength, nullptr, text.get() );
			text[logLength] = 0;

			log::error( "%s", text.get() );
			return false;
		}
	}

	if ( !_id )
		_id = gl.CreateProgram();

	for ( auto stageID : _stageIDs ) if ( stageID ) gl.AttachShader( _id, stageID );
	gl.LinkProgram( _id );
	for ( auto stageID : _stageIDs ) if ( stageID ) gl.DetachShader( _id, stageID );

	int linkStatus;
	gl.GetProgramiv( _id, gl_enum::LINK_STATUS, &linkStatus );
	if ( !linkStatus )
	{
		clear();
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
bool shader_code::source( std::string_view sourceCode, const std::filesystem::path &cwd )
{
	std::stringstream ss;
	if ( !detail::unroll_includes( ss, sourceCode, cwd ) )
		return false;

	for ( auto &stageSource : _stageSources )
		stageSource.clear();

	std::string sharedSource = "#version 330 core\n";
	std::string *currentStage = &sharedSource;

	_unrolledSource = ss.str();
	detail::for_each_line( _unrolledSource, [&]( std::string_view line, unsigned lineNum )
	{
		bool addLine = true;

		if ( auto dir = detail::trim( line ); !dir.empty() && dir[0] == '#' )
		{
			dir = detail::trim( dir.substr( 1 ) ); // Cut away '#' & trim
			if ( detail::starts_with_nocase( dir, "vertex", "vert", "vs" ) )
			{
				addLine = false;
				currentStage = &_stageSources[+shader_stage::vertex];
			}
			else if ( detail::starts_with_nocase( dir, "fragment", "frag", "fs", "pixel" ) )
			{
				addLine = false;
				currentStage = &_stageSources[+shader_stage::fragment];
			}
		}

		if ( addLine )
		{
			*currentStage += line;
			*currentStage += detail::s_lineSeparator;
		}
	} );

	for ( auto &stageSource : _stageSources )
		if ( !stageSource.empty() )
			stageSource = ( sharedSource + stageSource );

	_path.clear();
	_source = sourceCode;
	return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool shader_code::load( std::istream &is, const std::filesystem::path &cwd )
{
	detail::bytes_t bytes;
	detail::read_all_bytes( is, bytes, true );

	return source( detail::to_string_view( bytes ), cwd );
}

//---------------------------------------------------------------------------------------------------------------------
bool shader_code::load( const std::filesystem::path &path )
{
	detail::bytes_t bytes;
	if ( !vfs::load( path, bytes ) )
		return false;

	if ( !source( detail::to_string_view( bytes ), path.parent_path() ) )
		return false;

	_path = path;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
cmd_queue::cmd_queue( gl_state *state )
	: _recording( state == nullptr )
	, _state( state )
{
	if ( _recording )
	{
		_recordedData.reserve( 65536 );
		_resources.reserve( 1024 );
	}
}

//---------------------------------------------------------------------------------------------------------------------
cmd_queue::~cmd_queue()
{

}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::reset()
{
	_position = 0;
	_resources.clear();
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::clear_color( const vec4 &color )
{
	if ( _recording )
		write( cmd_type::clear_color, color );
	else
	{
		glClearColor( color.x, color.y, color.z, color.w );
		glClear( GL_COLOR_BUFFER_BIT );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::clear_depth( float depth )
{
	if ( _recording )
		write( cmd_type::clear_depth, depth );
	else
	{
		glClearDepth( depth );
		glClear( GL_DEPTH_BUFFER_BIT );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_state( const blend_state &bs )
{
	if ( _recording )
		write( cmd_type::bind_blend_state, bs );
	else
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_state( const depth_stencil_state &ds )
{
	if ( _recording )
		write( cmd_type::bind_depth_stencil_state, ds );
	else
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_state( const rasterizer_state &rs )
{
	if ( _recording )
		write( cmd_type::bind_rasterizer_state, rs );
	else
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_shader( shader::ptr sh )
{
	if ( _recording )
	{
		write( cmd_type::bind_shader );
		_resources.push_back( sh );
	}
	else
	{
		if ( sh )
		{
			if ( !sh->id() )
				sh->compile();

			gl.UseProgram( sh->id() );
			_state->current_program = sh->id();
		}
		else
		{
			gl.UseProgram( 0 );
			_state->current_program = 0;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_vertex_buffer( buffer::ptr vb, const detail::layout &layout, size_t offset )
{
	if ( _recording )
	{
		write( cmd_type::bind_vertex_buffer, &layout, offset );
		_resources.push_back( vb );
	}
	else
	{
		vb->bind( gl_enum::ARRAY_BUFFER );
		auto vaoID = detail::tl_currentContext->bind_vao( vb, layout, offset );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_vertex_attribute( buffer::ptr attribs, unsigned slot, gl_enum glType, size_t offset, size_t stride )
{
	if ( _recording )
	{
		write( cmd_type::bind_vertex_atrribute, slot, glType, offset, stride );
		_resources.push_back( attribs );
	}
	else
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::bind_index_buffer( buffer::ptr ib, bool use16bits, size_t offset )
{
	if ( _recording )
	{
		write( cmd_type::bind_index_buffer, use16bits, offset );
		_resources.push_back( ib );
	}
	else
	{
		ib->bind( gl_enum::ELEMENT_ARRAY_BUFFER );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::set_uniform_block( const detail::location_variant &location, const void *data, size_t size )
{
	assert( data && size );

	if ( _recording )
	{
		assert( size <= 65536 );
		write( cmd_type::set_uniform_block );
		write_location_variant( location );
		write_data( data, size );
	}
	else
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::set_uniform( const detail::location_variant &location, bool value )
{
	if ( _recording )
	{
		write( cmd_type::set_uniform, gl_enum::BOOL, value );
		write_location_variant( location );
	}
	else
	{
		auto id = find_uniform_id( location );
		gl.Uniform1i( id, value ? 1 : 0 );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::set_uniform( const detail::location_variant &location, int value )
{
	if ( _recording )
	{
		write( cmd_type::set_uniform, gl_enum::INT, value );
		write_location_variant( location );
	}
	else
	{
		auto id = find_uniform_id( location );
		gl.Uniform1i( id, value );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::set_uniform( const detail::location_variant &location, float value )
{
	if ( _recording )
	{
		write( cmd_type::set_uniform, gl_enum::FLOAT, value );
		write_location_variant( location );
	}
	else
	{
		auto id = find_uniform_id( location );
		gl.Uniform1f( id, value );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::draw( gl_enum primitive, size_t first, size_t count, size_t instanceCount, size_t instanceBase )
{
	if ( _recording )
	{
		write( cmd_type::draw, primitive, first, count, instanceCount, instanceBase );
	}
	else
	{
		glDrawArrays( +primitive, static_cast<int>( first ), static_cast<int>( count ) );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::draw_indexed( gl_enum primitive, size_t first, size_t count, size_t instanceCount, size_t instanceBase )
{
	if ( _recording )
	{
		write( cmd_type::draw_indexed, primitive, first, count, instanceCount, instanceBase );
	}
	else
	{

	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::execute( ptr cmdQueue )
{
	assert( cmdQueue && cmdQueue.get() != this );

	if ( _recording )
	{
		write( cmd_type::execute );
		_resources.push_back( cmdQueue );
	}
	else
	{
		cmdQueue->execute( _state );
	}
}

//---------------------------------------------------------------------------------------------------------------------
void cmd_queue::execute( gl_state *state )
{
	_state = state;
	_recording = false;
	_position = 0;
	size_t resIndex = 0;

	while ( _position < _recordedData.size() )
	{
		auto cmd = read<cmd_type>();
		switch ( cmd )
		{
			default:
				assert( 0 );
				break;

			case cmd_type::clear_color:
			{
				auto color = read<vec4>();
				clear_color( color );
			}
			break;

			case cmd_type::clear_depth:
			{
				auto depth = read<float>();
				clear_depth( depth );
			}
			break;

			case cmd_type::bind_blend_state:
				bind_state( read<blend_state>() );
				break;

			case cmd_type::bind_depth_stencil_state:
				bind_state( read<depth_stencil_state>() );
				break;

			case cmd_type::bind_rasterizer_state:
				bind_state( read<rasterizer_state>() );
				break;

			case cmd_type::bind_shader:
			{
				auto sh = std::static_pointer_cast<shader>( _resources[resIndex++] );
				bind_shader( sh );
			}
			break;

			case cmd_type::bind_vertex_buffer:
			{
				auto vb = std::static_pointer_cast<buffer>( _resources[resIndex++] );
				const auto &layout = *read<const detail::layout *>();
				auto offset = read<size_t>();
				bind_vertex_buffer( vb, layout, offset );
			}
			break;

			case cmd_type::bind_vertex_atrribute:
			{
				auto attribs = std::static_pointer_cast<buffer>( _resources[resIndex++] );
				auto slot = read<unsigned>();
				auto glType = read<gl_enum>();
				auto offset = read<size_t>();
				auto stride = read<size_t>();
				bind_vertex_attribute( attribs, slot, glType, offset, stride );
			}
			break;

			case cmd_type::bind_index_buffer:
			{
				auto ib = std::static_pointer_cast<buffer>( _resources[resIndex++] );
				auto use16bits = read<bool>();
				auto offset = read<size_t>();
				bind_index_buffer( ib, use16bits, offset );
			}
			break;

			case cmd_type::set_uniform_block:
			{
				auto location = read_location_variant();
				auto data = read_data();
				set_uniform_block( location, data.first, data.second );
			}
			break;

			case cmd_type::set_uniform:
			{
				switch ( read<gl_enum>() )
				{
					case gl_enum::BOOL:
					{
						auto value = read<bool>();
						set_uniform( read_location_variant(), value );
					}
					break;

					case gl_enum::INT:
					{
						auto value = read<int>();
						set_uniform( read_location_variant(), value );
					}
					break;

					case gl_enum::FLOAT:
					{
						auto value = read<float>();
						set_uniform( read_location_variant(), value );
					}
					break;
				}
			}
			break;

			case cmd_type::draw:
			case cmd_type::draw_indexed:
			{
				auto primitive = read<gl_enum>();
				auto first = read<size_t>();
				auto count = read<size_t>();
				auto instanceCount = read<size_t>();
				auto instanceBase = read<size_t>();

				if ( cmd == cmd_type::draw )
					draw( primitive, first, count, instanceCount, instanceBase );
				else
					draw_indexed( primitive, first, count, instanceCount, instanceBase );
			}
			break;

			case cmd_type::execute:
			{
				auto cmdQueue = std::static_pointer_cast<cmd_queue>( _resources[resIndex++] );
				execute( cmdQueue );
			}
			break;
		}

		check_gl_error();
	}

	_state = nullptr;
	_recording = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {

#if defined(WIN32)
unsigned g_contextAttribs[] =
{
	+gl_enum::CONTEXT_MAJOR_VERSION_ARB, 4,
	+gl_enum::CONTEXT_MINOR_VERSION_ARB, 6,
	+gl_enum::CONTEXT_PROFILE_MASK_ARB, +gl_enum::CONTEXT_CORE_PROFILE_BIT_ARB,
	0
};

//---------------------------------------------------------------------------------------------------------------------
context::context( void *windowNativeHandle )
	: cmd_queue( &_glState )
	, _window_native_handle( windowNativeHandle )
{
	static HMODULE s_renderDoc = LoadLibraryA( "renderdoc.dll" );

	auto hdc = GetDC( HWND( _window_native_handle ) );
	auto tempContext = wglCreateContext( hdc );
	wglMakeCurrent( hdc, tempContext );

	gl = gl_api();

	_native_handle = gl.CreateContextAttribsARB(
	                     hdc,
	                     nullptr,
	                     reinterpret_cast<const int *>( g_contextAttribs ) );

	wglMakeCurrent( nullptr, nullptr );
	wglDeleteContext( tempContext );
}

//---------------------------------------------------------------------------------------------------------------------
context::context( ptr sharedContext )
	: cmd_queue( &_glState )
	, _window_native_handle( sharedContext->_window_native_handle )
{
	sharedContext->make_current();

	_window_native_handle = sharedContext->_window_native_handle;
	auto hdc = GetDC( HWND( _window_native_handle ) );

	_native_handle = gl.CreateContextAttribsARB(
	                     hdc,
	                     HGLRC( sharedContext->_native_handle ),
	                     reinterpret_cast<const int *>( g_contextAttribs ) );

	wglMakeCurrent( nullptr, nullptr );

	_state = &_glState;
}

//---------------------------------------------------------------------------------------------------------------------
context::~context()
{
	wglDeleteContext( HGLRC( _native_handle ) );
}

//---------------------------------------------------------------------------------------------------------------------
void context::make_current()
{
	if ( wglGetCurrentContext() != HGLRC( _native_handle ) )
		wglMakeCurrent( GetDC( HWND( _window_native_handle ) ), HGLRC( _native_handle ) );

	tl_currentContext = this;
}
#else
#error Not implemented!
#endif

//---------------------------------------------------------------------------------------------------------------------
unsigned context::bind_vao( buffer::ptr vb, const detail::layout &layout, size_t offset )
{
	unsigned vaoID = 0;

	auto ptrID = reinterpret_cast<std::uintptr_t>( &layout );
	if ( auto iter = _layoutVAOs.find( ptrID ); iter == _layoutVAOs.end() )
	{
		gl.CreateVertexArrays( 1, &vaoID );

		for ( unsigned i = 0; i < 8; ++i )
		{
			if ( layout.mask & ( 1u << i ) )
				gl.EnableVertexArrayAttrib( vaoID, i );
			else
				gl.DisableVertexArrayAttrib( vaoID, i );
		}

		for ( auto &a : layout.attribs )
		{
			gl.VertexArrayAttribFormat( vaoID, a.location, a.element_count, a.element_type, 0, a.offset );
			gl.VertexArrayAttribBinding( vaoID, a.location, 0 );
		}

		_layoutVAOs.insert( { ptrID, vaoID } );
	}
	else
		vaoID = iter->second;

	gl.VertexArrayVertexBuffer( vaoID, 0, vb->id(), reinterpret_cast<const void *>( offset ), layout.stride );
	gl.BindVertexArray( vaoID );

	check_gl_error();
	return vaoID;
}

} // namespace gl3d::detail

} // namespace gl3d
