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

#include "settings.h"
#include "buffer.h"

#define PW_SERIALIZE_MAPID		"MapID"				///< Map ID field
#define PW_SERIALIZE_MAPSTATE	"MapState"			///< Map state field
#define PW_SERIALIZE_MAPSIZE	"MapSize"			///< Map size field
#define PW_SERIALIZE_WRAPMODE	"Wrapping"			///< Map wrapping mode field
//#define PW_SERIALIZE_BLACKOUT	"Blackout"			///< Blackout mode field
#define PW_SERIALIZE_SOUND		"Sound"				///< Sound mode field

#define PW_SERIALIZE_FILENAME	".pipewalker"		///< Filename to save state


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
	void Load(const char* fileName)
	{
		assert(fileName);

		CBuffer buf;
		buf.Load(fileName);

		while (!buf.EOB()) {
			string name;
			name.reserve(32);
			while (!buf.EOB()) {
				char c = 0;
				buf.Get(c);
				if (c == '\r')
					continue;
				if (c != '=')
					name += c;
				else
					break;
			}

			string val;
			val.reserve(32);
			while (!buf.EOB()) {
				char c = 0;
				buf.Get(c);
				if (c == '\r')
					continue;
				if (c != '\n')
					val += c;
				else
					break;
			}

			SetProperty(name.c_str(), val.c_str());
		}
	}

	/**
	 * Save ini file
	 * \param fileName file name
	 */
	void Save(const char* fileName) const
	{
		assert(fileName);

		CBuffer buf;

		for (map<string, string>::const_iterator it = m_Ini.begin(); it != m_Ini.end(); ++it) {
			buf.Put(it->first);
			buf.PutData("=", 1);
			buf.Put(it->second);
			buf.PutData("\n", 1);
		}

		buf.Save(fileName);
	}

	/**
	 * For for property existing
	 * \param name property name
	 * \return true if property has been set
	 */
	inline bool ExistProperty(const char* name) const		{ assert(name); return m_Ini.find(name) != m_Ini.end(); }


	/**
	 * Get property
	 * \param name property name
	 * \return property value
	 */
	string GetProperty(const char* name) const
	{
		assert(name);
		map<string, string>::const_iterator it = m_Ini.find(name);
		return (it != m_Ini.end() ? it->second : string());
	}

	/**
	 * Set property
	 * \param name property name
	 * \param value property value
	 */
	void SetProperty(const char* name, const char* value)	{ assert(name && value); m_Ini.insert(make_pair(name, value)); }


	/**
	 * Set property
	 * \param name property name
	 * \param value property value
	 */
	void SetProperty(const char* name, const unsigned long value)
	{
		char sz[16];
		sprintf(sz, "%u", static_cast<unsigned int>(value));
		SetProperty(name, sz);
	}

private:
	map<string, string>	m_Ini;	///< Ini values
};


CSettings::CSettings()
:	MapId(1),
	Size(MapSizeNormal),
	//Blackout(false),
	Wrapping(true),
	Sound(true)
{
}


void CSettings::Load()
{
	try {
		CIniFile ini;
		ini.Load(GetFileName().c_str());

		if (ini.ExistProperty(PW_SERIALIZE_MAPSTATE))
			State = ini.GetProperty(PW_SERIALIZE_MAPSTATE);

		if (ini.ExistProperty(PW_SERIALIZE_MAPID))
			MapId = atoi(ini.GetProperty(PW_SERIALIZE_MAPID).c_str());

		if (ini.ExistProperty(PW_SERIALIZE_MAPSIZE))
			Size = static_cast<MapSize>(atoi(ini.GetProperty(PW_SERIALIZE_MAPSIZE).c_str()));

		if (ini.ExistProperty(PW_SERIALIZE_SOUND))
			Sound = atoi(ini.GetProperty(PW_SERIALIZE_SOUND).c_str()) != 0;

		if (ini.ExistProperty(PW_SERIALIZE_WRAPMODE))
			Wrapping = atoi(ini.GetProperty(PW_SERIALIZE_WRAPMODE).c_str()) != 0;

// 		if (ini.ExistProperty(PW_SERIALIZE_BLACKOUT))
// 			Blackout = atoi(ini.GetProperty(PW_SERIALIZE_BLACKOUT).c_str()) != 0;
	}
	catch (CException& /*ex*/) {
	}
}


void CSettings::Save() const
{
	const string fileName = GetFileName();

	try {
		CIniFile ini;

		ini.SetProperty(PW_SERIALIZE_MAPID, MapId);
		ini.SetProperty(PW_SERIALIZE_MAPSTATE, State.c_str());
		ini.SetProperty(PW_SERIALIZE_MAPSIZE, Size);
		ini.SetProperty(PW_SERIALIZE_SOUND, Sound ? 100 : 0);
		ini.SetProperty(PW_SERIALIZE_WRAPMODE, Wrapping ? 1 : 0);
		//ini.SetProperty(PW_SERIALIZE_BLACKOUT, Blackout ? 1 : 0);

		ini.Save(fileName.c_str());
	}
	catch (CException& /*ex*/) {
	}
}


string CSettings::GetFileName() const
{
#ifdef PW_PORTABLE
	string loadFileName = ".";
#else
	string loadFileName = getenv(
#ifdef WIN32
		"USERPROFILE"
#else
		"HOME"
#endif //WIN32
		);
#endif //PW_PORTABLE

#ifdef WIN32
	loadFileName += '\\';
#else
	loadFileName += '/';
#endif //WIN32

	loadFileName += PW_SERIALIZE_FILENAME;
	return loadFileName;
}
