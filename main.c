#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "gl.h"
#include "model.h"

enum {
	MONKEY = 0,
	CUBE
};

struct object
{
	GLuint *vao, shader, *tex;
	GLint scale, rot4vf, persp, trans;

	struct model *m;
};

struct object model[2];

void nlog(char *str, ...)
{
	HWND notepad, edit;
	va_list ap;
	char buf[256];

	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	strcat(buf, "\r\n");
	notepad = FindWindow(NULL, "Untitled - Notepad");
	edit = FindWindowEx(notepad, NULL, "EDIT", NULL);
	SendMessage(edit, EM_REPLACESEL, TRUE, (LPARAM)buf);
}


static float rotx, roty, rotz;
static const float persp_mat[] = {
	1.0/tan(M_PI/4)/1.333, 0.0, 0.0, 0.0, 
	0.0, 1.0/tan(M_PI/4), 0.0, 0.0, 
	0.0, 0.0, -(8192+0.1)/(8192-0.1), -(2.0*8192*0.1)/(8192-0.1), 
	0.0, 0.0, -1.0, 0.0 };

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

static float identity[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

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
			o->m->meshes[i]->nelements * 3, o->m->meshes[i]->ele_type ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
	}
}

static void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	render_object(&model[CUBE], -45, 50, 4, 0);
	render_object(&model[MONKEY], -20, 4.68, 0, 1);
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
		p = &o->m->meshes[i]->p;
		glBindTexture(GL_TEXTURE_2D, o->tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p->w, p->h, 0, p->format == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, p->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			(o->m->meshes[i]->ele_type ? sizeof(int) : sizeof(short)) * 3 * o->m->meshes[i]->nelements, 
			m2ptr(o->m->meshes[i]), GL_STATIC_DRAW);

		png_kill(p);
	}
}

static void initgl(HWND hwnd)
{
	static GLchar *vshade_src;
	static GLchar *fshade_src;
	GLuint shader, vshade, fshade;
	struct object *o;

	HDC h;
	HGLRC gl;
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

	o = &model[MONKEY];
	load_model(o, "bomb.bin");

	o->scale  = glGetUniformLocation(shader, "scale");
	o->rot4vf = glGetUniformLocation(shader, "rot");
	o->persp  = glGetUniformLocation(shader, "persp");
	o->trans  = glGetUniformLocation(shader, "trans");
	o->shader = shader;
	
/*	o = &model[CUBE];
	o->m = load_model("box.bin");
	
	o->scale  = glGetUniformLocation(shader, "scale");
	o->rot4vf = glGetUniformLocation(shader, "rot");
	o->persp  = glGetUniformLocation(shader, "persp");
	o->trans  = glGetUniformLocation(shader, "trans");
	o->shader = shader;
*/
}

LRESULT APIENTRY mainproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 0x51:
					rotx -= 0.05;
				break;
				case 0x57:
					roty -= 0.05;
				break;
				case 0x45:
					rotz -= 0.05;
				break;
				case 0x41:
					rotx+= 0.05;
				break;
				case 0x53:
					roty+= 0.05;
				break;
				case 0x44:
					rotz+=0.05;
				break;
				case 0x46:
					nlog("%f %f %f", rotx, roty, rotz);
				break;
			}
		if(wParam != VK_ESCAPE)
				break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcx;
	MSG msg;
	HWND hwnd;
	HDC hdc;

	(void)hPrevInstance;
	(void)nCmdShow;
	(void)lpCmdLine;

	wcx.cbSize        = sizeof(WNDCLASSEX);
	wcx.style         = 0;
	wcx.lpfnWndProc   = mainproc;
	wcx.cbClsExtra    = 0;
	wcx.cbWndExtra    = 0;
	wcx.hInstance     = hInstance;
	wcx.hIcon         = NULL;
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = GetSysColorBrush(COLOR_3DLIGHT);
	wcx.lpszMenuName  = NULL;
	wcx.lpszClassName =  "gl";
	wcx.hIconSm       = NULL;

	if(!RegisterClassEx(&wcx))
		return 0;

	hwnd = CreateWindowEx(
		WS_EX_WINDOWEDGE, "gl", "gl",
		WS_CLIPCHILDREN | WS_SYSMENU,
		100, 100, 640, 480,
		NULL, NULL, hInstance, NULL
	);

	memset(&model[0], 0x42, sizeof(struct object));
	memset(&model[1], 0x69, sizeof(struct object));

	initgl(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	hdc = GetWindowDC(hwnd);

	
	while(1)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		render();

		SwapBuffers(hdc);

	}

	return msg.wParam;
}