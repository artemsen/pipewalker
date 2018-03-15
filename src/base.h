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

#pragma once


/**
 * An event handler base class
 * Used by window subsystem to pass events
 */
class CEventHandler
{
public:
	//! Default constructor
	CEventHandler(void)			{}

	//! Default destructor
	virtual ~CEventHandler()	{}

	//! Mouse button types
	enum MouseButton {
		LeftButton,
		MiddleButton,
		RightButton
	};

	/**
	 * Mouse click event handler
	 * @param MouseButton a mouse button type
	 * @param nXCoord an X coordinate of mouse pointer
	 * @param nYCoord an Y coordinate of mouse pointer
	 */
	virtual void OnMouseClick(const MouseButton enuButton, const int nXCoord, const int nYCoord) = 0;

	/**
	 * Key press event handler
	 * @param nKeyCode key code
	 */
	virtual void OnKeyPress(const int nKeyCode) = 0;

	/**
	 * Handler - draw the GL window
	 */
	virtual void OnDraw(void) = 0;
};


/**
 * A window graphics subsystem
 * Used for work with windows
 */
class CWinSubsystem
{
public:
	/**
	 * Default constructor
	 * @param pEventHandler an event handler
	 */
	CWinSubsystem(CEventHandler* pEventHandler) : m_pEventHandler(pEventHandler) {}

	//! Default destructor
	virtual ~CWinSubsystem() {}

	/**
	 * Initialize subsystem
	 * @return false if error
	 */
	virtual bool Initialize(void) = 0;

	/**
	 * Enter to main loop
	 * @return exit code (0 if no error)
	 */
	virtual int DoMainLoop(void) = 0;

	/**
	 * Post exit message
	 */
	virtual void PostExit(void) = 0;

	/**
	 * Post re-display message
	 */
	virtual void PostRedisplay(void) = 0;

	/**
	 * Show error message
	 * @param pszErrorMessage an error message to show
	 */
	virtual void ShowErrorMessage(const char* pszErrorMsg) = 0;

protected:	//Class variables

	CEventHandler* m_pEventHandler;	///< an event handler
};
