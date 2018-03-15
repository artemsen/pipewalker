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

#include "common.h"


class CGLObjects
{
public:
	//! Default constructor
	CGLObjects(void);

	//! Default destructor
	~CGLObjects();

	//! Texture type enum
	enum Texture {
		TxrCell = 0,	///< simple cell
		TxrActiveTube,	///< active tube
		TxrPassiveTube,	///< passive tube
		TxrSender,		///< sender (server)
		TxrRcvActive,	///< active receiver (client)
		TxrRcvPassive,	///< passive receiver (client)
		TxrRcvBack,		///< receiver (client) back side
		TxrLock,		///< lock
		TxrButtonNew,	///< button 'new'
		TxrButtonCust,	///< button 'custom'
		TxrButtonRset,	///< button 'reset'
		TxrButtonOk,	///< button 'Ok'
		TxrButtonCncl,	///< button 'cancel'
		TxrButtonRad,	///< radion button
		TxrWndInfo,		///< info window
		TxrWndCustom,	///< custom game window
		TxrEnvironment,	///< environment texture
		TxrHexNum,		///< hex numbers
		TxrTitle,		///< title
		TxrCounter		///< texture type counter (for static memory allocation)
	};

	//! Object display list type enum
	enum Object {
		ObjCell = 0,	///< simple cell
		ObjTube,		///< tube
		ObjTubeJoiner,	///< tube joiner
		ObjSender,		///< sender (server)
		ObjReceiverSb,	///< receiver (system block)
		ObjReceiverMon,	///< receiver (monitor)
		ObjSpin,		///< spin
		ObjLock,		///< lock
		ObjMapId,		///< text "Map ID:"
		ObjButtonRadio,	///< radio button
		ObjButton,		///< button (new/reset/custom)
		ObjHexNum,		///< hex numbers
		ObjEnvironment,	///< environment
		ObjCounter		///< object type counter (for static memory allocation)
	};

	/**
	 * Initialize
	 * @return false if error
	 */
	bool Initialize(void);

	/**
	 * Get latest error message
	 * @return latest error message
	 */
	const char* GetLastError(void) const {return m_pszLastErrorMessage; }

	/**
	 * Bind texture
	 * @param enuType texture type
	 */
	inline void BindTexture(const Texture enuType) const	{ glBindTexture(GL_TEXTURE_2D, m_aTextures[enuType]); }

	/**
	 * Draw object (display list)
	 * @param enuType object (display list) type
	 */
	inline void DrawObject(const Object enuType) const		{ glCallList(m_aDispList[enuType]); }

	/**
	 * Draw radio button
	 * @param fChecked button state
	 */
	inline void DrawRadioButton(const bool fChecked) const	{ glCallList(m_aDispList[ObjButtonRadio] + (fChecked ? 1 : 0)); }

	/**
	 * Draw single hex number
	 * @param nNum hex number
	 */
	inline void PrintHexNumber(const unsigned short nNum) const	{ assert(nNum < 16); glCallList(m_aDispList[ObjHexNum] + nNum); }

	/**
	 * Draw status bar
	 * @param nMapId map ID
	 */
	void DrawStatusBar(const unsigned int nMapId) const;


private:	//Class variables
	GLuint	m_aDispList[ObjCounter];	///< Objects lists
	GLuint	m_aTextures[TxrCounter];	///< Textures used by objects
	char m_pszLastErrorMessage[1024];	///< Latest error message

private:
	/**
	 * Draw box
	 * @param dX1 right coordinate
	 * @param dY1 up coordinate
	 * @param dZ1 near coordinate
	 * @param dWidth width of the box
	 * @param dHeight height of the box
	 * @param dDepth depth of the box
	 */
	void DrawBox(GLfloat dX1, GLfloat dY1, GLfloat dZ1, GLfloat dWidth, GLfloat dHeight, GLfloat dDepth) const;
};
