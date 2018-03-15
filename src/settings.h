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
 * CSettings - load / save game settungs
 */
class CSettings
{
protected:
	/**
	 * Load state
	 * \param fileName settings file name
	 * \return map with settings values
	 */
	map<string, string> Load(const char* fileName) const;

	/**
	 * Save state
	 * \param fileName settings file name
	 * \param settings map with settings values
	 * \param firstComment first string commentary
	 */
	void Save(const char* fileName, const map<string, string>& settings, const char* firstComment = NULL) const;

	/**
	 * Trim string (remove spaces at the end and begining the string)
	 * \param val source string
	 */
	void Trim(string& val) const;

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
};


/**
 * CGameSettings - game settings
 */
class CGameSettings : public CSettings
{
public:
	/**
	 * Load state
	 */
	void Load();

	//! Theme description
	struct Theme {
		string Name;
		string TextureFile;
		float  TextColor[3];
		float  TitleColor[3];
	};

private:
	/**
	 * Convert string to float values
	 * \param src source string (0.1 0.2 0.3 for example)
	 * \param val output values
	 */
	void ConvertToFloat(const string& src, float val[3]) const;

public:
	//Application properties
	vector<Theme> Themes;	///< Avialable themes
};


/**
 * CUserSettings - load / save user game settings
 */
class CUserSettings : public CSettings
{
public:
	//! Default constructor
	CUserSettings();

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
	/**
	 * Get user settings file name
	 * \return file name
	 */
	string GetFileName() const;

public:
	//User settings
	MapSize				Size;		///< Current map size (in cell)
	size_t				ThemeId;	///< Current theme index
	bool				Wrapping;	///< Wrapping mode on/off flag
	bool				Sound;		///< Sound on/off flag

private:
	struct MapDescr {
		MapDescr() : Id(1) {}
		unsigned long Id;	///< Map Id
		string State;		///< Map state
	};
	//Saved map properties
	MapDescr		_MapSmall;		///< Small map description
	MapDescr		_MapNormal;		///< Normal map description
	MapDescr		_MapBig;		///< Big map description
	MapDescr		_MapExtra;		///< Extra map description

private:
	bool			_UsePortable;	///< Type of used user settings file path
};
