#ifndef __GL3D_2D_H__
#define __GL3D_2D_H__

#include <map>

#include "gl3d.h"

namespace gl3d {

struct font
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

class immediate
{
public:
	using ptr = std::shared_ptr<immediate>;

	immediate();

	virtual ~immediate();

	void reset();

	void render( cmd_queue::ptr queue, const mat4 &view, const mat4 &projection );

	void begin( gl_enum primitiveType, bool autoClose = false );

	void end();

	void vertex( const vec3 &pos );

	void vertex( const vec2 &pos, float z = 0.0f ) { vertex( { pos.x, pos.y, z } ); }

protected:
	bool _dirtyBuffers = true;

	struct draw_call
	{
		gl_enum primitive = gl_enum::NONE;
		unsigned startVertex = UINT_MAX;
		unsigned startIndex = UINT_MAX;
		unsigned vertexCount = 0;
		unsigned indexCount = 0;
		unsigned stateIndex = UINT_MAX;
	};

	std::vector<draw_call> _drawCalls;

	struct gpu_vertex
	{
		vec3 pos;
		byte_vec4 color;
		vec2 uv;

		GL3D_LAYOUT( 0, &gpu_vertex::pos, 1, &gpu_vertex::color, 2, &gpu_vertex::uv )
	};

	std::vector<gpu_vertex> _vertices;
	std::vector<unsigned> _indices;

	buffer::ptr _vertexBuffer;
	buffer::ptr _indexBuffer;
};

} // namespace gl3d

#endif // __GL3D_2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef GL3D_IMPLEMENTATION
	#ifndef __GL3D_2D_H_IMPL__
		#define __GL3D_2D_H_IMPL__
		#include "gl3d_2d.inl"
	#endif // __GL3D_2D_H_IMPL__
#endif // GL3D_IMPLEMENTATION
