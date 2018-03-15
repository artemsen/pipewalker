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

#include "settings.h"

//Config file section and parameters names
const char* PWS_SECT_COMMON =   "common";      ///< Common properties section name
const char* PWS_SECT_LVLSM_P =  "level_sp";    ///< Small level map properties section name
const char* PWS_SECT_LVLSM_W =  "level_sw";    ///< Small level map properties section name
const char* PWS_SECT_LVLNL_P =  "level_np";    ///< Normal level map properties section name
const char* PWS_SECT_LVLNL_W =  "level_nw";    ///< Normal level map properties section name
const char* PWS_SECT_LVLBG_P =  "level_bp";    ///< Big level map properties section name
const char* PWS_SECT_LVLBG_W =  "level_bw";    ///< Big level map properties section name
const char* PWS_SECT_LVLEX_P =  "level_ep";    ///< Extra level map properties section name
const char* PWS_SECT_LVLEX_W =  "level_ew";    ///< Extra level map properties section name
const char* PWS_CPROP_THEME =   "theme";       ///< Theme file name field
const char* PWS_CPROP_SOUND =   "sound";       ///< Sound mode field
const char* PWS_CPROP_RNDLVL =  "rndlvl";      ///< Random level map mode field
const char* PWS_CPROP_LASTMAP = "last_lvl";    ///< Last map size
const char* PWS_LEVEL_ID =      "id";          ///< Last level Id
const char* PWS_LEVEL_STATE =   "state";       ///< Last level state

const char* PWS_FILENAME =      ".pipewalker"; ///< User settings file name


settings::settings()
:	_debug_mode(false),
	_rnd_lvl(false),
	_sound(true),
	_theme("Simple")
{
#ifndef NDEBUG
	_debug_mode = true;
#endif	//NDEBUG
}


settings& settings::instance()
{
	static settings i;
	return i;
}


void settings::load()
{
	settings& inst = instance();

	serializer sett;
	if (sett.load(inst._sett_file)) {
		inst._rnd_lvl = sett.get_value(PWS_SECT_COMMON, PWS_CPROP_RNDLVL, inst._rnd_lvl);
		inst._sound = sett.get_value(PWS_SECT_COMMON, PWS_CPROP_SOUND, inst._sound);
		inst._theme = sett.get_value(PWS_SECT_COMMON, PWS_CPROP_THEME, inst._theme);
		inst._last_level = sett.get_value(PWS_SECT_COMMON, PWS_CPROP_LASTMAP, inst._last_level);

		for (size_t i = 0; i < 8; ++i) {
			const char* section_name = inst.level_section_name(static_cast<level::size>(i / 2), (i % 2) != 0);
			assert(section_name && *section_name);
			inst._states[i].id = sett.get_value(section_name, PWS_LEVEL_ID, 1);
			inst._states[i].state = sett.get_value(section_name, PWS_LEVEL_STATE, string());
		}
	}
}


void settings::save()
{
	settings& inst = instance();

	serializer sett;
	sett.set_value(PWS_SECT_COMMON, PWS_CPROP_RNDLVL, inst._rnd_lvl);
	sett.set_value(PWS_SECT_COMMON, PWS_CPROP_SOUND, inst._sound);
	sett.set_value(PWS_SECT_COMMON, PWS_CPROP_THEME, inst._theme);
	sett.set_value(PWS_SECT_COMMON, PWS_CPROP_LASTMAP, inst._last_level);
	for (size_t i = 0; i < 8; ++i) {
		if (inst._states[i].id) {
			const char* section_name = inst.level_section_name(static_cast<level::size>(i / 2), (i % 2) != 0);
			assert(section_name && *section_name);
			sett.set_value(section_name, PWS_LEVEL_ID, static_cast<int>(inst._states[i].id));
			sett.set_value(section_name, PWS_LEVEL_STATE, inst._states[i].state);
		}
	}
	sett.save(inst._sett_file);
}


void settings::get_state(const level::size sz, const bool wrap, unsigned long& id, string& state)
{
	const level_state& ls = instance()._states[static_cast<size_t>(sz) * 2 + (wrap ? 1 : 0)];
	id = ls.id;
	state = ls.state;
}


void settings::set_state(const unsigned long id, const level::size sz, const bool wrap, const string& state)
{
	assert(id > 0);
	assert(!state.empty());

	settings& inst = instance();
	level_state& ls = inst._states[static_cast<size_t>(sz) * 2 + (wrap ? 1 : 0)];
	ls.id = id;
	ls.state = state;
	inst._last_level = inst.level_section_name(sz, wrap);
}


const char* settings::level_section_name(const level::size sz, const bool wrap) const
{
	switch (sz) {
		case level::sz_small:
			return wrap ? PWS_SECT_LVLSM_W : PWS_SECT_LVLSM_P;
		case level::sz_normal:
			return wrap ? PWS_SECT_LVLNL_W : PWS_SECT_LVLNL_P;
		case level::sz_big:
			return wrap ? PWS_SECT_LVLBG_W : PWS_SECT_LVLBG_P;
		case level::sz_extra:
			return wrap ? PWS_SECT_LVLEX_W : PWS_SECT_LVLEX_P;
		default:
			assert(false && "Unknown size");
	}
	return "";
}


