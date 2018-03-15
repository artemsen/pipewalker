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
#include "map.h"
#include "texture.h"


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

	/**
	 * Get map Id
	 * \return map id
	 */
	unsigned long GetCurrentMapId() const;

	/**
	 * Set map Id
	 * \param id map id
	 */
	void SetCurrentMapId(const unsigned long id);

	/**
	 * Get map state
	 * \return map state
	 */
	string GetCurrentMapState() const;

	/**
	 * Set map state
	 * \param state current map state
	 */
	void SetCurrentMapState(const string& state);

private:
	//! Key-value structure
	struct KeyVal {
		KeyVal(const string k = string(), const string v = string()) : Key(k), Value(v) {}
		string Key;
		string Value;
	};

	/**
	 * Get value from key-value array
	 * \param key search key
	 * \param val value
	 * \param sett key-value array
	 * \return false if not found
	 */
	bool GetVal(const string& key, string& val, const vector<KeyVal>& sett) const;

	/**
	 * Get file name
	 * \return file name
	 */
	string GetFileName() const;

	/**
	 * Load settings
	 * \param fileName file name for load settings
	 * \return map with ini values
	 */
	vector<KeyVal> LoadSettings(const char* fileName) const;

	/**
	 * Save settings
	 * \param sett map with ini values
	 * \param fileName file name to save settings
	 */
	void SaveSettings(const vector<KeyVal>& sett, const char* fileName) const;

	/**
	 * Convert numeric to string value
	 * \param num source numeric
	 * \return string value
	 */
	inline string NumericToString(const int num) const
	{
		string r(12, 0);
		sprintf(&r[0], "%i", num);
		return r.c_str();
	}

public:
	//Application properties
	MapSize			Size;			///< Current map size (in cell)
	DecorTheme		Theme;			///< Theme
	bool			Wrapping;		///< Wrapping mode on/off flag
	bool			Sound;			///< Sound on/off flag

private:
	struct MapDescr {
		MapDescr() : Id(1) {}
		unsigned long Id;	///< Map Id
		string State;		///< Map state
	};
	//Saved map properties
	MapDescr		m_MapSmall;		///< Small map description
	MapDescr		m_MapNormal;	///< Normal map description
	MapDescr		m_MapBig;		///< Big map description
	MapDescr		m_MapExtra;		///< Extra map description

private:
	bool			m_UsePortable;	///< Type of used file path
};
