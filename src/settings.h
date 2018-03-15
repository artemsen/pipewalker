/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2012 by Artem Senichev <artemsen@gmail.com>        *
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
#include "level.h"


class settings
{
private:
	settings();

public:
	/**
	 * Get class instance
	 * \return class instance
	 */
	static settings& instance();

	/**
	 * Load settings
	 */
	static void load();

	/**
	 * Save settings
	 */
	static void save();

	/**
	 * Get level state for specified size and mode
	 * \param sz level size
	 * \param wrap wrap mode
	 * \param id last saved level id
	 * \param state last saved level state
	 */
	static void get_state(const level::size sz, const bool wrap, unsigned long& id, string& state);

	/**
	 * Set level state for specified id, size and mode
	 * \param id level Id
	 * \param size level size
	 * \param wrap wrap mode
	 * \param state level state
	 */
	static void set_state(const unsigned long id, const level::size sz, const bool wrap, const string& state);

	//Accessors
	static bool debug_mode()                 { return instance()._debug_mode; }
	static void debug_mode(const bool mode)  { instance()._debug_mode = mode; }
	static bool rndlvl_mode()                { return instance()._rnd_lvl; }
	static void rndlvl_mode(const bool mode) { instance()._rnd_lvl = mode; }
	static bool sound_mode()                 { return instance()._sound; }
	static void sound_mode(const bool mode)  { instance()._sound = mode; }
	static const char* theme()               { return instance()._theme.c_str(); }
	static void theme(const char* name)      { assert(name); instance()._theme = name; }
	static level::size last_size();
	static bool last_wrap();

private:
	/**
	 * Get level section name
	 * \param sz level size
	 * \param wrap_mode wrap mode
	 * \return section name
	 */
	const char* level_section_name(const level::size sz, const bool wrap) const;

private:
	/**
	 * Load / save game settings
	 */
	class serializer
	{
	public:
		/**
		 * Load settings
		 * \param settings_file used settings file path
		 * \return true if settings loaded
		 */
		bool load(string& settings_file);

		/**
		 * Save settings
		 * \param settings_file used settings file path
		 */
		void save(const string& settings_file) const;

		//Get value
		string get_value(const string& section, const string& key, const string& default_value) const;
		int get_value(const string& section, const string& key, const int default_value) const;
		bool get_value(const string& section, const string& key, const bool default_value) const;

		//Set values
		void set_value(const string& section, const string& key, const string& val);
		void set_value(const string& section, const string& key, const int val);
		void set_value(const string& section, const string& key, const bool val);

	private:
		/**
		 * Trim string
		 * \param val source string
		 */
		void trim(string& val);

	private:
		typedef map<string, string> sett_value;
		typedef map<string, sett_value> sett_section;
		sett_section _settings;
	};


private:
	//! Saved level description
	struct level_state {
		unsigned long id;    ///< Level id
		string        state; ///< Level state
	};

	string      _sett_file;  ///< Full path to user settings file

	bool        _debug_mode; ///< Using debug mode flag

	bool        _rnd_lvl;    ///< Random level map flag
	bool        _sound;      ///< Use sound flag
	string      _theme;      ///< Default theme file
	string      _last_level; ///< Last saved level name (one of PWS_SECT_LVL*)
	level_state _states[8];  ///< Level states (8 = all sizes * wrap modes)
};
