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

#include "cell.h"

//! Map sizes
enum MapSize {
	MapSizeSmall  = 10,
	MapSizeNormal = 14,
	MapSizeBig    = 20,
	MapSizeExtra  = 30
};


class CMap
{
public:
	//! Default constructor
	CMap();

	/**
	 * Initialize new random map
	 * \param mapSize map size (in cell)
	 * \param mapId map ID (begin value for srand())
	 * \param wrapMode wrap mode on/off flag
	 */
	void New(const MapSize mapSize, const unsigned long mapId, const bool wrapMode);

	/**
	 * Load map state from string
	 * \param mapSize map size (in cell)
	 * \param descr map state description
	 * \param wrapMode wrap mode on/off flag
	 * \return false if loading fail
	 */
	bool LoadMap(const MapSize mapSize, const string& descr, const bool wrapMode);

	/**
	 * Save map state as text
	 * \return map state
	 */
	string SaveMap() const;

	/**
	 * Reset by rotate current map
	 */
	void ResetByRotate();

	/**
	 * Get "game over" flag
	 * \return "game over" flag
	 */
	inline bool IsGameOver() const	{ return m_GameOver; }

	/**
	 * Get object's cell
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \return pointer to object cell
	 */
	CCell& GetCell(const unsigned short x, const unsigned short y);

	/**
	 * Get object's cell
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \return pointer to object cell
	 */
	const CCell& GetCell(const unsigned short x, const unsigned short y) const;

	/**
	* Define connection status
	*/
	void DefineConnectStatus();

	/**
	 * Get current map size
	 * \return current map size
	 */
	MapSize GetMapSize() const	{ return static_cast<MapSize>(m_MapSize); }


private:
	/**
	 * Check cells status for "game over" (Define connection status for all receivers)
	 * \return "game over" flag
	 */
	bool CheckForGameOver();

	/**
	 * Install receiver on map
	 * \return result (false if it is unable to install)
	 */
	bool InstallReceiver();

	/**
	 * Install sender on map
	 */
	void InstallSender();

	/**
	 * Make route from receiver to sender
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \return result (false if route is unaccessible)
	 */
	bool MakeRoute(const unsigned short x, const unsigned short y);

	/**
	 * Make connection between two cell
	 * \param currX an X coordinate of source cell
	 * \param currY an Y coordinate of source cell
	 * \param nextX an X coordinate of destination cell
	 * \param nextY an Y coordinate of destination cell
	 */
	void MakeConnection(const unsigned short currX, const unsigned short currY, const unsigned short nextX, const unsigned short nextY);

	/**
	 * Define connection status
	 * \param x an X coordinates of latest connected cell
	 * \param y an Y coordinates of latest connected cell
	 */
	void DefineConnectStatus(const unsigned short x, const unsigned short y);


private:	//Class variables
	vector<CCell>	m_CellMap;				///< Object's map
	bool			m_GameOver;				///< Game over flag
	bool			m_WrapMode;				///< Wrapping mode on/off flag
	unsigned short	m_SenderX, m_SenderY;	///< Sender coordinates
	unsigned short	m_ZeroX, m_ZeroY;		///< Zero point coordinates (sender out)
	unsigned short	m_MapSize;				///< Map size
};
