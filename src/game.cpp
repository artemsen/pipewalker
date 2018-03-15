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

#include "game.h"
#include "render.h"
#include "settings.h"
#include "sound.h"
#include "mtrandom.h"
#ifndef WIN32
#include <dirent.h>
#endif //WIN32

#define PW_SWAP_MODE_SPEED     500
#define PW_THEME_FILE_EXT      ".png"


game::game()
:	_need_redisplay(true),
	_wnd_width(PW_SCREEN_WIDTH), _wnd_height(PW_SCREEN_HEIGHT),
	_mouse_x(0.0f), _mouse_y(0.0f),
	_curr_mode(NULL), _next_mode(NULL),
	_trans_stime(0)
{
}


game& game::instance()
{
	static game i;
	return i;
}


bool game::initialize(const unsigned long lvl_id, const level::size lvl_sz, const bool lvl_wrap)
{
	settings::load();
	if (lvl_id)
		settings::set_state(lvl_id, lvl_sz, lvl_wrap, "Dummy");

	//Initialize render subsystem
	render& renderer = render::instance();
	renderer.initialize();

	//Load texture image
	string file_name = PW_GAMEDATADIR;
	file_name += settings::theme();
	file_name += PW_THEME_FILE_EXT;
	if (!renderer.load(file_name.c_str())) {
		//Try to load first available theme
		if (!load_next_theme(true)) {
			fprintf(stderr, "Critical error\nNo one available themes found\n");
			return false;
		}
	}

	//Initialize randomizer
	mtrandom::seed(0xabcdef);

	//Initialize sound subsystem
	sound::instance().initialize();

	//Initialize modes
	_mode_puzzle.initialize();
	_mode_sett.initialize(_mode_puzzle.current_level_size(), _mode_puzzle.current_wrap_mode(),
		settings::rndlvl_mode(), settings::sound_mode());
	_curr_mode = &_mode_puzzle;

	return true;
}


void game::finalize()
{
	_mode_puzzle.save_current_level();
	settings::save();
}


void game::draw_scene()
{
	_need_redisplay = false;
	render& renderer = render::instance();

	renderer.draw_begin();

	float trn_step = 1.0f;
	if (_trans_stime != 0) {
		assert(_next_mode);
		_need_redisplay = true;
		const unsigned int diff_time = SDL_GetTicks() - _trans_stime;
		if (diff_time < PW_SWAP_MODE_SPEED) {
			//Transition in progress
			trn_step = static_cast<float>(diff_time) / static_cast<float>(PW_SWAP_MODE_SPEED);
			_next_mode->draw(trn_step);
			trn_step = 1.0f - trn_step;	//Invert
		}
		else {
			//Transition complete
			_trans_stime = 0;
			_curr_mode = _next_mode;
			_next_mode = NULL;
		}
	}

	_need_redisplay |= _curr_mode->draw(trn_step);

	renderer.draw_end();
}


void game::on_mouse_move(const int x, const int y)
{
	//Calculate new mouse world coordinates
	assert(x >= 0 && x <= _wnd_width && y >= 0 && y <= _wnd_height);
	_mouse_x = static_cast<float>(x) * (PW_BASE_WIDTH / static_cast<float>(_wnd_width)) - PW_BASE_WIDTH / 2.0f;
	_mouse_y = PW_ASPECT_RATIO * (PW_BASE_WIDTH / 2.0f - static_cast<float>(y) * (PW_BASE_WIDTH / static_cast<float>(_wnd_height)));

#ifndef NDEBUG
	printf("Mouse motion: %03i %03i on %.02f %.02f\n", x, y, _mouse_x, _mouse_y);
#endif //NDEBUG

	_need_redisplay |= _curr_mode->on_mouse_move(_mouse_x, _mouse_y);
}


