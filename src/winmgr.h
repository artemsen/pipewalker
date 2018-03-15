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

#include "common.h"


//! Mouse buttons identifiers
enum MouseButton {
	MouseButton_None =      0x00,
	MouseButton_Left =      0x01,
	MouseButton_Right =     0x02,
	MouseButton_Middle =    0x04,
	MouseButton_WheelUp =   0x08,
	MouseButton_WheelDown = 0x10
};


class CEventHandler
{
public:
	/**
	 * Called inside main loop to render scene
	 * \param mouseX an X mouse world coordinate
	 * \param mouseY an Y mouse world coordinate
	 */
	virtual void RenderScene(const float mouseX, const float mouseY) = 0;

	/**
	 * Keyboard key down handler
	 * \param key key identifier
	 */
	virtual void OnKeyboardKeyDown(const char key) = 0;

	/**
	 * Mouse button down handler
	 * \param mouseX an X mouse world coordinate
	 * \param mouseY an Y mouse world coordinate
	 * \param btn mouse button identifier
	 */
	virtual void OnMouseButtonDown(const float mouseX, const float mouseY, const MouseButton btn) = 0;

	/**
	 * Mouse move handler
	 * \param mouseX an X mouse world coordinate
	 * \param mouseY an Y mouse world coordinate
	 */
	virtual void OnMouseMove(const float mouseX, const float mouseY) = 0;
};


#ifdef _MSC_VER
#pragma warning(disable: 4512)	//assignment operator could not be generated
#endif // WIN32

class CWinManager
{
public:
	/**
	 * Constructor
	 * \param eventHandler event handler instance
	 */
	CWinManager(CEventHandler& eventHandler);
	virtual ~CWinManager() {}


public:	//Platform independent service functions
	/**
	 * Initialize OpenGL subsystem
	 * \param width window width
	 * \param height window height
	 */
	void InitializeOpenGL(const int width, const int height) const;


public:	//Platform dependent service functions
	/**
	 * Create main OpenGL window
	 * \param width window width
	 * \param height window height
	 */
	virtual void CreateGLWindow(const int width, const int height) = 0;

	/**
	 * Show error
	 * \param text error text
	 */
	virtual void ShowError(const char* err) = 0;

	/**
	 * Post exit message
	 */
	virtual void PostExit() = 0;

	/**
	 * Post redisplay message
	 */
	virtual void PostRedisplay() = 0;

	/**
	 * Main routine loop
	 */
	virtual void MainLoop() = 0;

	/**
	 * On exit handler
	 */
	virtual void OnApplicationExit() = 0;


protected:	//Platform dependent window manager functions
	/**
	 * Swap OpenGL buffers
	 */
	virtual void SwapBuffers() const = 0;


protected:	//Event handlers
	/**
	 * Called inside main loop to render scene
	 */
	void OnRenderScene();

	/**
	 * Keyboard key down handler
	 * \param key key identifier
	 */
	void OnKeyboardKeyDown(const char key);

	/**
	 * Mouse button down handler
	 * \param x new mouse X window coordinate
	 * \param y new mouse Y window coordinate
	 * \param btn mouse button identifier
	 */
	void OnMouseButtonDown(const int x, const int y, const MouseButton btn);

	/**
	 * Mouse move handler
	 * \param x new mouse X window coordinate
	 * \param y new mouse Y window coordinate
	 */
	void OnMouseMove(const int x, const int y);


private:
	/**
	 * Update mouse position
	 * \param x new mouse X window coordinate
	 * \param y new mouse Y window coordinate
	 */
	void UpdateMousePosition(const int x, const int y);


protected:	//Class variables
	CEventHandler&	m_EventHandler;		///< Event handler instance
	int				m_MouseWndX;		///< Last mouse X window coordinate
	int				m_MouseWndY;		///< Last mouse Y window coordinate
	float			m_MouseWrldX;		///< Last mouse X world coordinate
	float			m_MouseWrldY;		///< Last mouse Y world coordinate
};
