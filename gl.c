#include "gl.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>

void nlog(char *str, ...);

char *load_txt(const char *name)
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

void free_txt(char *buf)
{
	free(buf);
}

void rot(float *f, float a, float x, float y, float z)
{
	float xx, xy, xz, xw, yy, yz, yw, zz, zw;
	float len;
	
	len = sqrt(x * x + y * y + z * z + a * a);
	x/=len; y/=len; z/=len; a/=len;

	xx = x * x;
	xy = x * y;
	xz = x * z;
	xw = x * a;
	yy = y * y;
	yz = y * z;
	yw = y * a;
	zz = z * z;
	zw = z * a;
	
	f[0]  = 1 - 2 * (yy + zz);
	f[1]  =     2 * (xy - zw);
	f[2]  =     2 * (xz + yw);
	f[3]  = 0;
	f[4]  =     2 * (xy + zw);
	f[5]  = 1 - 2 * (xx + zz);
	f[6]  =     2 * (yz - xw);
	f[7]  = 0;
	f[8]  =     2 * (xz - yw);
	f[9]  =     2 * (yz + xw);
	f[10] = 1 - 2 * (xx + yy);
	f[11] = 0;
	f[12] = 0;
	f[13] = 0;
	f[14] = 0;
	f[15] = 1;
}

void init_gl_procs(void)
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