level::size settings::last_size()
{
	const string& ll = instance()._last_level;
	if (ll.compare(PWS_SECT_LVLSM_P) == 0 || ll.compare(PWS_SECT_LVLSM_W) == 0)
		return level::sz_small;
	else if (ll.compare(PWS_SECT_LVLNL_P) == 0 || ll.compare(PWS_SECT_LVLNL_W) == 0)
		return level::sz_normal;
	else if (ll.compare(PWS_SECT_LVLBG_P) == 0 || ll.compare(PWS_SECT_LVLBG_W) == 0)
		return level::sz_big;
	else if (ll.compare(PWS_SECT_LVLEX_P) == 0 || ll.compare(PWS_SECT_LVLEX_W) == 0)
		return level::sz_extra;
	return level::sz_normal;
}



bool settings::last_wrap()
{
	const string& ll = instance()._last_level;
	return !(
		ll.compare(PWS_SECT_LVLSM_P) == 0 ||
		ll.compare(PWS_SECT_LVLNL_P) == 0 ||
		ll.compare(PWS_SECT_LVLBG_P) == 0 ||
		ll.compare(PWS_SECT_LVLEX_P) == 0);
}


bool settings::serializer::load(string& settings_file)
{
	settings_file.clear();

	//Try to open "portable" version settings file
#ifdef WIN32
	settings_file += ".\\";
#else
	settings_file += "./";
#endif //WIN32
	settings_file += PWS_FILENAME;

	ifstream fstr;
	fstr.open(settings_file.c_str(), ifstream::in);
	if (fstr.fail()) {
		//Try to open usual version settings file
#ifdef WIN32
		settings_file = getenv("USERPROFILE");
		settings_file += '\\';
#else
		settings_file = getenv("HOME");
		settings_file += '/';
#endif //WIN32
		settings_file += PWS_FILENAME;

		fstr.open(settings_file.c_str(), ifstream::in);
		if (fstr.fail())
			return false;	//No settings file
	}

	//Read all settings
	string cs_name;
	sett_value cs_val;
	string cfg_line;
	while (!fstr.eof()) {
		getline(fstr, cfg_line);
		trim(cfg_line);
		if (cfg_line.empty() || cfg_line[0] == '#')
			continue;
		if (cfg_line[0] == '[' && cfg_line[cfg_line.length() - 1] == ']') {
			if (!cs_val.empty() && !cs_name.empty())
				_settings[cs_name].insert(cs_val.begin(), cs_val.end());
			cs_val.clear();
			cs_name = cfg_line.substr(1, cfg_line.length() - 2);
		}
		else {
			const size_t delim = cfg_line.find('=');
			if (delim != string::npos) {
				pair<string, string> kv;
				kv.first = cfg_line.substr(0, delim);
				kv.second = cfg_line.substr(delim + 1);
				trim(kv.first);
				trim(kv.second);
				cs_val.insert(kv);
			}
		}
	}
	if (!cs_val.empty())
		_settings[cs_name].insert(cs_val.begin(), cs_val.end());
	fstr.close();

	return true;
}


void settings::serializer::save(const string& settings_file) const
{
	assert(!settings_file.empty());

	ofstream fstr;
	fstr.open(settings_file.c_str(), ofstream::out);
	if (!fstr.fail()) {
		for (sett_section::const_iterator it_sect = _settings.begin(); it_sect != _settings.end(); ++it_sect) {
			fstr << "[" << it_sect->first << "]" << endl;
			for (sett_value::const_iterator it_val = it_sect->second.begin(); it_val != it_sect->second.end(); ++it_val)
				fstr << it_val->first << "=" << it_val->second << endl;
			fstr << endl;
		}
		fstr.close();
	}
}


string settings::serializer::get_value(const string& section, const string& key, const string& default_value) const
{
	sett_section::const_iterator it_sec = _settings.find(section);
	if (it_sec == _settings.end())
		return default_value;
	sett_value::const_iterator it_val = it_sec->second.find(key);
	if (it_val == it_sec->second.end())
		return default_value;
	return it_val->second;
}


int settings::serializer::get_value(const string& section, const string& key, const int default_value) const
{
	const string val = get_value(section, key, string());
	return val.empty() ? default_value : atoi(val.c_str());
}


bool settings::serializer::get_value(const string& section, const string& key, const bool default_value) const
{
	return get_value(section, key, static_cast<int>(default_value ? 1 : 0)) != 0L;
}


void settings::serializer::set_value(const string& section, const string& key, const string& val)
{
	_settings[section][key] = val;
}


void settings::serializer::set_value(const string& section, const string& key, const int val)
{
	char buff[16];
	sprintf(buff, "%i", val);
	set_value(section, key, string(buff));
}


void settings::serializer::set_value(const string& section, const string& key, const bool val)
{
	set_value(section, key, static_cast<int>(val ? 1 : 0));
}


void settings::serializer::trim(string& val)
{
	while (!val.empty() && isspace(*val.begin()))
		val.erase(val.begin());
	while (!val.empty() && isspace(*val.rbegin()))
		val.erase(val.end() - 1);
}
