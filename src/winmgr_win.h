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

#pragma once

#include "winmgr.h"


#ifdef PW_USE_WIN

class CWinManagerWin : public CWinManager
{
public:
	//! Default constructor
	CWinManagerWin(CEventHandler& eventHandler) : CWinManager(eventHandler), m_Wnd(NULL), m_DC(NULL), m_AspectRatio(0.0f), m_Redisplay(false)  {}
	~CWinManagerWin();

public:
	//From CWinManager
	void CreateGLWindow(const int width, const int height);
	void PostRedisplay();
	void MainLoop();
	void OnApplicationExit();
	void ShowError(const char* err);
	void PostExit();

protected:
	//From CWinManager
	void SwapBuffers() const;

private:
	/**
	 * Destroy window and post exit message
	 */
	void DestroyAndQuit();

	/**
	 * The WindowProc function (for more information see MSDN)
	 * \param wnd Handle to the window
	 * \param msg Specifies the message
	 * \param wParam Specifies additional message information
	 * \param lParam Specifies additional message information
	 * \return result of the message processing, depends on the message sent
	 */
	static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND	m_Wnd;			///< Holds Our Window Handle
	HDC		m_DC;			///< Private GDI Device Context
	float	m_AspectRatio;	///< Needed window aspect ration
	bool	m_Redisplay;	///< Redisplay flag
};

#endif	//PW_USE_WIN
