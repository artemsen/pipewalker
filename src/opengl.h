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

#include "base.h"
#include "game.h"


class COpenGL : public CEventHandler
{
public:
	//! Default constructor
	COpenGL(void);
	//! Default destructor
	virtual ~COpenGL(void);

	/**
	 * Main routine (intialize OpenGL and run the application)
	 * @return result
	 */
	static int Run(void);

	//From CEventHandler
	void OnMouseClick(const MouseButton enuButton, const int nXCoord, const int nYCoord);
	void OnKeyPress(const int nKeyCode);
	void OnDraw(void);

private:	//Class variables
	//! Texture type enum
	enum Texture {
		TxrCell = 0,	///< simple cell
		TxrActiveTube,	///< active tube
		TxrPassiveTube,	///< passive tube
		TxrSender,		///< sender (server)
		TxrRcvActive,	///< active receiver (client)
		TxrRcvPassive,	///< passive receiver (client)
		TxrRcvBack,		///< receiver (client) back side
		TxrEnvironment,	///< environment texture
		TxrButtonNew,	///< "new" button
		TxrButtonRst,	///< "reset" button
		TxrWebLink,		///< web link texture
		TxrWinTitle,	///< win title
		TxrTitle,		///< title
		TxrCounter		///< texture type counter (for static memory allocation)
	};
	int				m_aTextures[TxrCounter];	///< Textures used by objects
	CGame			m_objGame;					///< Game logic class
	CWinSubsystem*	m_pWinSubsystem;			///< Window manager

private:
	/**
	 * Set windows manager
	 * @param pWinSubsystem pointer to window manager class
	 */
	void SetWndSubsystem(CWinSubsystem* pWinSubsystem);

	/**
	 * Start new game
	 */
	void NewGame(void)		{ m_objGame.New(); m_pWinSubsystem->PostRedisplay(); }

	/**
	 * Reset current game
	 */
	void ResetGame(void)	{ m_objGame.Reset(); m_pWinSubsystem->PostRedisplay(); }

private:	//Drawing functions
	/**
	 * Draw environment (title, buttons, etc)
	 */
	void RenderEnv(void);

	/**
	 * Draw background cells
	 */
	void RenderBackground(void);

	/**
	 * Draw scene (tubes, sender, recievers)
	 * @param fIsShadow true if it is shadow projection drawing
	 */
	void RenderScene(bool fIsShadow);

	/**
	 * Draw tube (ObjStrTube, ObjHalfTube, ObjCurTube, ObjTubeJoint)
	 * @param pCell an object
	 * @return true if redisplay is needed
	 */
	bool DrawTube(CCell* pCell, bool fIsShadow);

	/**
	 * Draw sender (TxrSender)
	 * @param pCell an object
	 */
	void DrawSender(CCell* pCell);

	/**
	 * Draw receiver (ObjReceiver)
	 * @param pCell an object
	 */
	void DrawReciever(CCell* pCell);
};
