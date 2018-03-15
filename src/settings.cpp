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

#include "settings.h"
#include "buffer.h"

#define PW_SERIALIZE_MAPSIZE	"MapSize"			///< Map size field
#define PW_SERIALIZE_WRAPMODE	"Wrapping"			///< Map wrapping mode field
#define PW_SERIALIZE_THEME		"Theme"				///< Theme id field
#define PW_SERIALIZE_SOUND		"Sound"				///< Sound mode field

#define PW_SERIALIZE_MAPID_S	"MapIDSmall"		///< Map ID field
#define PW_SERIALIZE_MAPST_S	"MapStateSmall"		///< Map state field
#define PW_SERIALIZE_MAPID_N	"MapIDNormal"		///< Map ID field
#define PW_SERIALIZE_MAPST_N	"MapStateNormal"	///< Map state field
#define PW_SERIALIZE_MAPID_B	"MapIDBig"			///< Map ID field
#define PW_SERIALIZE_MAPST_B	"MapStateBig"		///< Map state field
#define PW_SERIALIZE_MAPID_E	"MapIDExtra"		///< Map ID field
#define PW_SERIALIZE_MAPST_E	"MapStateExtra"		///< Map state field

#define PW_SERIALIZE_FILENAME	".pipewalker"		///< Filename to save state


CSettings::CSettings()
:	Size(MapSizeNormal),
	Theme(ThemeNetwork),
	Wrapping(true),
	Sound(true),
	m_UsePortable(true)
{
}


void CSettings::Load()
{
	vector<KeyVal> sett;

	//Try to open portable version settings file
	bool usePortable = false;
	try {
		sett = LoadSettings(GetFileName().c_str());
		usePortable = true;
	}
	catch (CException& /*ex*/) {
	}

	//Try to open non-portable version settings file
	if (!usePortable) {
		m_UsePortable = false;
		try {
			sett = LoadSettings(GetFileName().c_str());
		}
		catch (CException& /*ex*/) {
		}
	}

	string val;

	if (GetVal(PW_SERIALIZE_MAPSIZE, val, sett))
		Size = static_cast<MapSize>(atoi(val.c_str()));
	if (GetVal(PW_SERIALIZE_THEME, val, sett))
		Theme = static_cast<DecorTheme>(atoi(val.c_str()));
	if (GetVal(PW_SERIALIZE_SOUND, val, sett))
		Sound = atoi(val.c_str()) != 0;
	if (GetVal(PW_SERIALIZE_WRAPMODE, val, sett))
		Wrapping = atoi(val.c_str()) != 0;
	
	if (GetVal(PW_SERIALIZE_MAPID_S, val, sett))
		m_MapSmall.Id = atoi(val.c_str());
	if (GetVal(PW_SERIALIZE_MAPST_S, val, sett))
		m_MapSmall.State = val;

	if (GetVal(PW_SERIALIZE_MAPID_N, val, sett))
		m_MapNormal.Id = atoi(val.c_str());
	if (GetVal(PW_SERIALIZE_MAPST_N, val, sett))
		m_MapNormal.State = val;

	if (GetVal(PW_SERIALIZE_MAPID_B, val, sett))
		m_MapBig.Id = atoi(val.c_str());
	if (GetVal(PW_SERIALIZE_MAPST_B, val, sett))
		m_MapBig.State = val;

	if (GetVal(PW_SERIALIZE_MAPID_E, val, sett))
		m_MapExtra.Id = atoi(val.c_str());
	if (GetVal(PW_SERIALIZE_MAPST_E, val, sett))
		m_MapExtra.State = val;
}


void CSettings::Save() const
{
	try {
		vector<KeyVal> sett;

		sett.push_back(KeyVal(PW_SERIALIZE_MAPSIZE, NumericToString(static_cast<int>(Size))));
		sett.push_back(KeyVal(PW_SERIALIZE_THEME, NumericToString(static_cast<int>(Theme))));
		sett.push_back(KeyVal(PW_SERIALIZE_SOUND, Sound ? "100" : "0"));
		sett.push_back(KeyVal(PW_SERIALIZE_WRAPMODE, Wrapping ? "1" : "0"));

		sett.push_back(KeyVal(PW_SERIALIZE_MAPID_S, NumericToString(static_cast<int>(m_MapSmall.Id))));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPST_S, m_MapSmall.State));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPID_N, NumericToString(static_cast<int>(m_MapNormal.Id))));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPST_N, m_MapNormal.State));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPID_B, NumericToString(static_cast<int>(m_MapBig.Id))));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPST_B, m_MapBig.State));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPID_E, NumericToString(static_cast<int>(m_MapExtra.Id))));
		sett.push_back(KeyVal(PW_SERIALIZE_MAPST_E, m_MapExtra.State));

		SaveSettings(sett, GetFileName().c_str());
	}
	catch (CException& /*ex*/) {
	}
}


