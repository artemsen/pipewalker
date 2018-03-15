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

#ifdef PW_USE_WINCE

#include "winmgr_wince.h"
#include "synchro.h"
#include "PipeWalkerRes.h"


CWinManagerWinCE::CWinManagerWinCE(CEventHandler& eventHandler)
:	CWinManager(eventHandler),
	_Display(NULL), _Config(NULL), _Context(NULL), _WindowSurface(NULL), _Wnd(NULL),
	_Redisplay(false)
{
}


void CWinManagerWinCE::PostRedisplay()
{
	if (!_Redisplay) {
 		static unsigned int lastRedraw = CSynchro::GetTick();
 		const unsigned int currTick = CSynchro::GetTick();
 		if (currTick - lastRedraw < 30)
 			::Sleep(30 - (currTick - lastRedraw));
 		lastRedraw = CSynchro::GetTick();
		::RedrawWindow(m_wnd, NULL, 0, RDW_INTERNALPAINT);
		_Redisplay = true;
	}
}


void CWinManagerWinCE::MainLoop()
{
	::ShowWindow(_Wnd, SW_SHOWNORMAL);
	::UpdateWindow(_Wnd);

	//Main message loop
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}


void CWinManagerWinCE::CreateGLWindow(const int width, const int height)
{
	//Register window class
	static const wchar_t* windowClassName = L"PipeWalkerWindowClass";
	WNDCLASS wndcl;
	wndcl.style			= 0;
	wndcl.lpfnWndProc	= (WNDPROC)&CWinManagerWinCE::WndProc;
	wndcl.cbClsExtra	= 0;
	wndcl.cbWndExtra	= 0;
	wndcl.hInstance		= ::GetModuleHandle(NULL);
	wndcl.hIcon			= ::LoadIcon(wndcl.hInstance, (LPCTSTR)IDI_PIPEWALKER);
	wndcl.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wndcl.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wndcl.lpszMenuName	= NULL;
	wndcl.lpszClassName	= windowClassName;
	if (!::RegisterClass(&wndcl))
		throw CException("Can't register window class");

	//Create main window
	_Wnd = CreateWindow(windowClassName, PACKAGE_STRING, 
		WS_VISIBLE,
//		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		::GetModuleHandle(NULL),
		NULL);
		
	if (!_Wnd)
		throw CException("Can't create window");
	::SetWindowLong(_Wnd, GWL_USERDATA, reinterpret_cast<long>(this));


	/* EGL Setup */
	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;

	static const EGLint s_configAttribs[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, EGL_DONT_CARE,
		EGL_DEPTH_SIZE, 32,
		EGL_STENCIL_SIZE, EGL_DONT_CARE,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(_Display, &majorVersion, &minorVersion);
	eglGetConfigs(_Display, NULL, 0, &numConfigs);
	eglChooseConfig(_Display, s_configAttribs, &_Config, 1, &numConfigs);
	_Context = eglCreateContext(_Display, _Config, NULL, NULL);

	_WindowSurface = eglCreateWindowSurface(_Display, _Config, _Wnd, NULL);

	eglMakeCurrent(_Display, _WindowSurface, _WindowSurface, _Context);
	const int error = eglGetError();
	if (error != EGL_SUCCESS)
		throw CException("Error creating GL window");
}


void CWinManagerWinCE::SwapBuffers() const
{
	eglSwapBuffers(_Display, _WindowSurface);
}


void CWinManagerWinCE::OnApplicationExit()
{
}


void CWinManagerWinCE::ShowError(const char* err)
{
	wstring buf;
	buf.resize(strlen(err) + 1);
	MultiByteToWideChar(CP_ACP, 0, err, -1, &buf[0], buf.capacity() - 1);
	::MessageBox(m_Wnd, buf.c_str(), PACKAGE_STRING L" Error", MB_ICONERROR | MB_OK);
}


LRESULT CALLBACK CWinManagerWinCE::WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Get pointer to this class
	CWinManagerWinCE* mgr = reinterpret_cast<CWinManagerWinCE*>(::GetWindowLong(wnd, GWL_USERDATA));

	switch (msg) {
		case WM_PAINT:
			assert(mgr);
			mgr->m_Redisplay = false;
			PAINTSTRUCT ps;
			::BeginPaint(wnd, &ps);
			mgr->OnRenderScene();
			::EndPaint(wnd, &ps);
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


void CWinManagerWinCE::DestroyAndQuit()
{
	eglMakeCurrent(_Display, NULL, NULL, NULL);
	eglDestroyContext(_Display, _Context);
	eglDestroySurface(_Display, _WindowSurface);
	eglTerminate(_Display);

	if (_Wnd)
		::DestroyWindow(_Wnd);
	_Wnd = NULL;
	::PostQuitMessage(0);
}


void CWinManagerWinCE::PostExit()
{
	DestroyAndQuit();
}

#endif	//PW_USE_WINCE
