#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "game.h"
#include "gfx.h"
#include "gl.h"

extern float rotx, roty, rotz;
static HGLRC gl;

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

LRESULT APIENTRY mainproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
		if(wParam != VK_ESCAPE)
				break;
		case WM_QUIT:
		case WM_DESTROY:
			gfx_kill();
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
		100, 100, 1920, 1080,
		NULL, NULL, hInstance, NULL
	);

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
	gl_init_procs();
	wglSwapIntervalEXT(1);

	gfx_init();
	game_init();
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

		game();

		SwapBuffers(hdc);
	}

	game_kill();

	return msg.wParam;
}
