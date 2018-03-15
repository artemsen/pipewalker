/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2009 by Artem A. Senichev <artemsen@gmail.com>     *
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

/**
 * CButton - Simple button
 */
class CButton
{
public:
	//! Default constructor
	CButton();

	/**
	 * Constructor
	 * \param x button x coordinate
	 * \param y button y coordinate
	 * \param width button width
	 * \param height button height
	 * \param tex button texture identifier
	 * \param id button ID
	 */
	CButton(const float x, const float y, const float width, const float height, const GLuint tex, const int id);

	/**
	 * Check for cross mouse and button coordinates
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 * \return true if mouse pointer is over button
	 */
	virtual bool IsMouseOver(const float x, const float y) const;

	/**
	 * Render button
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 */
	virtual void Render(const float x, const float y) const;

	/**
	 * Get button ID
	 * \return button ID
	 */
	virtual int GetId() const	{ return m_BtnId; }


protected:
	/**
	 * Render button
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 * \param texture texture identifier
	 */
	virtual void RenderButton(const float x, const float y, const GLuint texture) const;


protected:	//Class variables
	float	m_X;		///< Button x coordinate
	float	m_Y;		///< Button y coordinate
	float	m_Width;	///< Button width
	float	m_Height;	///< Button height
	GLuint	m_TexId;	///< Button texture identifier
	int		m_BtnId;	///< Button ID
};


/**
 * CRadioButton - Simple radio button
 */
class CRadioButton : public CButton
{
public:
	//! Default constructor
	CRadioButton() : CButton(), m_State(false), m_TexOff(0) {}

	/**
	 * Constructor
	 * \param state initial button state (on/off)
	 * \param x button x coordinate
	 * \param y button y coordinate
	 * \param width width button width
	 * \param height button height
	 * \param texOn button on-state texture identifier
	 * \param texOff button off-state texture identifier
	 * \param id button ID
	 */
	CRadioButton(const bool state, const float x, const float y, const float width, const float height, const GLuint texOn, const GLuint texOff, const int id);

	/**
	 * Get button state
	 * \return button state
	 */
	bool GetState()	const				{ return m_State; }

	/**
	 * Set button state
	 * \param newState new state
	 */
	void SetState(const bool newState)	{ m_State = newState; }

	// From CButton
	virtual void Render(const float x, const float y) const;

protected:
	bool	m_State;	///< Button state (on/off)
	GLuint	m_TexOff;	///< Button second texture identifier
};
