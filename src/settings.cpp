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

static const char* PW_SERIALIZE_MAPSIZE = "MapSize";			///< Map size field
static const char* PW_SERIALIZE_WRAPMODE = "Wrapping";			///< Map wrapping mode field
static const char* PW_SERIALIZE_THEME = "Theme";				///< Theme id field
static const char* PW_SERIALIZE_SOUND = "Sound";				///< Sound mode field

static const char* PW_SERIALIZE_MAPID_S = "MapIDSmall";			///< Map ID field
static const char* PW_SERIALIZE_MAPST_S = "MapStateSmall";		///< Map state field
static const char* PW_SERIALIZE_MAPID_N = "MapIDNormal";		///< Map ID field
static const char* PW_SERIALIZE_MAPST_N = "MapStateNormal";		///< Map state field
static const char* PW_SERIALIZE_MAPID_B = "MapIDBig";			///< Map ID field
static const char* PW_SERIALIZE_MAPST_B = "MapStateBig";		///< Map state field
static const char* PW_SERIALIZE_MAPID_E = "MapIDExtra";			///< Map ID field
static const char* PW_SERIALIZE_MAPST_E = "MapStateExtra";		///< Map state field

static const char* PW_USRSETTINGS_FILENAME = ".pipewalker";		///< Filename to load/save user settings
static const char* PW_APPSETTINGS_FILENAME = "settings.ini";	///< Filename to load game application settings


map<string, string> CSettings::Load(const char* fileName) const
{
	assert(fileName);

	map<string, string> result;

	CBuffer buf;
	buf.Load(fileName);

	while (!buf.EOB()) {
		const string str = buf.GetString();
		if (str.empty() || str[0] == ';' || str[0] == '#')
			continue;

		const size_t delimPos = str.find('=');
		if (delimPos != string::npos) {
			pair<string, string> kv;
			kv.first = str.substr(0, delimPos);
			kv.second = str.substr(delimPos + 1);
			Trim(kv.first);
			Trim(kv.second);
			result.insert(kv);
		}
	}

	return result;
}


void CSettings::Save(const char* fileName, const map<string, string>& settings, const char* firstComment /*= NULL*/) const
{
	assert(fileName);

	CBuffer buf;
	if (firstComment)
		buf.Put(string(firstComment));

	for (map<string, string>::const_iterator it = settings.begin(); it != settings.end(); ++it) {
		buf.Put(it->first);
		buf.PutData(" = ", 3);
		buf.Put(it->second);
		buf.PutData("\n", 1);
	}

	buf.Save(fileName);
}


void CSettings::Trim(string& val) const
{
	while (!val.empty() && (val[0] == ' ' || val[0] == '\t'))
		val.erase(0, 1);
	while (!val.empty() && (val[val.length() - 1] == ' ' || val[val.length() - 1] == '\t'))
		val.erase(val.length() - 1);
}


void CGameSettings::Load()
{
	string fileName = DIR_GAMEDATA;
	fileName += PW_APPSETTINGS_FILENAME;

	const map<string, string> settings = CSettings::Load(fileName.c_str());
	int i = 0;
	while (++i) {	//Eternal
		char themeName[32], themeFile[32], themeTxtColor[32], themeTtlColor[32];
		sprintf(themeName, "Theme%iName", i);
		sprintf(themeFile, "Theme%iTexture", i);
		sprintf(themeTxtColor, "Theme%iTxtColor", i);
		sprintf(themeTtlColor, "Theme%iTtlColor", i);
		map<string, string>::const_iterator itName = settings.find(themeName);
		map<string, string>::const_iterator itFile = settings.find(themeFile);
		map<string, string>::const_iterator itTxtColor = settings.find(themeTxtColor);
		map<string, string>::const_iterator itTtlColor = settings.find(themeTtlColor);
		if (itName == settings.end() || itFile == settings.end() || itTxtColor == settings.end() || itTtlColor == settings.end())
			break;

		Theme theme;
		theme.Name = itName->second;
		theme.TextureFile = itFile->second;
		ConvertToFloat(itTxtColor->second, theme.TextColor);
		ConvertToFloat(itTtlColor->second, theme.TitleColor);
		Themes.push_back(theme);
	}
}


void CGameSettings::ConvertToFloat(const string& src, float val[3]) const
{
	val[0] = val[1] = val[2] = 0.0f;

	if (src.length() != 11 /* "0.1 0.2 0.3" */)
		return;
	val[0] = static_cast<float>(atof(src.c_str() + 0));
	val[1] = static_cast<float>(atof(src.c_str() + 4));
	val[2] = static_cast<float>(atof(src.c_str() + 8));
}


CUserSettings::CUserSettings()
:	Size(MapSizeNormal),
	ThemeId(0),
	Wrapping(true),
	Sound(true),
	_UsePortable(true)
{
}


