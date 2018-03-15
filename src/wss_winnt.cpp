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

#ifdef PW_SYSTEM_WINNT	//Only for MS Windows

#include "wss_winnt.h"


bool CMSWindows::Initialize(void)
{
	try {
		//Register window class
		const char* pszWindowClassName = "PipeWalkerWindowClass";
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)&CMSWindows::WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= GetModuleHandle(NULL);
		wcex.hIcon			= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_PIPEWALKER);
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= pszWindowClassName;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_PIPEWALKER);
		if (!RegisterClassEx(&wcex))
			throw("Can't register window class");

		//Window styles
		const DWORD dwWndStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		const DWORD dwWndStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

		//Adjust window to true requested size
		RECT rcWnd;
		rcWnd.top = rcWnd.left = 0;
		rcWnd.right = PW_SCREEN_WIDTH;
		rcWnd.bottom = PW_SCREEN_HEIGHT;
		AdjustWindowRectEx(&rcWnd, dwWndStyle, FALSE, dwWndStyleEx);

		//Create main window
		m_hWnd = CreateWindowEx(dwWndStyleEx,
								pszWindowClassName,
								PW_WINDOW_TITLE,
								dwWndStyle,
								CW_USEDEFAULT, 0,
								rcWnd.right - rcWnd.left,
								rcWnd.bottom - rcWnd.top,
								NULL, NULL,
								wcex.hInstance,
								NULL);
		if (!m_hWnd)
			throw("Can't create window");
		SetWindowLong(m_hWnd, GWL_USERDATA, reinterpret_cast<long>(this));

		static PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			16,											// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		//Get device context
		m_hDC = GetDC(m_hWnd);
		if (!m_hDC)
			throw("Can't create a GL device context");

		int nPixelFormat = ChoosePixelFormat(m_hDC,&pfd);
		if (!nPixelFormat)
			throw("Can't find a suitable pixel format");

		if (!SetPixelFormat(m_hDC, nPixelFormat, &pfd))
			throw("Can't set the pixel format");

		HGLRC hRC = wglCreateContext(m_hDC);
		if (!hRC)
			throw("Can't create a GL rendering context");

		if (!wglMakeCurrent(m_hDC, hRC))
			throw("Can't activate the GL rendering context");
	}
	catch(const char* pszErr) {
		char pErrMsg[1024];
		lstrcpy(pErrMsg, "Unable to initialize:\n");
		lstrcat(pErrMsg, pszErr);
		MessageBox(NULL, pErrMsg, NULL, MB_ICONERROR | MB_OK);
		return false;
	}
	return true;
}


int CMSWindows::DoMainLoop(void)
{
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	//Main message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}


LRESULT CALLBACK CMSWindows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Get pointer to this class
	CMSWindows* pThis = reinterpret_cast<CMSWindows*>(GetWindowLong(hWnd, GWL_USERDATA));

	switch (uMsg) {
		case WM_PAINT:
			assert(pThis);
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			pThis->m_pEventHandler->OnDraw();
			SwapBuffers(pThis->m_hDC);
			EndPaint(hWnd, &ps);
			break;
		case WM_CHAR:
			assert(pThis);
			pThis->m_pEventHandler->OnKeyPress(wParam);
			break;
		case WM_DESTROY:
			pThis->DestroyAndQuit();
			break;
	    case WM_LBUTTONDOWN:	//Mouse buttons click
		case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:
			assert(pThis);
			//Mouse button press event
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_RBUTTONDOWN) {
				pThis->m_pEventHandler->OnMouseClick(
					uMsg == WM_LBUTTONDOWN ? CEventHandler::LeftButton :
					uMsg == WM_MBUTTONDOWN ? CEventHandler::MiddleButton : CEventHandler::RightButton,
					LOWORD(lParam), HIWORD(lParam));
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
   }
   return 0;	//Compiler stub
}


void CMSWindows::PostExit(void)
{
	DestroyAndQuit();
}


void CMSWindows::PostRedisplay(void)
{
	RedrawWindow(m_hWnd, 0, 0, RDW_INTERNALPAINT);
}


void CMSWindows::DestroyAndQuit()
{
	if (m_hDC)
		ReleaseDC(m_hWnd, m_hDC);
	if (m_hWnd)
		DestroyWindow(m_hWnd);
	PostQuitMessage(0);
}


void CMSWindows::ShowErrorMessage(const char* pszErrorMsg)
{
	MessageBox(m_hWnd, pszErrorMsg, "Error", MB_ICONERROR | MB_OK);
}

#endif	//PW_SYSTEM_WINNT
