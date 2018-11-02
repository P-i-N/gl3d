#ifndef __GL3D_BASE_H_IMPL__
	#define __GL3D_BASE_H_IMPL__
#endif

#include "gl3d_base.h"

#include <cstdarg>
#include <fstream>

#define GL3D_FORMAT_LOG_TEXT(_Input) \
	if (!(_Input)) return; \
	va_list ap; \
	va_start(ap, _Input); \
	vsnprintf(detail::tl_logBuffer, detail::logBufferSize, _Input, ap); \
	va_end(ap)

namespace gl3d {

decltype( on_log_message ) on_log_message;
decltype( on_data_request ) on_data_request;

namespace detail {

constexpr char *s_lineSeparator = "\n";
constexpr size_t logBufferSize = 1025;
thread_local char tl_logBuffer[logBufferSize];

//---------------------------------------------------------------------------------------------------------------------
std::string_view trim( std::string_view text )
{
	if ( text.empty() ) return text;

	size_t start = 0, end = text.length() - 1;
	while ( start < text.length() && isspace( text[start] ) ) ++start;
	while ( end > start && isspace( text[end] ) ) --end;

	return text.substr( start, end - start + 1 );
}

//---------------------------------------------------------------------------------------------------------------------
std::string_view to_string_view( bytes_t &bytes )
{
	return std::string_view( reinterpret_cast<const char *>( bytes.data() ), bytes.size() );
}

//---------------------------------------------------------------------------------------------------------------------
void for_each_line( std::string_view text, std::function<void( std::string_view, unsigned )> callback )
{
	size_t cursor = 0;
	unsigned lineNum = 0;
	while ( cursor <= text.length() )
	{
		auto sepPos = text.find( s_lineSeparator, cursor );
		if ( sepPos == std::string::npos )
			sepPos = text.length();

		callback( text.substr( cursor, sepPos - cursor ), ++lineNum );
		cursor = sepPos + strlen( s_lineSeparator );
	}
}

//---------------------------------------------------------------------------------------------------------------------
bool read_all_bytes( std::istream &is, bytes_t &bytes, bool addNullTerm, size_t size )
{
	if ( size == size_t( -1 ) )
	{
		is.seekg( 0, std::ios_base::end );
		size = is.tellg();
		is.seekg( 0, std::ios_base::beg );
	}

	bytes.resize( size );
	is.read( reinterpret_cast<char *>( bytes.data() ), size );
	if ( addNullTerm ) bytes.push_back( 0 );
	return true;
}

} // namespace gl3d::detail

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
void log::info( const char *fmt, ... )
{
	GL3D_FORMAT_LOG_TEXT( fmt );
	on_log_message.call( log::message_type::info, detail::tl_logBuffer );
}

//---------------------------------------------------------------------------------------------------------------------
void log::success( const char *fmt, ... )
{
	GL3D_FORMAT_LOG_TEXT( fmt );
	on_log_message.call( log::message_type::success, detail::tl_logBuffer );
}

//---------------------------------------------------------------------------------------------------------------------
void log::warning( const char *fmt, ... )
{
	GL3D_FORMAT_LOG_TEXT( fmt );
	on_log_message.call( log::message_type::warning, detail::tl_logBuffer );
}

//---------------------------------------------------------------------------------------------------------------------
void log::error( const char *fmt, ... )
{
	GL3D_FORMAT_LOG_TEXT( fmt );
	on_log_message.call( log::message_type::error, detail::tl_logBuffer );
}

//---------------------------------------------------------------------------------------------------------------------
void log::fatal( const char *fmt, ... )
{
	GL3D_FORMAT_LOG_TEXT( fmt );
	on_log_message.call( log::message_type::fatal, detail::tl_logBuffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail {

struct mount_info
{
	std::filesystem::path path;
	std::function<bool( const std::filesystem::path &, std::vector<uint8_t> & )> callback;

	bool operator==( const std::filesystem::path &p ) const { return path == p; }
};

std::mutex g_mountInfosMutex;
std::vector<mount_info> g_mountInfos;

struct vfs_init { vfs_init() { vfs::mount( "." ); } } g_vfsInit;

} // namespace gl3d::detail

//---------------------------------------------------------------------------------------------------------------------
void vfs::mount( const std::filesystem::path &path )
{
	auto absPath = std::filesystem::absolute( path );

	std::scoped_lock lock( detail::g_mountInfosMutex );
	auto iter = std::find( detail::g_mountInfos.begin(), detail::g_mountInfos.end(), absPath );
	if ( iter == detail::g_mountInfos.end() )
	{
		auto callback = [absPath]( const std::filesystem::path & relPath, detail::bytes_t &bytes )->bool
		{
			auto finalPath = std::filesystem::absolute( absPath / relPath );
			if ( std::filesystem::is_regular_file( finalPath ) )
			{
				std::ifstream ifs( finalPath.c_str(), std::ios_base::in | std::ios_base::binary );
				if ( !ifs.is_open() )
					return false;

				size_t size = std::filesystem::file_size( finalPath );
				if ( !detail::read_all_bytes( ifs, bytes, false, size ) )
				{
					return false;
				}

				return true;
			}

			return false;
		};

		on_data_request += callback;
		detail::g_mountInfos.push_back( { absPath, callback } );
	}
}

//---------------------------------------------------------------------------------------------------------------------
bool vfs::unmount( const std::filesystem::path &path )
{
	auto absPath = std::filesystem::absolute( path );

	std::scoped_lock lock( detail::g_mountInfosMutex );
	auto iter = std::find( detail::g_mountInfos.begin(), detail::g_mountInfos.end(), absPath );
	if ( iter == detail::g_mountInfos.end() )
		return false;

	detail::g_mountInfos.erase( iter );
	return true;
}

} // namespace gl3d

#undef GL3D_FORMAT_LOG_TEXT
