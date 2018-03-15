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

#if defined PW_SYSTEM_WINNT && !defined PW_USE_GLUT

#pragma once

#include "base.h"
#include "common.h"
#include "PipeWalkerRes.h"


class CMSWindows : public CWinSubsystem
{
public:
	//! Default constructor
	CMSWindows(CEventHandler* pEventHandler) : CWinSubsystem(pEventHandler), m_hWnd(NULL), m_hDC(NULL)	{}

	//! Default destructor
	~CMSWindows()	{}

public:	//From CWinSubsystem
	bool Initialize(void);
	int DoMainLoop(void);
	void PostExit(void);
	void PostRedisplay(void);
	void ShowErrorMessage(const char* pszErrorMsg);

private:
	HWND	m_hWnd;		///< Holds Our Window Handle
	HDC		m_hDC;		///< Private GDI Device Context

	/**
	 * Destroy window and post exit message
	 */
	void DestroyAndQuit(void);

	/**
	 * The WindowProc function (for more information see MSDN)
	 * @param hWnd Handle to the window
	 * @param uMsg Specifies the message
	 * @param wParam Specifies additional message information
	 * @param lParam Specifies additional message information
	 * @return result of the message processing, depends on the message sent
	 */
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif	//PW_SYSTEM_WINNT
