#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl.h"

void nlog(const char *fmt, ...);

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

void gl_init()
{
	glEnable(GL_DEPTH_TEST);
	
	shader = gl_make_shader("fshade.txt", "vshade.txt");
}
