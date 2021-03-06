#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "gl.h"

void nlog(const char *fmt, ...);

static HGLRC gl;
GLuint shader;

static char *load_txt(const char *name)
{
	FILE *f;
	size_t len;
	char *s;

	f = fopen(name, "rb");
	if(!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);

	s = malloc(len+1);
	fread(s, 1, len, f);

	s[len] = 0;

	fclose(f);

	return s;
}

static void gl_init_procs(void)
{
	glBindBuffer = (void *)wglGetProcAddress("glBindBuffer");
	glGenBuffers = (void *)wglGetProcAddress("glGenBuffers");
	glBufferData = (void *)wglGetProcAddress("glBufferData");
	glGenVertexArrays = (void *)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (void *)wglGetProcAddress("glBindVertexArray");
	glVertexAttribPointer = (void *)wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (void *)wglGetProcAddress("glEnableVertexAttribArray");
	glCreateShader = (void *)wglGetProcAddress("glCreateShader");
	glShaderSource = (void *)wglGetProcAddress("glShaderSource");
	glCompileShader = (void *)wglGetProcAddress("glCompileShader");
	glCreateProgram = (void *)wglGetProcAddress("glCreateProgram");
	glUseProgram = (void *)wglGetProcAddress("glUseProgram");
	glLinkProgram = (void *)wglGetProcAddress("glLinkProgram");
	glAttachShader = (void *)wglGetProcAddress("glAttachShader");
	glUniform1f = (void *)wglGetProcAddress("glUniform1f");
	glGetUniformLocation = (void *)wglGetProcAddress("glGetUniformLocation");
	glUniformMatrix4fv = (void *)wglGetProcAddress("glUniformMatrix4fv");
	glProgramUniform1f = (void *)wglGetProcAddress("glProgramUniform1f");
	glGetShaderInfoLog = (void *)wglGetProcAddress("glGetShaderInfoLog");
	wglSwapIntervalEXT = (void *)wglGetProcAddress("wglSwapIntervalEXT");
	glGenerateMipmap = (void *)wglGetProcAddress("glGenerateMipmap");
}

void gl_destroy(void)
{
	wglDeleteContext(gl);
}

GLint gl_get_uniform(const char *name)
{
	return glGetUniformLocation(shader, name);
}

GLuint gl_make_shader(const char *fpath, const char *vpath)
{
	GLuint shader, vshade, fshade;
	char *vshade_src, *fshade_src;
	vshade_src = load_txt(vpath);
	if(!vshade_src)
	{
		nlog("Unable to open vertex shader");
		exit(EXIT_FAILURE);
	}

	vshade = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshade, 1, (const char **)&vshade_src, NULL);
	glCompileShader(vshade);
	free(vshade_src);
	
	fshade_src = load_txt(fpath);
	if(!fshade_src)
	{
		nlog("Unable to open fragment shader");
		exit(EXIT_FAILURE);
	}

	fshade = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshade, 1, (const char **)&fshade_src, NULL);
	glCompileShader(fshade);
	free(fshade_src);

	shader = glCreateProgram();
	glAttachShader(shader, vshade);
	glAttachShader(shader, fshade);
	glLinkProgram(shader);

	return shader;
}

void gl_init(void *win)
{
	HDC h;
	int nformat;
	HWND hwnd = *(HWND *)win;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	};

	h = GetWindowDC(hwnd);
	nformat = ChoosePixelFormat(h, &pfd);
	SetPixelFormat(h, nformat, &pfd); 
	gl = wglCreateContext(h);
	if(!gl)
	{
		nlog("Unable to create OpenGL context");
		exit(EXIT_FAILURE);
	}

	wglMakeCurrent(h, gl);
	gl_init_procs();
	wglSwapIntervalEXT(1);
	glEnable(GL_DEPTH_TEST);
	
	shader = gl_make_shader("fshade.txt", "vshade.txt");
}
