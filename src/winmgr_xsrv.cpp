/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#ifdef PW_USE_XSRV

#include "winmgr_xsrv.h"
#include "../extra/pipewalker.xpm"


CWinManagerXSrv::CWinManagerXSrv(CEventHandler& eventHandler)
:	CWinManager(eventHandler),
	_Disp(NULL), _Ctx(NULL),
	_Done(false)
{
}


void CWinManagerXSrv::CreateGLWindow(const int width, const int height)
{
	//Open display
	_Disp = XOpenDisplay(0);
	if (!_Disp)
		throw CException("Can't open display");

	int screen = DefaultScreen(_Disp);

	//Check extension and version
	int errorBase = 0, eventBase = 0;
	if (!glXQueryExtension(_Disp, &errorBase, &eventBase))
		throw CException("GLX extension unsupported");
	int verMajor = 0, verMinor = 0;
	if (!glXQueryVersion(_Disp, &verMajor, &verMinor))
		throw CException("Unable to query GLX version");
	printf("Using GLX version: %i.%i\n", verMajor, verMinor);

	if (false) {//Unable to verify :-( ////verMajor > 1 || (verMajor == 1 && verMinor > 2)) {
		//1.3 or greater
		int fbElements = 0;
		GLXFBConfig* glxConf = glXGetFBConfigs(_Disp, screen, &fbElements);
		if (!glxConf)
			throw CException("Unable to get frame buffer config");
		int checkAttrValue = 0;
		if (glXGetFBConfigAttrib(_Disp, *glxConf, GLX_DOUBLEBUFFER, &checkAttrValue) != Success)
			throw CException("Unable to get GLX_DOUBLEBUFFER attribute");
		if (checkAttrValue != True)
			throw CException("Double buffered visual not supported");
		//Create GLX context
		_Ctx = glXCreateNewContext(_Disp, *glxConf, GLX_RGBA_TYPE, NULL, True);
		if (!_Ctx)
			throw CException("Unable to create new context");
	}
	else {
		//1.2
		//Attributes for a double buffered visual in RGBA format with at least 4 bits per color and a 16 bit depth buffer
		static int dbAttrList[] = {
			GLX_RGBA, GLX_DOUBLEBUFFER,
			GLX_RED_SIZE, 4,
			GLX_GREEN_SIZE, 4,
			GLX_BLUE_SIZE, 4,
			GLX_DEPTH_SIZE, 16,
			None };
	
		//Get an appropriate visual
		XVisualInfo* vi = glXChooseVisual(_Disp, screen, dbAttrList);
		if (!vi)
			throw CException("Double buffered visual not supported");

		//Create a GLX context
		_Ctx = glXCreateContext(_Disp, vi, 0, GL_TRUE);
	}

	//Create new window
	_Wnd = XCreateWindow(_Disp, RootWindow(_Disp, screen),
				0, 0, PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT, 0, CopyFromParent, InputOutput, CopyFromParent, 0, 0);
	if (!_Wnd)
		throw CException("Can't create window");

	//Register wm_delete_events handle
	Atom wmDelete = XInternAtom(_Disp, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(_Disp, _Wnd, &wmDelete, 1);

	//Set window properties
	XSizeHints xSize;	//Set min window size and aspect ratio
	xSize.flags = PMinSize | PMaxSize | PAspect;
	xSize.min_width = width / 2;
	xSize.min_height = height / 2;
	xSize.max_width = width * 10;
	xSize.max_height = height * 10;
	xSize.max_aspect.x = xSize.min_aspect.x = width;
	xSize.max_aspect.y = xSize.min_aspect.y = height;
	Pixmap xIcon;
	XpmCreatePixmapFromData(_Disp, _Wnd, const_cast<char**>(pipewalker_xpm), &xIcon, NULL, NULL);
	XSetStandardProperties(_Disp, _Wnd, PACKAGE_STRING, PACKAGE_STRING, xIcon, NULL, 0, &xSize);

	//Register handled events
	XSelectInput(_Disp, _Wnd, ExposureMask | KeyPressMask | ButtonPressMask | PointerMotionMask | StructureNotifyMask);

	//Connect the glx-context to the window
	glXMakeCurrent(_Disp, _Wnd, _Ctx);

	//Just warning
	if (!glXIsDirect(_Disp, _Ctx))
		printf("Warning: No Direct Rendering possible!\n");

	//Draw window
	XMapRaised(_Disp, _Wnd);
}


void CWinManagerXSrv::PostRedisplay()
{
	XEvent xExpEvent;
	if (!XCheckTypedWindowEvent(_Disp, _Wnd, Expose, &xExpEvent)) {
		xExpEvent.type = Expose;
		xExpEvent.xexpose.type = Expose;
		xExpEvent.xexpose.display = _Disp;
		xExpEvent.xexpose.window = _Wnd;
		xExpEvent.xexpose.send_event = True;
		xExpEvent.xexpose.x = 0;
		xExpEvent.xexpose.y = 0;
		xExpEvent.xexpose.width = 0;
		xExpEvent.xexpose.height = 0;
		xExpEvent.xexpose.count = 0;
		xExpEvent.xexpose.serial = 1;
	}
	XSendEvent(_Disp, _Wnd, True, ExposureMask, &xExpEvent);
}


void CWinManagerXSrv::MainLoop()
{
	int wndWidth = PW_SCREEN_WIDTH;
	int wndHeight = PW_SCREEN_HEIGHT;

	while (!_Done) {
		XEvent xEvent;
		XNextEvent(_Disp, &xEvent);
		switch (xEvent.type) {
			case Expose:			//Draw window
				if (xEvent.xexpose.count == 0) {
					OnRenderScene();
				}
				break;
			case ConfigureNotify:	//Resize window
				if (wndWidth != xEvent.xconfigure.width || wndHeight != xEvent.xconfigure.height) {
					wndWidth = xEvent.xconfigure.width;
					wndHeight = xEvent.xconfigure.height;
					InitializeOpenGL(wndWidth, wndHeight);
					OnRenderScene();
				}
				break;
			case KeyPress:			//Key press event
				{
					KeySym ks = XLookupKeysym(&xEvent.xkey, 0);
					if (ks == XK_Escape)
						ks = 27; //ASCII Esc
					if (ks < 256)
						_EventHandler.OnKeyboardKeyDown(static_cast<char>(ks));
				}
				break;
			case ButtonPress:		//Mouse button press event
				if (xEvent.xbutton.button >= 1 && xEvent.xbutton.button <= 3) {
					OnMouseButtonDown(xEvent.xbutton.x, xEvent.xbutton.y,
						xEvent.xbutton.button == 1 ? MouseButton_Left :
						xEvent.xbutton.button == 2 ? MouseButton_Middle : MouseButton_Right);
				}
				break;
			case MotionNotify:		//Mouse motion
				OnMouseMove(xEvent.xmotion.x, xEvent.xmotion.y);
				break;
			case ClientMessage:		//Main window closed
				if (*XGetAtomName(_Disp, xEvent.xclient.message_type) == *"WM_PROTOCOLS")
					_Done = true;
				break;
			}
	}

	//Destroy context
	if (_Ctx) {
		if (!glXMakeCurrent(_Disp, None, NULL))
			printf("Could not release drawing context!\n");
		glXDestroyContext(_Disp, _Ctx);
		_Ctx = NULL;
	}
	XCloseDisplay(_Disp);
}


void CWinManagerXSrv::ShowError(const char* err)
{
	fputs("Critical error: ", stderr);
	fputs(err, stderr);
	fputs("\n", stderr);
}


void CWinManagerXSrv::PostExit(void)
{
	_Done = true;
}


void CWinManagerXSrv::SwapBuffers() const
{
	glXSwapBuffers(_Disp, _Wnd);
}

#endif	//PW_USE_XSRV
