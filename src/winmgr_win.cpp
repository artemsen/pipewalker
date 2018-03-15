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

#ifdef PW_USE_WIN

#include "winmgr_win.h"
#include "synchro.h"
#include "PipeWalkerRes.h"


CWinManagerWin::~CWinManagerWin()
{
	if (m_DC)
		::ReleaseDC(m_Wnd, m_DC);
	if (m_Wnd)
		::DestroyWindow(m_Wnd);
}


void CWinManagerWin::PostRedisplay()
{
	if (!m_Redisplay) {
 		static unsigned int lastRedraw = CSynchro::GetTick();
 		const unsigned int currTick = CSynchro::GetTick();
 		if (currTick - lastRedraw < 30)
 			::Sleep(30 - (currTick - lastRedraw));
 		lastRedraw = CSynchro::GetTick();
		::RedrawWindow(m_Wnd, NULL, 0, RDW_INTERNALPAINT);
		m_Redisplay = true;
	}
}


void CWinManagerWin::MainLoop()
{
	::ShowWindow(m_Wnd, SW_SHOWNORMAL);
	::UpdateWindow(m_Wnd);

	//Main message loop
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}


void CWinManagerWin::CreateGLWindow(const int width, const int height)
{
	//Register window class
	static const char* windowClassName = "PipeWalkerWindowClass";
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(wcex);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)&CWinManagerWin::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= ::GetModuleHandle(NULL);
	wcex.hIcon			= ::LoadIcon(wcex.hInstance, (LPCTSTR)IDI_PIPEWALKER);
	wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= windowClassName;
	wcex.hIconSm		= ::LoadIcon(wcex.hInstance, (LPCTSTR)IDI_PIPEWALKER);
	if (!::RegisterClassEx(&wcex))
		throw CException("Can't register window class");

	//Window styles
	const DWORD wndStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	const DWORD wndStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	//Adjust window to true requested size
	RECT rcWnd;
	rcWnd.top = rcWnd.left = 0;
	rcWnd.right = width;
	rcWnd.bottom = height;
	::AdjustWindowRectEx(&rcWnd, wndStyle, FALSE, wndStyleEx);
	m_AspectRatio = static_cast<float>(rcWnd.right - rcWnd.left) / static_cast<float>(rcWnd.bottom - rcWnd.top);

	//Create main window
	m_Wnd = CreateWindowEx(wndStyleEx, windowClassName, PACKAGE_STRING, wndStyle, CW_USEDEFAULT, 0,
		rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, NULL, NULL, wcex.hInstance, NULL);
	if (!m_Wnd)
		throw CException("Can't create window");
	::SetWindowLong(m_Wnd, GWL_USERDATA, reinterpret_cast<long>(this));

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	m_DC = ::GetDC(m_Wnd);
	if (!m_DC)
		throw CException("Can't create a GL device context");

	const int pixelFormat = ::ChoosePixelFormat(m_DC, &pfd);
	if (!pixelFormat)
		throw CException("Can't find a suitable pixel format");

	if (!::SetPixelFormat(m_DC, pixelFormat, &pfd))
		throw CException("Can't set the pixel format");

	HGLRC rc = ::wglCreateContext(m_DC);
	if (!rc)
		throw CException("Can't create a GL rendering context");

	if (!::wglMakeCurrent(m_DC, rc))
		throw CException("Can't activate the GL rendering context");
}


void CWinManagerWin::SwapBuffers() const
{
	::SwapBuffers(m_DC);
}


void CWinManagerWin::OnApplicationExit()
{
}


void CWinManagerWin::ShowError(const char* err)
{
	::MessageBoxA(m_Wnd, err, PACKAGE_STRING " Error", MB_ICONERROR | MB_OK);
}


LRESULT CALLBACK CWinManagerWin::WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Get pointer to this class
	CWinManagerWin* mgr = reinterpret_cast<CWinManagerWin*>(::GetWindowLong(wnd, GWL_USERDATA));

	switch (msg) {
		case WM_PAINT:
			assert(mgr);
			mgr->m_Redisplay = false;
			PAINTSTRUCT ps;
			::BeginPaint(wnd, &ps);
			mgr->OnRenderScene();
			::EndPaint(wnd, &ps);
			break;
 		case WM_GETMINMAXINFO:
 			{
				LPMINMAXINFO mmi = reinterpret_cast<LPMINMAXINFO>(lParam);
				
				const int maxSize = GetSystemMetrics(SM_CYMAXIMIZED);
				static const float aspect = static_cast<float>(PW_SCREEN_WIDTH) / static_cast<float>(PW_SCREEN_HEIGHT);

				mmi->ptMaxSize.x = static_cast<LONG>(static_cast<float>(maxSize) * aspect);
				mmi->ptMaxSize.y = maxSize;
 				mmi->ptMinTrackSize.x = PW_SCREEN_WIDTH / 3;
 				mmi->ptMinTrackSize.y = PW_SCREEN_HEIGHT / 3;
 			}
 			break;
 		case WM_SIZING:
			assert(mgr);
 			{
 				//Preserve correct aspect ratio
 				LPRECT rc = reinterpret_cast<LPRECT>(lParam);
 				const float currAspect = static_cast<float>(rc->right - rc->left) / static_cast<float>(rc->bottom - rc->top);
 				if (static_cast<int>(mgr->m_AspectRatio * 1000.0f) != static_cast<int>(currAspect * 1000.0f)) {
 					switch (wParam) {
 						case WMSZ_BOTTOM:
 						case WMSZ_BOTTOMRIGHT:
 						case WMSZ_TOP:
 						case WMSZ_TOPRIGHT:
 							rc->right = rc->left + static_cast<long>(static_cast<float>(rc->bottom - rc->top) * mgr->m_AspectRatio);
 							break;
 						case WMSZ_TOPLEFT:
 						case WMSZ_BOTTOMLEFT:
 							rc->left = rc->right - static_cast<long>(static_cast<float>(rc->bottom - rc->top) * mgr->m_AspectRatio);
 							break;
 						case WMSZ_RIGHT:
 						case WMSZ_LEFT:
 							rc->bottom = rc->top + static_cast<long>(static_cast<float>(rc->right - rc->left) / mgr->m_AspectRatio);
 							break;
						default:
							break;
 					}
 				}
 			}
 			break;
		case WM_SIZE:
			assert(mgr);
			mgr->InitializeOpenGL(LOWORD(lParam), HIWORD(lParam));
			mgr->OnRenderScene();
			break;
		case WM_CHAR:
			assert(mgr);
			mgr->OnKeyboardKeyDown(static_cast<char>(wParam));
			break;
		case WM_DESTROY:
			assert(mgr);
			mgr->DestroyAndQuit();
			break;
		case WM_MOUSEMOVE:
			assert(mgr);
			mgr->OnMouseMove(((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)));
			break;
		case WM_LBUTTONDOWN:	//Mouse buttons click
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			{
				assert(mgr);
				MouseButton btn = MouseButton_None;
				if (msg == WM_LBUTTONDOWN)
					btn = MouseButton_Left;
				else if (msg == WM_RBUTTONDOWN)
					btn = MouseButton_Right;
				else if (msg == WM_MBUTTONDOWN)
					btn = MouseButton_Middle;
				mgr->OnMouseButtonDown(((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)), btn);
			}
			break;
		default:
			return ::DefWindowProc(wnd, msg, wParam, lParam);
	}
	return 0;
}


void CWinManagerWin::DestroyAndQuit()
{
	::PostQuitMessage(0);
}


void CWinManagerWin::PostExit()
{
	DestroyAndQuit();
}

#endif	//PW_USE_WIN
