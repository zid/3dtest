#include "gl.h"
#include <GL/glx.h>
#include <stdio.h>
#include <stdarg.h>
#include "game.h"
#include "gfx.h"

/* size of our main window */
#define WIDTH 800
#define HEIGHT 600

/* stuff from Khronos GLX that we'll need */
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

static int keep_going = 1; /* used to break out of the main loop */

void nlog(char *str, ...)
{
	va_list ap;

	va_start(ap, str);
	vfprintf(stderr, str, ap);
	va_end(ap);
}

void gl_destroy(void)
{
	keep_going = 0;
}

int main(int argc, char **argv)
{
	Display *dpy = XOpenDisplay(":0.0");

	if (!dpy)
		return 1;

	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);
	const int attribs[] = {
		GLX_X_RENDERABLE  , True,
		GLX_DRAWABLE_TYPE , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE   , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE , GLX_TRUE_COLOR,
		GLX_DEPTH_SIZE    , 1,
		GLX_DOUBLEBUFFER  , True,
		None
	};
	int fbcount;
	GLXFBConfig *fbconfiglist, fbconfig;

	fbconfiglist = glXChooseFBConfig(dpy, screen, attribs, &fbcount);
	if (fbcount <= 0) {
		nlog("No valid FB configurations\n");
		return 1;
	}
	fbconfig = fbconfiglist[0];
	XFree(fbconfiglist);

	XVisualInfo *xvi = glXGetVisualFromFBConfig(dpy, fbconfig);

	XSetWindowAttributes wattr;
	wattr.event_mask = StructureNotifyMask | KeyPressMask;
	wattr.background_pixmap = None;
	wattr.background_pixel = 0;
	wattr.border_pixel = 0;
	wattr.colormap = XCreateColormap(dpy, root, xvi->visual, AllocNone);

	Window win = XCreateWindow (dpy, root, 0, 0, WIDTH, HEIGHT, 0,
			xvi->depth, InputOutput, xvi->visual,
			CWBackPixmap | CWBorderPixel |
			CWColormap | CWEventMask, &wattr);
	Atom wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
	Atom wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &wm_delete_window, True);
	XStoreName(dpy, win, "gl"); /* set window title */

	/* make a temporary context to find glXCreateContextAttribsARB() */
	GLXContext ctx_tmp = glXCreateContext(dpy, xvi, 0, True);
	if (!ctx_tmp) {
		nlog("Failed to allocate legacy GL context!\n");
		return 1;
	}
	glXMakeCurrent(dpy, win, ctx_tmp);
	PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
		(PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *) "glXCreateContextAttribsARB");
	if (!glXCreateContextAttribsARB) {
		nlog("Failed to bind glXCreateContextAttribsARB!\n");
		return 1;
	}
	glXMakeCurrent(dpy, None, 0);
	glXDestroyContext(dpy, ctx_tmp);

	static int ctx_attribs[] = {
		/* GLX_CONTEXT_MAJOR_VERSION_ARB, 4, */
		/* GLX_CONTEXT_MINOR_VERSION_ARB, 2, */
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		/* GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB, */
		/* GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT, */
		None
	};

	GLXContext ctx = glXCreateContextAttribsARB(dpy, fbconfig, 0, True,
			ctx_attribs);
	XSync(dpy, False);
	if (!ctx) {
		nlog("Failed to allocate GL context!\n");
		return 1;
	}

	glXMakeCurrent(dpy, win, ctx);
	nlog("OpenGL version %s (%s)\n",
			glGetString(GL_VERSION), glGetString(GL_RENDERER));

	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT =
		(PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
	if (!glXSwapIntervalEXT) {
		nlog("Failed to bind glXCreateContextAttribsARB!\n");
		return 1;
	}
	glXSwapIntervalEXT(dpy, win, 1);


	XMapRaised(dpy, win);

	XFlush(dpy);

	XEvent ev;
	int win_is_mapped = 0; /* window is not yet visible/mapped */

	/* initialize the app */
	gfx_init();
	game_init();

	keep_going = 1;
	while (keep_going) {
		if (win_is_mapped) {
			game();
			glFlush();
			glXSwapBuffers(dpy, win);
		}
		while (XEventsQueued(dpy, QueuedAfterFlush)) {
			XNextEvent(dpy, &ev);
			switch (ev.type) {
			case MapNotify:
				nlog("Mapped!\n");
				win_is_mapped = 1;
				break;
			case UnmapNotify: /* window was probably minimized */
				nlog("Unmapped!\n");
				win_is_mapped = 0;
				break;
			case ConfigureNotify:
				nlog("Resize %d, %d\n", ev.xconfigure.width, ev.xconfigure.height);
				break;
			case KeyPress: {
				KeySym sym = XLookupKeysym(&ev.xkey, 0);
				switch (sym) {
				case XK_Escape:
					keep_going = 0;
					break;
				}
				break;
				}
			case ClientMessage:
				if (ev.xclient.message_type == wm_protocols && (Atom)ev.xclient.data.l[0] == wm_delete_window) {
					keep_going = 0;
				}
				break;
			}
		}
	}

	gfx_kill();
	game_kill();

	return 0;
}
