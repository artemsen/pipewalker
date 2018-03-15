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
 * CSerializer - load / save game state
 */
class CSerializer
{
public:
	/**
	 * Load state
	 * \param mapId map ID
	 * \param mapState map state
	 * \return false if error
	 */
	static bool Load(unsigned int& mapId, string& mapState);

	/**
	 * Save state
	 * \param mapId map ID
	 * \param mapState map state
	 * \return false if error
	 */
	static bool Save(const unsigned int mapId, const char* mapState);

private:
	/**
	 * Get file name
	 * \return file name
	 */
	static string GetFileName();
};


/**
 * CIniFile - work with Ini files
 */
class CIniFile
{
public:
	/**
	 * Load ini file
	 * \param fileName file name
	 */
	void Load(const char* fileName);

	/**
	 * Save ini file
	 * \param fileName file name
	 */
	void Save(const char* fileName) const;

	/**
	 * Get property
	 * \param name property name
	 * \return property value
	 */
	string GetProperty(const char* name) const;

	/**
	 * Set property
	 * \param name property name
	 * \param value property value
	 */
	void SetProperty(const char* name, const char* value);

	/**
	 * Set property
	 * \param name property name
	 * \param value property value
	 */
	void SetProperty(const char* name, const unsigned int value);

private:
	map<string, string>	m_Ini;	///< Ini values
};
