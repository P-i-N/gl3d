#ifndef __GL3D_QUICK_DRAW_H__
#define __GL3D_QUICK_DRAW_H__

#include <map>
#include <unordered_map>

#include "gl3d.h"

namespace gl3d {

struct GL3D_API font
{
	using ptr = std::shared_ptr<font>;

	int base = 0;
	int line_height = 0;

	struct char_info
	{
		int id = 0;
		uvec2 size;
		ibox2 box;
		ivec2 offset;
		vec2 uv[4];
		int x_advance;
	};

	std::map<int, char_info> char_infos;
	std::map<std::pair<int, int>, int> kernings;
	texture::ptr char_atlas;

	static ptr create( const char *base64Data );
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class sorting_mode { none = 0, front_to_back, back_to_front };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GL3D_API quick_draw
{
public:
	using ptr = std::shared_ptr<quick_draw>;

	quick_draw();

	virtual ~quick_draw();

	void reset();

	void render( cmd_queue::ptr queue, const mat4 &view, const mat4 &proj );

	void push_transform();

	void push_transform( const mat4 &mult );

	void pop_transform();

	void bind_texture( texture::ptr tex );

	void bind_font( font::ptr f );

	void set_scissors( const uvec4 &box );

	void begin( gl_enum primitiveType );

	void end();

	bool building_mesh() const { return _buildingMesh; }

	void vertex( const vec3 &pos );

	void vertex( const vec2 &pos, float z = 0.0f ) { vertex( { pos.x, pos.y, z } ); }

	void color( const vec4 &c );

	void color( const vec3 &c, float alpha = 1.0f ) { color( { c.x, c.y, c.z, alpha } ); }

	void uv( const vec2 &texCoord );

	void draw_aabb( const box3 &aabb, bool filled = false );

	void draw_frustum( const mat4 &mat );

	void draw_texture( const vec2 &pos, texture::ptr tex, float scale = 1.0f );

	void draw_text( const vec2 &pos, std::string_view text );

protected:
	bool _dirtyBuffers = true;
	bool _buildingMesh = false;

	struct state
	{
		blend_state bs;
		depth_stencil_state ds;
		rasterizer_state rs;
	};

	std::vector<state> _states;

	state _currentState;

	struct draw_call
	{
		gl_enum primitive = gl_enum::NONE;
		unsigned firstIndex = UINT_MAX;
		unsigned indexCount = 0;
		unsigned stateIndex = UINT_MAX;
		unsigned transformIndex = 0;

		bool try_merging_with( const draw_call &dc )
		{
			if (
			    stateIndex != dc.stateIndex ||
			    transformIndex != dc.transformIndex ||
			    primitive != dc.primitive ||
			    ( firstIndex + indexCount ) != dc.firstIndex )
				return false;

			indexCount += dc.indexCount;
			return true;
		}
	};

	std::vector<draw_call> _drawCalls;

	draw_call _currentDrawCall;

	struct compact_gpu_vertex
	{
		vec4 posColor; // xyz = world position, w = 32bit RGBA color (converted using floatBitsToUint)
		uvec4 data;    // xy = UV coord, zw = 16bits texture index, 4x 12bits scissor rect.

		GL3D_LAYOUT( 0, &compact_gpu_vertex::posColor, 1, &compact_gpu_vertex::data )
	};

	float _currentColor;
	uvec4 _currentData;

	std::vector<compact_gpu_vertex> _vertices;
	std::vector<unsigned> _indices;
	std::vector<mat4> _transforms;

	decltype( _vertices )::iterator _currentVertex;
	unsigned _startVertex = UINT_MAX;

	std::vector<uint64_t> _textureHandles;
	std::unordered_map<texture::ptr, unsigned> _textureIndexMap;

	buffer::ptr _vertexBuffer;
	buffer::ptr _indexBuffer;
	shader::ptr _shader;
};

} // namespace gl3d

#endif // __GL3D_QUICK_DRAW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef GL3D_IMPLEMENTATION
	#ifndef __GL3D_QUICK_DRAW_H_IMPL__
		#define __GL3D_QUICK_DRAW_H_IMPL__
		#include "gl3d_quick_draw.inl"
	#endif // __GL3D_QUICK_DRAW_H_IMPL__
#endif // GL3D_IMPLEMENTATION