unsigned long CSettings::GetCurrentMapId() const
{
	switch (Size) {
		case MapSizeSmall:	return m_MapSmall.Id;
		case MapSizeNormal:	return m_MapNormal.Id;
		case MapSizeBig:	return m_MapBig.Id;
		case MapSizeExtra:	return m_MapExtra.Id;
		default:			assert(false && "Unknown size");
	}
	return 0;
}


void CSettings::SetCurrentMapId(const unsigned long id)
{
	switch (Size) {
		case MapSizeSmall:	m_MapSmall.Id = id;		break;
		case MapSizeNormal:	m_MapNormal.Id = id;	break;
		case MapSizeBig:	m_MapBig.Id = id;		break;
		case MapSizeExtra:	m_MapExtra.Id = id;		break;
		default:			assert(false && "Unknown size");
	}
}


string CSettings::GetCurrentMapState() const
{
	switch (Size) {
		case MapSizeSmall:	return m_MapSmall.State;
		case MapSizeNormal:	return m_MapNormal.State;
		case MapSizeBig:	return m_MapBig.State;
		case MapSizeExtra:	return m_MapExtra.State;
		default:			assert(false && "Unknown size");
	}
	return string();
}


void CSettings::SetCurrentMapState(const string& state)
{
	switch (Size) {
		case MapSizeSmall:	m_MapSmall.State = state;	break;
		case MapSizeNormal:	m_MapNormal.State = state;	break;
		case MapSizeBig:	m_MapBig.State = state;		break;
		case MapSizeExtra:	m_MapExtra.State = state;	break;
		default:			assert(false && "Unknown size");
	}
}


bool CSettings::GetVal(const string& key, string& val, const vector<KeyVal>& sett) const
{
	bool found = false;
	for (vector<KeyVal>::const_iterator it = sett.begin(); !found && it != sett.end(); ++it) {
		if (it->Key.compare(key) == 0) {
			found = true;
			val = it->Value;
		}
	}
	return found;
}


vector<CSettings::KeyVal> CSettings::LoadSettings(const char* fileName) const
{
	assert(fileName);
	
	vector<KeyVal> result;

	CBuffer buf;
	buf.Load(fileName);

	while (!buf.EOB()) {
		const string str = buf.GetString();
		if (str.empty() || str[0] == ';' || str[0] == '#')
			continue;

		const size_t delimPos = str.find('=');
		if (delimPos != string::npos) {
			KeyVal kv;
			kv.Key = str.substr(0, delimPos);
			kv.Value = str.substr(delimPos + 1);
			
			//Trim
			const char* trimSpaces = " \t";
			size_t sp = 0;
			while ((sp = kv.Key.find_first_of(trimSpaces)) != string::npos)
				kv.Key.erase(sp, 1);
			sp = 0;
			while ((sp = kv.Value.find_first_of(trimSpaces)) != string::npos)
				kv.Value.erase(sp, 1);
			
			result.push_back(kv);
		}
	}

	return result;
}


void CSettings::SaveSettings(const vector<KeyVal>& sett, const char* fileName) const
{
	CBuffer buf;
	buf.Put(string("# This is the user settings file of the PipeWalker game\n# Please, do not edit it manually\n\n"));

	for (vector<KeyVal>::const_iterator it = sett.begin(); it != sett.end(); ++it) {
		buf.Put(it->Key);
		buf.PutData(" = ", 3);
		buf.Put(it->Value);
		buf.PutData("\n", 1);
	}

	buf.Save(fileName);
}


string CSettings::GetFileName() const
{
	string loadFileName = m_UsePortable ? "." : getenv(
#ifdef WIN32
		"USERPROFILE"
#else
		"HOME"
#endif //WIN32
		);

#ifdef WIN32
	loadFileName += '\\';
#else
	loadFileName += '/';
#endif //WIN32

	loadFileName += PW_SERIALIZE_FILENAME;
	return loadFileName;
}
