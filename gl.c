#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "gl.h"

void nlog(const char *fmt, ...);

static HGLRC gl;

float rotx = 0, roty = 0, rotz = 0;

static const float persp_mat[] = {
	1.0/tan(M_PI/4)/1.333, 0.0, 0.0, 0.0,
	0.0, 1.0/tan(M_PI/4), 0.0, 0.0,
	0.0, 0.0, -(8192+0.1)/(8192-0.1), -(2.0*8192*0.1)/(8192-0.1),
	0.0, 0.0, -1.0, 0.0
};

static const float identity[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
static struct object model[2];

static void load_ident(float *f, float a, float b)
{
	f[0] = 1;
	f[1] = 0;
	f[2] = 0;
	f[3] = 0;
	
	f[4] = 0;
	f[5] = 1;
	f[6] = 0;
	f[7] = b;

	f[8] = 0;
	f[9] = 0;
	f[10] = 1;
	f[11] = a;
	
	f[12] = 0;
	f[13] = 0;
	f[14] = 0;
	f[15] = 1;
}

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

static void free_txt(char *buf)
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
static void render_object(struct object *o, int depth, float scale, float height, int dorot)
{
	float f[16];
	unsigned int i;

	glUseProgram(o->shader);

	rot(f, 1.0f, rotx, roty, rotz);
	if(dorot)
		glUniformMatrix4fv(o->rot4vf, 1, GL_TRUE, f);
	else
		glUniformMatrix4fv(o->rot4vf, 1, GL_TRUE, identity);

	load_ident(f, depth, height);
	glUniform1f(o->scale, scale);
	glUniformMatrix4fv(o->trans, 1, GL_TRUE, f);
	glUniformMatrix4fv(o->persp, 1, GL_TRUE, persp_mat);

	for(i = 0; i < o->m->nmeshes; i++)
	{
		glBindVertexArray(o->vao[i]);
		glBindTexture(GL_TEXTURE_2D, o->tex[i]);
		glDrawElements(GL_TRIANGLES,
			o->m->meshes[i]->nelements * 3, o->m->meshes[i]->ele_size == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
	}
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_object(&model[0], -20, 4.68, 0, 1);
}

void load_model(struct object *o, const char *binpath)
{
	struct png *p;
	GLuint vbo[2];
	unsigned int i, n;

	o->m = load_bin(binpath);
	if(!o->m)
	{
		nlog("Load_bin(%s) failed, exploding.", binpath);
		exit(1);
	}
	n = o->m->nmeshes;

	o->tex = malloc(sizeof (GLuint[n]));
	o->vao = malloc(sizeof (GLuint[n]));
	glGenTextures(n, o->tex);
	glGenVertexArrays(n, o->vao);

	for(i = 0; i < n; i++)
	{
		GLint format;
		GLsizeiptr data_size;

		p = &o->m->meshes[i]->p;
		glBindTexture(GL_TEXTURE_2D, o->tex[i]);
		format = p->format == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, p->w, p->h, 0, format, GL_UNSIGNED_BYTE, p->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindVertexArray(o->vao[i]);

		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float[8]) * o->m->meshes[i]->nverts, o->m->meshes[i]->verts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat[8]), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat[8]), (GLvoid *)(sizeof(GLfloat[3])));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat[8]), (GLvoid *)(sizeof(GLfloat[6])));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		data_size = o->m->meshes[i]->ele_size * o->m->meshes[i]->nelements * 3;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, m2ptr(o->m->meshes[i]), GL_STATIC_DRAW);

		png_kill(p);
	}
}

void gldestroy(void)
{
	wglDeleteContext(gl);
}

void initgl(HWND hwnd)
{
	static GLchar *vshade_src;
	static GLchar *fshade_src;
	GLuint shader, vshade, fshade;
	struct object *o;
	HDC h;
	int nformat;

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
	glEnable(GL_DEPTH_TEST);

	init_gl_procs();
	wglSwapIntervalEXT(1);

	vshade_src = load_txt("vshade.txt");
	if(!vshade_src)
	{
		nlog("Unable to open vertex shader");
		exit(EXIT_FAILURE);
	}

	vshade = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshade, 1, (const char **)&vshade_src, NULL);
	glCompileShader(vshade);
	free_txt(vshade_src);
	
	fshade_src = load_txt("fshade.txt");
	if(!fshade_src)
	{
		nlog("Unable to open fragment shader");
		exit(EXIT_FAILURE);
	}
	fshade = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshade, 1, (const char **)&fshade_src, NULL);
	glCompileShader(fshade);
	free_txt(fshade_src);

	shader = glCreateProgram();
	glAttachShader(shader, vshade);
	glAttachShader(shader, fshade);
	glLinkProgram(shader);

	o = &model[0];
	load_model(o, "bomb.bin");

	o->scale  = glGetUniformLocation(shader, "scale");
	o->rot4vf = glGetUniformLocation(shader, "rot");
	o->persp  = glGetUniformLocation(shader, "persp");
	o->trans  = glGetUniformLocation(shader, "trans");
	o->shader = shader;
}
