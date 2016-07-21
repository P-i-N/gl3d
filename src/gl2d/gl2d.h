#ifndef __GL2D_H__
#define __GL2D_H__

#include <vector>

#if !defined(GL2D_APIENTRY)
#if defined(WIN32)
#define GL2D_APIENTRY __stdcall
#else
#define GL2D_APIENTRY
#endif
#endif

#if defined(WIN32)
#include <windows.h>
#else
#endif

#include <gl/GL.h>

namespace gl2d {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec2
{
  float x = 0.0f, y = 0.0f;

  vec2() { }
  vec2(const vec2 &copy): x(copy.x), y(copy.y) { }

  template <typename T>
  vec2(T v): x(static_cast<float>(v)), y(static_cast<float>(v)) { }

  template <typename T>
  vec2(T _x, T _y): x(static_cast<float>(_x)), y(static_cast<float>(_y)) { }

  float *data() { return &x; }
  const float *data() const { return &x; }

  vec2 operator+(const vec2 &v) const { return vec2(x + v.x, y + v.y); }
  vec2 operator-(const vec2 &v) const { return vec2(x - v.x, y - v.y); }
  vec2 operator*(const vec2 &v) const { return vec2(x * v.x, y * v.y); }
  vec2 operator/(const vec2 &v) const { return vec2(x / v.x, y / v.y); }

  vec2 operator+(float v) const { return vec2(x + v, y + v); }
  vec2 operator-(float v) const { return vec2(x - v, y - v); }
  vec2 operator*(float v) const { return vec2(x * v, y * v); }
  vec2 operator/(float v) const { return vec2(x / v, y / v); }

  friend vec2 operator+(float v, const vec2 &vec) { return vec2(v + vec.x, v + vec.y); }
  friend vec2 operator-(float v, const vec2 &vec) { return vec2(v - vec.x, v - vec.y); }
  friend vec2 operator*(float v, const vec2 &vec) { return vec2(v * vec.x, v * vec.y); }
  friend vec2 operator/(float v, const vec2 &vec) { return vec2(v / vec.x, v / vec.y); }

  vec2 &operator+=(const vec2 &v) { x += v.x; y += v.y; return *this; }
  vec2 &operator-=(const vec2 &v) { x -= v.x; y -= v.y; return *this; }
  vec2 &operator*=(const vec2 &v) { x *= v.x; y *= v.y; return *this; }
  vec2 &operator/=(const vec2 &v) { x /= v.x; y /= v.y; return *this; }

