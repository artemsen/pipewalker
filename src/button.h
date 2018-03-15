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
#include "texture.h"

class CGame;

#ifdef _MSC_VER
#pragma warning(disable: 4512)	//assignment operator could not be generated
#endif // _MSC_VER


/**
 * CButton - Simple button
 */
class CButton
{
public:
	/**
	 * Constructor
	 * \param game the game instance
	 */
	CButton(CGame& game);

	/**
	 * Constructor
	 * \param game the game instance
	 * \param x button x coordinate
	 * \param y button y coordinate
	 * \param width button width
	 * \param height button height
	 * \param tex button texture identifier
	 * \param id button texture ID
	 */
	CButton(CGame& game, const float x, const float y, const float width, const float height, const CTextureBank::TextureType tex, const int id);

	/**
	 * Button initialization
	 * \param x button x coordinate
	 * \param y button y coordinate
	 * \param width button width
	 * \param height button height
	 * \param tex button texture identifier
	 * \param id button ID
	 */
	void Init(const float x, const float y, const float width, const float height, const CTextureBank::TextureType tex, const int id);

	/**
	 * Check for cross mouse and button coordinates
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 * \return true if mouse pointer is over button
	 */
	bool IsMouseOver(const float x, const float y) const;

	/**
	 * Get button ID
	 * \return button ID
	 */
	int GetId() const	{ return _BtnId; }

	/**
	 * Render button
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 */
	virtual void Render(const float x, const float y) const;


protected:
	/**
	 * Render button
	 * \param x mouse x coordinate
	 * \param y mouse y coordinate
	 * \param texture texture identifier
	 */
	void RenderButton(const float x, const float y, const CTextureBank::TextureType texture) const;


protected:	//Class variables
	CGame&	_Game;		///< Game instance
	float	_X;		///< Button x coordinate
	float	_Y;		///< Button y coordinate
	float	_Width;	///< Button width
	float	_Height;	///< Button height
	int		_BtnId;	///< Button ID
	CTextureBank::TextureType	_TexId;	///< Button texture identifier
};


/**
 * CCheckBoxButton - Simple check box button
 */
class CCheckBoxButton : public CButton
{
public:
	/**
	 * Constructor
	 * \param game the game instance
	 */
	CCheckBoxButton(CGame& game) : CButton(game), _State(false), _TexOff(CTextureBank::TexCounter) {}

	/**
	 * Constructor
	 * \param game the game instance
	 * \param state initial button state (on/off)
	 * \param x button x coordinate
	 * \param y button y coordinate
	 * \param width width button width
	 * \param height button height
	 * \param texOn button on-state texture identifier
	 * \param texOff button off-state texture identifier
	 * \param id button ID
	 */
	CCheckBoxButton(CGame& game, const bool state, const float x, const float y, const float width, const float height, const CTextureBank::TextureType texOn, const CTextureBank::TextureType texOff, const int id);

	/**
	 * Get button state
	 * \return button state
	 */
	bool GetState()	const				{ return _State; }

	/**
	 * Set button state
	 * \param newState new state
	 */
	void SetState(const bool newState)	{ _State = newState; }

	// From CButton
	virtual void Render(const float x, const float y) const;

protected:
	bool	_State;	///< Button state (on/off)
	CTextureBank::TextureType	_TexOff;	///< Button second texture identifier
};


/**
 * CRadioButton - Simple radio buttons group
 */
class CRadioButtons
{
public:
	/**
	 * Add button to group
	 * \param btn adding button
	 */
	void AddButton(const CCheckBoxButton& btn);

	/**
	 * Render radio button group
	 * \param mouseX mouse x coordinate
	 * \param mouseY mouse y coordinate
	 */
	void Render(const float mouseX, const float mouseY) const;

	/**
	 * On mouse click handler
	 * \param mouseX mouse x coordinate
	 * \param mouseY mouse y coordinate
	 * \return true if state has been changed
	 */
	bool OnClick(const float mouseX, const float mouseY);

	/**
	 * Get current choice id
	 * \return current choice id
	 */
	int GetChoice() const;

	/**
	 * Set current choice id
	 * \param choiceId current choice id
	 */
	void SetChoice(const int choiceId);

private:
	list<CCheckBoxButton>	_Buttons;	///< Buttons group
};