void game::on_mouse_click(const Uint8 btn)
{
#ifndef NDEBUG
	printf("Mouse button click: %i on %.02f %.02f\n", btn, _mouse_x, _mouse_y);
#endif //NDEBUG

	if (_trans_stime != 0)
		return;	//No action in time of transition between modes

	_need_redisplay = true;
	if (_curr_mode->on_mouse_click(_mouse_x, _mouse_y, btn))
		swap_mode();
}


bool game::on_key_press(const SDLKey key)
{
#ifndef NDEBUG
	printf("Pressed key: %02i (%s)\n", key, SDL_GetKeyName(key));
#endif //NDEBUG

	if (_trans_stime != 0)
		return false;	//No action in time of transition between modes
	if (key == SDLK_ESCAPE) {
		if (_curr_mode == &_mode_sett)
			swap_mode();
		else
			return true;
	}
	return false;
}


void game::on_window_resize(const int width, const int height)
{
	_wnd_width = width;
	_wnd_height = height;
	render::instance().on_window_resize(_wnd_width, _wnd_height);
}


bool game::load_next_theme(const bool direction)
{
	vector<string> themes;

#ifdef WIN32
	string find_path = PW_GAMEDATADIR;
	find_path += "*" PW_THEME_FILE_EXT;
	WIN32_FIND_DATAA find_data;
	HANDLE find_handle = FindFirstFileA(find_path.c_str(), &find_data);
	if (find_handle != INVALID_HANDLE_VALUE) {
		do {
			if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				themes.push_back(find_data.cFileName);
		}
		while (FindNextFileA(find_handle, &find_data));
		FindClose(find_handle);
	}

#else
	DIR* dir = opendir(PW_GAMEDATADIR);
	if (dir) {
		dirent* ent = NULL;
		while ((ent = readdir(dir))) {
			if (strstr(ent->d_name, PW_THEME_FILE_EXT))
				themes.push_back(ent->d_name);
		}
		closedir(dir);
	}
#endif //WIN32

	const size_t themes_sz = themes.size();
	if (themes_sz == 0)
		return false;	//Not themes found?

	//Find current position
	const string curr_theme_file = string(settings::theme()) + PW_THEME_FILE_EXT;
	int curr_theme_id = -1;
	for (size_t i = 0; curr_theme_id  < 0 && i < themes_sz; ++i) {
		if (themes[i].compare(curr_theme_file) == 0)
			curr_theme_id = static_cast<int>(i);
	}

	//Try to load next available theme
	int try_count = static_cast<int>(themes_sz);
	while (--try_count >= 0) {
		string new_theme_file = PW_GAMEDATADIR;
		if (curr_theme_id < 0)
			new_theme_file += themes.front();	//Current theme not found
		else {
			curr_theme_id += (direction ? 1 : -1);
			if (curr_theme_id < 0)
				curr_theme_id = static_cast<int>(themes_sz - 1);
			else if (curr_theme_id >= static_cast<int>(themes_sz))
				curr_theme_id = 0;
			new_theme_file += themes[curr_theme_id];
		}
		if (render::instance().load(new_theme_file.c_str())) {
			string theme_name = new_theme_file;
			theme_name.erase(0, theme_name.find_last_of("\\/") + 1); //Erase path
			theme_name.erase(theme_name.length() - 4);               //Erase file extension
			settings::theme(theme_name.c_str());
			break;
		}
	}

	return try_count >= 0;
}


void game::swap_mode()
{
	assert(!_next_mode);

	if (_curr_mode == &_mode_sett) {
		//May be we need change size/mode
		_mode_puzzle.on_settings_changed(_mode_sett.level_size(), _mode_sett.wrap_mode());
		settings::sound_mode(_mode_sett.sound_mode());
		settings::rndlvl_mode(_mode_sett.random_mode());
		_next_mode = &_mode_puzzle;
	}
	else {
		_mode_sett.reset();
		_next_mode = &_mode_sett;
	}

	_need_redisplay = true;
	_trans_stime = SDL_GetTicks();
}