  vec2 &operator+=(float v) { x += v; y += v; return *this; }
  vec2 &operator-=(float v) { x -= v; y -= v; return *this; }
  vec2 &operator*=(float v) { x *= v; y *= v; return *this; }
  vec2 &operator/=(float v) { x /= v; y /= v; return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rect
{
  vec2 min, max;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rgba_color
{
  float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;

  rgba_color() { }

  rgba_color(const rgba_color &copy)
    : r(copy.r)
    , g(copy.g)
    , b(copy.b)
    , a(copy.a)
  {
    
  }

  rgba_color(float _r, float _g, float _b, float _a = 1.0f)
    : r(_r)
    , g(_g)
    , b(_b)
    , a(_a)
  {
    
  }

  rgba_color(uint32_t argb)
    : r(((argb >> 16) & 0xFFu) / 255.0f)
    , g(((argb >>  8) & 0xFFu) / 255.0f)
    , b(((argb      ) & 0xFFu) / 255.0f)
    , a(((argb >> 24) & 0xFFu) / 255.0f)
  {
    
  }
};

namespace detail {
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *vertex_shader_code = R"GLSHADER(
#version 330

layout(location = 0) in vec2 vert_Position;
layout(location = 1) in vec4 vert_Color;
layout(location = 2) in vec2 vert_UV;

uniform vec2 u_ScreenSize;

out vec4 Color;
out vec2 UV;

void main()
{
  vec2 clipPos = (vert_Position / u_ScreenSize);
  clipPos.y = 1.0 - clipPos.y;
  clipPos = clipPos * 2.0 - 1.0;

  gl_Position = vec4(clipPos, 1, 1);
  Color = vert_Color;
  UV = vert_UV;
}
)GLSHADER";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char *fragment_shader_code = R"GLSHADER(
#version 330

uniform sampler2D u_FontTexture;

in vec4 Color;
in vec2 UV;

out vec4 out_Color;

void main()
{
  out_Color = texture(u_FontTexture, UV) * Color;
}
)GLSHADER";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
static const unsigned long long ibm_font[96] =
{
  0x0000000000000000ull, 0x183C3C1818001800ull, 0x3636000000000000ull, 0x36367F367F363600ull, 0x0C3E031E301F0C00ull, 0x006333180C666300ull, 0x1C361C6E3B336E00ull, 0x0606030000000000ull,
  0x180C0606060C1800ull, 0x060C1818180C0600ull, 0x00663CFF3C660000ull, 0x000C0C3F0C0C0000ull, 0x00000000000C0C06ull, 0x0000003F00000000ull, 0x00000000000C0C00ull, 0x6030180C06030100ull,
  0x3E63737B6F673E00ull, 0x0C0E0C0C0C0C3F00ull, 0x1E33301C06333F00ull, 0x1E33301C30331E00ull, 0x383C36337F307800ull, 0x3F031F3030331E00ull, 0x1C06031F33331E00ull, 0x3F3330180C0C0C00ull,
  0x1E33331E33331E00ull, 0x1E33333E30180E00ull, 0x000C0C00000C0C00ull, 0x000C0C00000C0C06ull, 0x180C0603060C1800ull, 0x00003F00003F0000ull, 0x060C1830180C0600ull, 0x1E3330180C000C00ull,
  0x3E637B7B7B031E00ull, 0x0C1E33333F333300ull, 0x3F66663E66663F00ull, 0x3C66030303663C00ull, 0x1F36666666361F00ull, 0x7F46161E16467F00ull, 0x7F46161E16060F00ull, 0x3C66030373667C00ull,
  0x3333333F33333300ull, 0x1E0C0C0C0C0C1E00ull, 0x7830303033331E00ull, 0x6766361E36666700ull, 0x0F06060646667F00ull, 0x63777F7F6B636300ull, 0x63676F7B73636300ull, 0x1C36636363361C00ull,
  0x3F66663E06060F00ull, 0x1E3333333B1E3800ull, 0x3F66663E36666700ull, 0x1E33070E38331E00ull, 0x3F2D0C0C0C0C1E00ull, 0x3333333333333F00ull, 0x33333333331E0C00ull, 0x6363636B7F776300ull,
  0x6363361C1C366300ull, 0x3333331E0C0C1E00ull, 0x7F6331184C667F00ull, 0x1E06060606061E00ull, 0x03060C1830604000ull, 0x1E18181818181E00ull, 0x081C366300000000ull, 0x00000000000000FFull,
  0x0C0C180000000000ull, 0x00001E303E336E00ull, 0x0706063E66663B00ull, 0x00001E3303331E00ull, 0x3830303e33336E00ull, 0x00001E333f031E00ull, 0x1C36060f06060F00ull, 0x00006E33333E301Full,
  0x0706366E66666700ull, 0x0C000E0C0C0C1E00ull, 0x300030303033331Eull, 0x070666361E366700ull, 0x0E0C0C0C0C0C1E00ull, 0x0000337F7F6B6300ull, 0x00001F3333333300ull, 0x00001E3333331E00ull,
  0x00003B66663E060Full, 0x00006E33333E3078ull, 0x00003B6E66060F00ull, 0x00003E031E301F00ull, 0x080C3E0C0C2C1800ull, 0x0000333333336E00ull, 0x00003333331E0C00ull, 0x0000636B7F7F3600ull,
  0x000063361C366300ull, 0x00003333333E301Full, 0x00003F190C263F00ull, 0x380C0C070C0C3800ull, 0x1818180018181800ull, 0x070C0C380C0C0700ull, 0x6E3B000000000000ull, 0x0000000000000000ull
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GL2D_API_FUNC(retValue, name, ...) \
  typedef retValue(GL2D_APIENTRY *gl_ ## name ## _ptr_t)(__VA_ARGS__); \
  gl_ ## name ## _ptr_t name = reinterpret_cast<gl_ ## name ## _ptr_t>(get_gl_proc_address("gl" ## #name));

#define GL2D_API_FUNC_INIT(name) \
  name = reinterpret_cast<gl_ ## name ## _ptr_t>(get_gl_proc_address("gl" ## #name)); \
  if (name == nullptr) return false

//---------------------------------------------------------------------------------------------------------------------
struct gl_api
{
  static void *get_gl_proc_address(const char *funcName) { return wglGetProcAddress(funcName); }

  GL2D_API_FUNC(void, GenBuffers, GLsizei, GLuint *)
  GL2D_API_FUNC(void, DeleteBuffers, GLsizei, const GLuint *)
  GL2D_API_FUNC(void, BindBuffer, GLenum, GLuint)
  GL2D_API_FUNC(void, BufferData, GLenum, ptrdiff_t, const GLvoid *, GLenum)
  GL2D_API_FUNC(void, GenVertexArrays, GLsizei, GLuint *)
  GL2D_API_FUNC(void, BindVertexArray, GLuint)
  GL2D_API_FUNC(void, EnableVertexAttribArray, GLuint)
  GL2D_API_FUNC(void, VertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *)
  GL2D_API_FUNC(void, BindAttribLocation, GLuint, GLuint, const char *)
  GL2D_API_FUNC(void, DeleteVertexArrays, GLsizei, const GLuint *)
  GL2D_API_FUNC(GLuint, CreateShader, GLenum)
  GL2D_API_FUNC(void, DeleteShader, GLuint)
  GL2D_API_FUNC(void, ShaderSource, GLuint, GLsizei, const char **, const GLint *)
  GL2D_API_FUNC(void, CompileShader, GLuint)
  GL2D_API_FUNC(void, GetShaderiv, GLuint, GLenum, GLint *)
  GL2D_API_FUNC(GLuint, CreateProgram)
  GL2D_API_FUNC(void, DeleteProgram, GLuint)
  GL2D_API_FUNC(void, AttachShader, GLuint, GLuint)
  GL2D_API_FUNC(void, DetachShader, GLuint, GLuint)
  GL2D_API_FUNC(void, LinkProgram, GLuint)
  GL2D_API_FUNC(void, UseProgram, GLuint)
  GL2D_API_FUNC(void, GetProgramiv, GLuint, GLenum, GLint *)
  GL2D_API_FUNC(GLint, GetUniformLocation, GLuint, const char *)
  GL2D_API_FUNC(void, Uniform1i, GLint, GLint)
  GL2D_API_FUNC(void, Uniform2fv, GLint, GLsizei, const GLfloat *)
  GL2D_API_FUNC(void, ActiveTexture, GLenum)
    
  static const GLenum CLAMP_TO_EDGE = 0x812F;
  static const GLenum TEXTURE0 = 0x84C0;
  static const GLenum ARRAY_BUFFER = 0x8892;
  static const GLenum STREAM_DRAW = 0x88E0;
  static const GLenum FRAGMENT_SHADER = 0x8B30;
  static const GLenum VERTEX_SHADER = 0x8B31;
  static const GLenum COMPILE_STATUS = 0x8B81;
  static const GLenum LINK_STATUS = 0x8B82;

  gl_api()
  {
    
  }

  ~gl_api()
  {
    
  }

  bool init()
  {
    auto funcPtr = reinterpret_cast<size_t *>(this);
    for (size_t i = 0; i < sizeof(gl_api) / sizeof(size_t *); ++i, ++funcPtr)
      if (!funcPtr)
        return false;

    return true;
  }

  GLuint compile_shader(GLenum shaderType, const char *source)
  {
    GLuint result = CreateShader(shaderType);
    ShaderSource(result, 1, &source, nullptr);
    CompileShader(result);

    GLint status;
    GetShaderiv(result, COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
      DeleteShader(result);
      return 0;
    }

    return result;
  }

  GLuint link_program(GLuint vert, GLuint frag)
  {
    GLuint result = CreateProgram();
    AttachShader(result, vert);
    AttachShader(result, frag);
    LinkProgram(result);

    GLint status;
    GetProgramiv(result, LINK_STATUS, &status);

    DetachShader(result, vert);
    DetachShader(result, frag);

    if (status == GL_FALSE)
    {
      DeleteProgram(result);
      return 0;
    }

    return result;
  }
};

#undef GL2D_API_FUNC
#undef GL2D_API_FUNC_INIT

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vertex
{
  vec2 pos;
  rgba_color rgba_color;
  vec2 uv;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct draw_call
{
  bool triangles; // true for triangles, false for lines
  size_t length;  // number of vertices

  draw_call(bool tris, size_t len)
    : triangles(tris)
    , length(len)
  {
    
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct state
{
  rect viewport;
  rect scissors;
  rgba_color rgba_color;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class context
{
public:
  static context *current;

  context()
  {
    clear();
  }

  virtual ~context()
  {
    done();
  }
  
  bool initialized() const { return _initialized; }

  bool init()
  {
    using namespace detail;

    if (_initialized)
      return true;

    _gl = new gl_api();

    if (!_gl->init())
      return false;

    _initialized = true;

    _gl->GenBuffers(1, &_vbo);
    _gl->GenVertexArrays(1, &_vao);

    // Initialize VBO and VAO
    {
      _gl->BindBuffer(gl_api::ARRAY_BUFFER, _vbo);
      _gl->BindVertexArray(_vao);
      _gl->EnableVertexAttribArray(0);
      _gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
      _gl->EnableVertexAttribArray(1);
      _gl->VertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, rgba_color)));
      _gl->EnableVertexAttribArray(2);
      _gl->VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, uv)));
      _gl->BindVertexArray(0);
      _gl->BindBuffer(gl_api::ARRAY_BUFFER, 0);
    }

    _vertShader = _gl->compile_shader(gl_api::VERTEX_SHADER, vertex_shader_code);
    if (!_vertShader)
    {
      done();
      return false;
    }

    _fragShader = _gl->compile_shader(gl_api::FRAGMENT_SHADER, fragment_shader_code);
    if (!_fragShader)
    {
      done();
      return false;
    }

    _program = _gl->link_program(_vertShader, _fragShader);
    if (!_program)
    {
      done();
      return false;
    }

    _uScreenSize = _gl->GetUniformLocation(_program, "u_ScreenSize");
    _uFontTexture = _gl->GetUniformLocation(_program, "u_FontTexture");

    // Initialize font texture
    {
      uint32_t *image = new uint32_t[256 * 128];
      memset(image, 0, 256 * 128 * sizeof(uint32_t));
      
      for (int ch = 0; ch < 96; ++ch)
      {
        uint32_t *charPixel = image + ((ch % 16) * 16) + ((ch / 16) * 16 * 256);
        auto charBits = detail::ibm_font[ch];

        for (int y = 0; y < 8; ++y, charPixel += 256)
        {
          for (int x = 0; x < 8; ++x, charBits >>= 1)
          {
            if (charBits & 1)
            {
              charPixel[x] = 0xFFFFFFFFu;
              *(charPixel + x + 256 + 1) = 0xFF000000u;
            }
          }
        }
      }

      image[256 * 128 - 1] = 0xFFFFFFFFu;

      glGenTextures(1, &_fontTexture);
      glBindTexture(GL_TEXTURE_2D, _fontTexture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_api::CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_api::CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_2D, 0);

      delete [] image;
    }
    
    return true;
  }

  void done()
  {
    if (!_initialized)
      return;

    if (_vbo)
    {
      _gl->DeleteBuffers(1, &_vbo);
      _vbo = 0;
    }

    if (_vao)
    {
      _gl->DeleteVertexArrays(1, &_vao);
      _vao = 0;
    }

    if (_vertShader)
    {
      _gl->DeleteShader(_vertShader);
      _vertShader = 0;
    }

    if (_fragShader)
    {
      _gl->DeleteShader(_fragShader);
      _fragShader = 0;
    }

    if (_program)
    {
      _gl->DeleteProgram(_program);
      _program = 0;
    }

    if (_fontTexture)
    {
      glDeleteTextures(1, &_fontTexture);
      _fontTexture = 0;
    }

    delete _gl;
    _gl = nullptr;
    _initialized = false;
  }

  void clear()
  {
    _drawCalls.clear();
    _drawCalls.emplace_back(true, 0);
    _vertexCursor = 0;
    _state.rgba_color = gl2d::rgba_color(1, 1, 1, 1);
  }

  void color(const rgba_color &c) { _state.rgba_color = c; }

  void color(float r, float g, float b, float a = 1.0f) { _state.rgba_color = rgba_color(r, g, b, a); }

  void color(uint32_t argb) { _state.rgba_color = rgba_color(argb); }

  const rgba_color &color() const { return _state.rgba_color; }

  void line(const vec2 &a, const vec2 &b)
  {
    if (!_drawCalls.back().triangles)
      _drawCalls.back().length += 2;
    else
      _drawCalls.emplace_back(false, 2);
    
    auto *v = alloc_vertices(2);
    v->pos = a;
    v->rgba_color = _state.rgba_color;
    v->uv = vec2(1, 1);
    ++v;
    v->pos = b;
    v->rgba_color = _state.rgba_color;
    v->uv = vec2(1, 1);
  }

  void line(float x1, float y1, float x2, float y2) { line(vec2(x1, y1), vec2(x2, y2)); }

  void linei(int x1, int y1, int x2, int y2) { line(vec2(x1, y1), vec2(x2, y2)); }

  void rectangle(const vec2 &a, const vec2 &b, bool filled = false)
  {
    if (filled)
    {
      if (_drawCalls.back().triangles)
        _drawCalls.back().length += 6;
      else
        _drawCalls.emplace_back(true, 6);
      
      auto *v = alloc_vertices(6);
      v->pos = a;
      v->rgba_color = _state.rgba_color;
      v->uv = vec2(1, 1);
      ++v;
      v->pos = vec2(b.x, a.y);
      v->rgba_color = _state.rgba_color;
      v->uv = vec2(1, 1);
      ++v;
      v->pos = vec2(a.x, b.y);
      v->rgba_color = _state.rgba_color;
      v->uv = vec2(1, 1);
      
      v[1] = *v;
      v[2] = v[-1];
      v += 3;

      v->pos = vec2(b.x, b.y);
      v->rgba_color = _state.rgba_color;
      v->uv = vec2(1, 1);
    }
    else
    {
      line(a.x, a.y, b.x, a.y);
      line(b.x, a.y, b.x, b.y);
      line(b.x, b.y, a.x, b.y);
      line(a.x, b.y, a.x, a.y);
    }
  }

  void rectangle(float x1, float y1, float x2, float y2, bool filled = false) { rectangle(vec2(x1, y1), vec2(x2, y2), filled); }

  void rectanglei(int x1, int y1, int x2, int y2, bool filled = false)  { rectangle(vec2(x1, y1), vec2(x2, y2), filled); }
  
  void text(const vec2 &pos, const char *fmt, va_list &ap)
  {
    if (!fmt || !(*fmt)) return;

    char buff[1024];
    size_t length = static_cast<size_t>(vsnprintf(buff, 1024, fmt, ap));
    if (!length) return;

    float x = pos.x;
    float y = pos.y;
    const char *text = buff;
    const char *end = text + length;
    gl2d::rgba_color rgba_color = _state.rgba_color;

    while (text < end)
    {
      const char *mark = strchr(text, '^');
    
      if (!mark)
        mark = end;

      int partLen = mark - text;

      if (partLen)
        print_substring(x, y, rgba_color, text, partLen);

      if (mark[0] == '^')
      {
        static const char *colorString = "0123456789abcdef";
        const char *colorChar = strchr(colorString, tolower(mark[1]));
      
        if (colorChar)
        {
          const float q = 0.25f;
          const float h = 0.5f;

          switch (*colorChar)
          {
            case '0': rgba_color = gl2d::rgba_color(0, 0, 0, 1); break;
            case '1': rgba_color = gl2d::rgba_color(0, 0, h, 1); break;
            case '2': rgba_color = gl2d::rgba_color(0, h, 0, 1); break;
            case '3': rgba_color = gl2d::rgba_color(0, h, h, 1); break;
            case '4': rgba_color = gl2d::rgba_color(h, 0, 0, 1); break;
            case '5': rgba_color = gl2d::rgba_color(h, 0, h, 1); break;
            case '6': rgba_color = gl2d::rgba_color(h, h, 0, 1); break;
            case '7': rgba_color = gl2d::rgba_color(h, h, h, 1); break;
            case '8': rgba_color = gl2d::rgba_color(q, q, q, 1); break;
            case '9': rgba_color = gl2d::rgba_color(q, h, 1, 1); break;
            case 'a': rgba_color = gl2d::rgba_color(q, 1, q, 1); break;
            case 'b': rgba_color = gl2d::rgba_color(q, 1, 1, 1); break;
            case 'c': rgba_color = gl2d::rgba_color(1, h, q, 1); break;
            case 'd': rgba_color = gl2d::rgba_color(1, q, 1, 1); break;
            case 'e': rgba_color = gl2d::rgba_color(1, 1, 0, 1); break;

            default:
            case 'f': rgba_color = gl2d::rgba_color(1, 1, 1, 1); break;
          }
        }
        else
          ++partLen;
      }

      text = mark + 2;
    }
  }

  void text(float x, float y, const char *fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    text(vec2(x, y), fmt, ap);
    va_end(ap);
  }

  void texti(int x, int y, const char *fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    text(vec2(x, y), fmt, ap);
    va_end(ap);
  }

  void render(int x, int y, int width, int height)
  {
    using namespace detail;

    glViewport(x, y, width, height);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Copy vertices into VBO
    _gl->BindBuffer(gl_api::ARRAY_BUFFER, _vbo);
    _gl->BufferData(gl_api::ARRAY_BUFFER, sizeof(vertex) * _vertexCursor, _vertices.data(), gl_api::STREAM_DRAW);
    _gl->BindVertexArray(_vao);
    _gl->UseProgram(_program);
    _gl->BindAttribLocation(_program, 0, "vert_Position");
    _gl->BindAttribLocation(_program, 1, "vert_Color");
    _gl->BindAttribLocation(_program, 2, "vert_UV");

    _gl->ActiveTexture(gl_api::TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _fontTexture);

    vec2 screenSize(width, height);
    _gl->Uniform2fv(_uScreenSize, 1, screenSize.data());
    _gl->Uniform1i(_uFontTexture, 0);

    size_t startVertex = 0;
    for (auto &&dc : _drawCalls)
    {
      if (!dc.length) continue;

      glDrawArrays(dc.triangles ? GL_TRIANGLES : GL_LINES, static_cast<GLint>(startVertex), static_cast<GLsizei>(dc.length));
      startVertex += dc.length;
    }

    _gl->BindBuffer(gl_api::ARRAY_BUFFER, 0);
    _gl->BindVertexArray(0);
    _gl->UseProgram(0);

    clear(); 
  }

  void render(int width, int height) { render(0, 0, width, height); }

private:
  detail::vertex *alloc_vertices(size_t count)
  {
    if (_vertexCursor + count > _vertices.size())
      _vertices.resize(_vertexCursor + count);

    auto result = _vertices.data() + _vertexCursor;
    _vertexCursor += count;

    return result;
  }

  void print_substring(float &x, float &y, const gl2d::rgba_color &rgba_color, const char *text, size_t length)
  {
    if (_drawCalls.back().triangles)
      _drawCalls.back().length += length * 6;
    else
      _drawCalls.emplace_back(true, length * 6);

    size_t skippedChars = 0;
    auto *v = alloc_vertices(length * 6);

    const float uvStepX = 16.0f / 256.0f;
    const float uvStepY = 16.0f / 128.0f;
    const float uvW = 8.0f / 256.0f;
    const float uvH = 8.0f / 128.0f;

    while (length)
    {
      char ch = *text;
      if (ch > 32)
      {
        ch -= 32;
        float uvX = (ch % 16) * uvStepX;
        float uvY = (ch / 16) * uvStepY;

        v->pos = vec2(x, y);
        v->rgba_color = rgba_color;
        v->uv = vec2(uvX, uvY + uvH);
        ++v;
        v->pos = vec2(x + 8, y);
        v->rgba_color = rgba_color;
        v->uv = vec2(uvX + uvW, uvY + uvH);
        ++v;
        v->pos = vec2(x, y + 8);
        v->rgba_color = rgba_color;
        v->uv = vec2(uvX, uvY);

        v[1] = *v;
        v[2] = v[-1];
        v += 3;

        v->pos = vec2(x + 8, y + 8);
        v->rgba_color = rgba_color;
        v->uv = vec2(uvX + uvW, uvY);
        ++v;
      }
      else
        ++skippedChars;

      x += 8;
      ++text;
      --length;
    }

    _vertexCursor -= skippedChars * 6;
  }

  bool _initialized = false;

  detail::gl_api *_gl = nullptr;

  detail::state _state;

  GLuint _vbo = 0;
  
  GLuint _vao = 0;

  GLuint _vertShader = 0;

  GLuint _fragShader = 0;

  GLuint _program = 0;

  GLint _uScreenSize = -1;

  GLint _uFontTexture = -1;

  GLuint _fontTexture = 0;

  std::vector<detail::vertex> _vertices;

  size_t _vertexCursor = 0;

  std::vector<detail::draw_call> _drawCalls;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
void color(const rgba_color &c)
{
  if (context::current)
    context::current->color(c);
}

//---------------------------------------------------------------------------------------------------------------------
void color(float r, float g, float b, float a = 1.0f)
{
  if (context::current)
    context::current->color(r, g, b, a);
}

//---------------------------------------------------------------------------------------------------------------------
void color(uint32_t argb)
{
  if (context::current)
    context::current->color(argb);
}

//---------------------------------------------------------------------------------------------------------------------
void line(const vec2 &a, const vec2 &b)
{
  if (context::current)
    context::current->line(a, b);
}

//---------------------------------------------------------------------------------------------------------------------
void line(float x1, float y1, float x2, float y2)
{
  if (context::current)
    context::current->line(x1, y1, x2, y2);
}

//---------------------------------------------------------------------------------------------------------------------
void linei(int x1, int y1, int x2, int y2)
{
  if (context::current)
    context::current->linei(x1, y1, x2, y2);
}

//---------------------------------------------------------------------------------------------------------------------
void rectangle(const vec2 &a, const vec2 &b, bool filled = false)
{
  if (context::current)
    context::current->rectangle(a, b, filled);
}

//---------------------------------------------------------------------------------------------------------------------
void rectangle(float x1, float y1, float x2, float y2, bool filled = false)
{
  if (context::current)
    context::current->rectangle(x1, y1, x2, y2, filled);
}

//---------------------------------------------------------------------------------------------------------------------
void rectanglei(int x1, int y1, int x2, int y2, bool filled = false)
{
  if (context::current)
    context::current->rectanglei(x1, y1, x2, y2, filled);
}

//---------------------------------------------------------------------------------------------------------------------
void text(const vec2 &pos, const char *fmt, va_list &ap)
{
  if (context::current)
    context::current->text(pos, fmt, ap);
}

//---------------------------------------------------------------------------------------------------------------------
void text(float x, float y, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  text(vec2(x, y), fmt, ap);
  va_end(ap);
}

//---------------------------------------------------------------------------------------------------------------------
void text(int x, int y, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  text(vec2(x, y), fmt, ap);
  va_end(ap);
}

}

#endif // __GL2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef GL2D_IMPLEMENTATION
#ifndef __GL2D_H_IMPL__
#define __GL2D_H_IMPL__

namespace gl2d {
  
context *context::current = nullptr;

}

#endif // __GL2D_H_IMPL__
#endif // GL2D_IMPLEMENTATION
