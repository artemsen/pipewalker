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
#include "globjects.h"


class COpenGL : public CEventHandler
{
public:
	//! Default constructor
	COpenGL(void);

	//! Default destructor
	virtual ~COpenGL();

	/**
	 * Main routine
	 * @return result
	 */
	static int Run(void);

public:
	//From CEventHandler
	void OnMouseClick(const MouseButton enuButton, const int nXCoord, const int nYCoord);
	void OnKeyPress(const int nKeyCode);
	void OnDraw(void);
	void OnWndSizeChanged(const int nWidth, const int nHeight);

private:	//Class variables

	//! Game modes
	enum Mode {
		ModePlayGame,
		ModeNewGame,
		ModeCustomGame,
		ModeInfo
	} m_enuCurrentMode, m_enuNewMode;	///< Game modes
	bool			m_fMotionFirstPhase;///< First motion phase (first 90 degree)
	unsigned long	m_nMotionStartTime;	///< Motion start time

	CGame*			m_pGameLogic;		///< Game logic class
	CWinSubsystem*	m_pWinSubsystem;	///< Window manager
	CGLObjects*		m_pObjects;			///< OpenGL objects drawer

private:

	/**
	 * Set new game mode
	 * @param enuNewMode new game mode
	 */
	void SetNewMode(const Mode enuNewMode);

	/**
	 * Reset current game
	 */
	void ResetGame(void)						{ m_pGameLogic->Reset(); m_pWinSubsystem->PostRedisplay(); }

	/**
	 * Initialization
	 * @param pWinSubsystem pointer to window manager class
	 * @return false if error
	 */
	bool Initialize(CWinSubsystem* pWinSubsystem);
	
	/**
	 * Draw environment (title, buttons etc)
	 */
	void DrawEnvironment(void);

	/**
	 * Draw setup game window
	 * @param nGLMode render mode
	 */
	void DrawSetUpGame(int nGLMode);

	/**
	 * Draw scene
	 * @param nGLMode render mode
	 * @return true if redisplay is needed
	 */
	bool DrawGame(int nGLMode);

	/**
	 * Draw map cells
	 */
	void DrawMapCells(void);

	/**
	 * Draw scene (tubes, sender, recievers)
	 * @param fIsShadow true if it is shadow projection drawing
	 * @return true if redisplay is needed
	 */
	bool DrawMapObjects(bool fIsShadow);

	/**
	 * Draw tube (ObjStrTube, ObjHalfTube, ObjCurTube, ObjTubeJoint)
	 * @param pCell an object
	 * @return true if redisplay is needed
	 */
	bool DrawTube(CCell* pCell, bool fIsShadow);

	/**
	 * Set up projection matrix
	 */
	void SetupProjection(void) const;

private:	//Implementation functions from GLu
	
	//! gluLookAt implementation
	void LookAt(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat lookAtX, GLfloat lookAtY, GLfloat lookAtZ, GLfloat upX, GLfloat upY, GLfloat upZ) const;
	
	//! gluPickMatrix implementation
	void PickMatrix(GLfloat x, GLfloat y, GLfloat width, GLfloat height, const GLint viewport[4]) const;
	
	//! Perform cross product between 2 vectors
	void CrossProd(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat res[3]) const;


};
