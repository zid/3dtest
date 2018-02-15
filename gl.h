#ifndef GL_H
#define GL_H
#include <windows.h>
#include <gl/gl.h>
#include <stddef.h>
#include "model.h"

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW          0x88E4
#define GL_ARRAY_BUFFER         0x8892
#define GL_FRAGMENT_SHADER      0x8B30
#define GL_VERTEX_SHADER        0x8B31
#define FIRST_VERTEX_CONVENTION 0x8E4D
#define LAST_VERTEX_CONVENTION  0x8E4E

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

struct object
{
	GLuint *vao, shader, *tex;
	GLint scale, rot4vf, persp, trans;

	struct model *m;
};

void rot(float *, float, float, float, float);
void init_gl_procs(void);
void initgl(HWND);
void render(void);
void load_model(struct object *, const char *);
#endif