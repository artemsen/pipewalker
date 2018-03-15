/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
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

#ifdef PW_SYSTEM_NIX

#include "wss_x11.h"
#include "PipeWalker.xpm"	//Window icon


bool CXServer::Initialize(void)
{
	try {
		//Open display
		m_xDisp = XOpenDisplay(0);
		if (!m_xDisp)
			throw("Can't open display");

		int nScreen = DefaultScreen(m_xDisp);

		//Attributes for a double buffered visual in RGBA format with at least 4 bits per color and a 16 bit depth buffer
		static int pnAttrList[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
									GLX_RED_SIZE, 4,
                                    GLX_GREEN_SIZE, 4,
                                    GLX_BLUE_SIZE, 4,
                                    GLX_DEPTH_SIZE, 16,
                                    None };

		//Get an appropriate visual
		XVisualInfo* pVI = glXChooseVisual(m_xDisp, nScreen, pnAttrList);
		if (!pVI)
			throw("Double buffered visual not supported");

		//Create a GLX context
		m_xCtx = glXCreateContext(m_xDisp, pVI, 0, GL_TRUE);

		//Create new window
		m_xWnd = XCreateWindow(m_xDisp, RootWindow(m_xDisp, pVI->screen),
							0, 0, PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT, 0, pVI->depth, InputOutput, pVI->visual,
							/*CWBorderPixel | CWColormap | CWEventMask*/ 0, 0);
		if (!m_xWnd)
			throw("Can't create window");

		//Register wm_delete_events handle
        Atom wmDelete = XInternAtom(m_xDisp, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(m_xDisp, m_xWnd, &wmDelete, 1);

		//Set window properties
		XSizeHints xSize;	//Set min/max window size (to avoid window size changes)
		xSize.flags = PMinSize | PMaxSize;
		xSize.min_width = xSize.max_width = PW_SCREEN_WIDTH;
		xSize.min_height = xSize.max_height = PW_SCREEN_HEIGHT;
		Pixmap xIcon;
		XpmCreatePixmapFromData(m_xDisp, m_xWnd, const_cast<char**>(PipeWalker_xpm), &xIcon, NULL, NULL);
		XSetStandardProperties(m_xDisp, m_xWnd, PW_WINDOW_TITLE, PW_WINDOW_TITLE, xIcon, NULL, 0, &xSize);

		//Register handled events
		XSelectInput(m_xDisp, m_xWnd, ExposureMask | KeyPressMask | ButtonPressMask);

		//Connect the glx-context to the window
		glXMakeCurrent(m_xDisp, m_xWnd, m_xCtx);

		if (!glXIsDirect(m_xDisp, m_xCtx))
			printf("Warning: No Direct Rendering possible!\n");

		//Draw window
        XMapRaised(m_xDisp, m_xWnd);

		m_fDone = false;
	}
	catch(const char* pszErr) {
		printf("Unable to initialize: %s\n", pszErr);
		return false;
	}
	return true;
}


int CXServer::DoMainLoop(void)
{
	while(!m_fDone) {
		XEvent xEvent;
		XNextEvent(m_xDisp, &xEvent);
		switch (xEvent.type) {
			case Expose:
				//Draw window
				if (xEvent.xexpose.count == 0) {
					m_pEventHandler->OnDraw();
					glXSwapBuffers(m_xDisp, m_xWnd);
				}
				break;
			case KeyPress:
				//Key press event
				{
					KeySym ks = XLookupKeysym(&xEvent.xkey, 0);
					if (ks == XK_Escape)
						ks = 27; //ASCII Esc
					if (ks < 256)
						m_pEventHandler->OnKeyPress(ks);
				}
				break;
			case ButtonPress:
				//Mouse button press event
				if (xEvent.xbutton.button >= 1 && xEvent.xbutton.button <= 3) {
					m_pEventHandler->OnMouseClick(
						xEvent.xbutton.button == 1 ? CEventHandler::LeftButton :
						xEvent.xbutton.button == 2 ? CEventHandler::MiddleButton : CEventHandler::RightButton,
						xEvent.xbutton.x, xEvent.xbutton.y);
				}
				break;
			case ClientMessage:
				//Main window closed
				if (*XGetAtomName(m_xDisp, xEvent.xclient.message_type) == *"WM_PROTOCOLS")
					m_fDone = true;
				break;
		}
	}

	//Destroy context
	if (m_xCtx) {
        if (!glXMakeCurrent(m_xDisp, None, NULL))
            printf("Could not release drawing context!\n");
        glXDestroyContext(m_xDisp, m_xCtx);
        m_xCtx = NULL;
	}
	XCloseDisplay(m_xDisp);

	return 0;
}


void CXServer::PostExit(void)
{
	m_fDone = true;
}


void CXServer::PostRedisplay(void)
{
	XEvent xExpEvent;
	if (!XCheckTypedWindowEvent(m_xDisp, m_xWnd, Expose, &xExpEvent)) {
		xExpEvent.type = Expose;
		xExpEvent.xexpose.type = Expose;
		xExpEvent.xexpose.display = m_xDisp;
		xExpEvent.xexpose.window = m_xWnd;
		xExpEvent.xexpose.send_event = True;
		xExpEvent.xexpose.x = 0;
		xExpEvent.xexpose.y = 0;
		xExpEvent.xexpose.width = PW_SCREEN_WIDTH;
		xExpEvent.xexpose.height = PW_SCREEN_HEIGHT;
		xExpEvent.xexpose.count = 0;
		xExpEvent.xexpose.serial = 1;
	}
	XSendEvent(m_xDisp, m_xWnd, True, ExposureMask, &xExpEvent);
}

void CXServer::ShowErrorMessage(const char* pszErrorMsg)
{
	printf(pszErrorMsg);
	printf("\n");
}

#endif	//PW_SYSTEM_NIX