void CUserSettings::Load()
{
	map<string, string> settings;

	//Try to open portable version settings file
	bool usePortable = false;
	try {
		settings = CSettings::Load(GetFileName().c_str());
		usePortable = true;
	}
	catch (CException& /*ex*/) {
	}

	//Try to open non-portable version settings file
	if (!usePortable) {
		_UsePortable = false;
		try {
			settings = CSettings::Load(GetFileName().c_str());
		}
		catch (CException& /*ex*/) {
		}
	}

	//Common settings
	if (settings.find(PW_SERIALIZE_MAPSIZE) != settings.end())
		Size = static_cast<MapSize>(atoi(settings[PW_SERIALIZE_MAPSIZE].c_str()));
	if (settings.find(PW_SERIALIZE_THEME) != settings.end())
		ThemeId = static_cast<size_t>(atoi(settings[PW_SERIALIZE_THEME].c_str()));
	if (settings.find(PW_SERIALIZE_SOUND) != settings.end())
		Sound = (atoi(settings[PW_SERIALIZE_SOUND].c_str()) != 0);
	if (settings.find(PW_SERIALIZE_WRAPMODE) != settings.end())
		Wrapping = (atoi(settings[PW_SERIALIZE_WRAPMODE].c_str()) != 0);
	
	//Maps properties
	if (settings.find(PW_SERIALIZE_MAPID_S) != settings.end())
		_MapSmall.Id = atoi(settings[PW_SERIALIZE_MAPID_S].c_str());
	if (settings.find(PW_SERIALIZE_MAPST_S) != settings.end())
		_MapSmall.State = settings[PW_SERIALIZE_MAPST_S];

	if (settings.find(PW_SERIALIZE_MAPID_N) != settings.end())
		_MapNormal.Id = atoi(settings[PW_SERIALIZE_MAPID_N].c_str());
	if (settings.find(PW_SERIALIZE_MAPST_N) != settings.end())
		_MapNormal.State = settings[PW_SERIALIZE_MAPST_N];

	if (settings.find(PW_SERIALIZE_MAPID_B) != settings.end())
		_MapBig.Id = atoi(settings[PW_SERIALIZE_MAPID_B].c_str());
	if (settings.find(PW_SERIALIZE_MAPST_B) != settings.end())
		_MapBig.State = settings[PW_SERIALIZE_MAPST_B];

	if (settings.find(PW_SERIALIZE_MAPID_E) != settings.end())
		_MapExtra.Id = atoi(settings[PW_SERIALIZE_MAPID_E].c_str());
	if (settings.find(PW_SERIALIZE_MAPST_E) != settings.end())
		_MapExtra.State = settings[PW_SERIALIZE_MAPST_E];
}


void CUserSettings::Save() const
{
	map<string, string> settings;

	settings.insert(make_pair(PW_SERIALIZE_MAPSIZE, NumericToString(static_cast<int>(Size))));
	settings.insert(make_pair(PW_SERIALIZE_THEME, NumericToString(static_cast<int>(ThemeId))));
	settings.insert(make_pair(PW_SERIALIZE_SOUND, Sound ? "100" : "0"));
	settings.insert(make_pair(PW_SERIALIZE_WRAPMODE, Wrapping ? "1" : "0"));

	settings.insert(make_pair(PW_SERIALIZE_MAPID_S, NumericToString(static_cast<int>(_MapSmall.Id))));
	settings.insert(make_pair(PW_SERIALIZE_MAPST_S, _MapSmall.State));
	settings.insert(make_pair(PW_SERIALIZE_MAPID_N, NumericToString(static_cast<int>(_MapNormal.Id))));
	settings.insert(make_pair(PW_SERIALIZE_MAPST_N, _MapNormal.State));
	settings.insert(make_pair(PW_SERIALIZE_MAPID_B, NumericToString(static_cast<int>(_MapBig.Id))));
	settings.insert(make_pair(PW_SERIALIZE_MAPST_B, _MapBig.State));
	settings.insert(make_pair(PW_SERIALIZE_MAPID_E, NumericToString(static_cast<int>(_MapExtra.Id))));
	settings.insert(make_pair(PW_SERIALIZE_MAPST_E, _MapExtra.State));

	try {
		CSettings::Save(GetFileName().c_str(), settings, "# This is the user settings file of the PipeWalker game\n# Please, do not edit it manually\n\n");
	}
	catch (CException& /*ex*/) {
	}
}


unsigned long CUserSettings::GetCurrentMapId() const
{
	switch (Size) {
		case MapSizeSmall:	return _MapSmall.Id;
		case MapSizeNormal:	return _MapNormal.Id;
		case MapSizeBig:	return _MapBig.Id;
		case MapSizeExtra:	return _MapExtra.Id;
		default:			assert(false && "Unknown size");
	}
	return 0;
}


void CUserSettings::SetCurrentMapId(const unsigned long id)
{
	switch (Size) {
		case MapSizeSmall:	_MapSmall.Id = id;		break;
		case MapSizeNormal:	_MapNormal.Id = id;	break;
		case MapSizeBig:	_MapBig.Id = id;		break;
		case MapSizeExtra:	_MapExtra.Id = id;		break;
		default:			assert(false && "Unknown size");
	}
}


string CUserSettings::GetCurrentMapState() const
{
	switch (Size) {
		case MapSizeSmall:	return _MapSmall.State;
		case MapSizeNormal:	return _MapNormal.State;
		case MapSizeBig:	return _MapBig.State;
		case MapSizeExtra:	return _MapExtra.State;
		default:			assert(false && "Unknown size");
	}
	return string();
}


void CUserSettings::SetCurrentMapState(const string& state)
{
	switch (Size) {
		case MapSizeSmall:	_MapSmall.State = state;	break;
		case MapSizeNormal:	_MapNormal.State = state;	break;
		case MapSizeBig:	_MapBig.State = state;		break;
		case MapSizeExtra:	_MapExtra.State = state;	break;
		default:			assert(false && "Unknown size");
	}
}


string CUserSettings::GetFileName() const
{
	string loadFileName = _UsePortable ? "." : getenv(
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

	loadFileName += PW_USRSETTINGS_FILENAME;
	return loadFileName;
}
