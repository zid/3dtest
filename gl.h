#ifndef GL_H
#define GL_H
#ifdef _WIN32
#include <GL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include <stddef.h>

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW          0x88E4
#define GL_ARRAY_BUFFER         0x8892
#define GL_FRAGMENT_SHADER      0x8B30
#define GL_VERTEX_SHADER        0x8B31
#define FIRST_VERTEX_CONVENTION 0x8E4D
#define LAST_VERTEX_CONVENTION  0x8E4E

#ifdef _WIN32
#define ABI __attribute__((stdcall))

ABI void   (*glBindBuffer)(GLenum, GLuint);
ABI void   (*glGenBuffers)(GLsizei, GLuint *);
ABI void   (*glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
ABI void   (*glGenVertexArrays)(GLsizei, GLuint *);
ABI void   (*glBindVertexArray)(GLuint);
ABI void   (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
ABI void   (*glEnableVertexAttribArray)(GLuint);
ABI GLuint (*glCreateShader)(GLenum);
ABI void   (*glShaderSource)(GLuint, GLsizei, const GLchar **, const GLint *);
ABI void   (*glCompileShader)(GLuint);
ABI GLuint (*glCreateProgram)(void);
ABI void   (*glUseProgram)(GLuint);
ABI void   (*glLinkProgram)(GLuint);
ABI void   (*glAttachShader)(GLuint, GLuint);
ABI GLint  (*glGetUniformLocation)(GLuint, const GLchar *);
ABI void   (*glUniform1f)(GLint, GLfloat);
ABI void   (*glProgramUniform1f)(GLuint, GLint, GLfloat);
ABI void   (*glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *);
ABI void   (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
ABI void   (*wglSwapIntervalEXT)(GLuint);
ABI void   (*glGenerateMipmap)(GLenum);
#endif

void gl_destroy(void);
GLuint gl_make_shader(const char *, const char *);
GLint gl_get_uniform(const char *);
void gl_init();

#endif
