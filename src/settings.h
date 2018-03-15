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
#include "map.h"


/**
 * CSettings - load / save game state
 */
class CSettings
{
public:
	//! Default constructor
	CSettings();

	/**
	 * Load state
	 */
	void Load();

	/**
	 * Save state
	 */
	void Save() const;

private:
	/**
	 * Get file name
	 * \return file name
	 */
	string GetFileName() const;

public:
	//Application properties
	unsigned long	MapId;		///< Map MapId
	MapSize			Size;		///< Map size (in cell)
	string			State;		///< Map state
	//bool			Blackout;	///< Blackout mode on/off flag
	bool			Wrapping;	///< Wrapping mode on/off flag
	bool			Sound;		///< Sound on/off flag
};