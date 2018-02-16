#include <windows.h>
//#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "gl.h"

extern float rotx, roty, rotz;

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
		case WM_QUIT:
		case WM_DESTROY:
			gldestroy();
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
