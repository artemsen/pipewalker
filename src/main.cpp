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

#include "common.h"
#include "game.h"
#include "image.h"
#include "settings.h"
#include "../extra/pipewalker.xpm"

#ifdef WIN32
#include <SDL/SDL_syswm.h>

//! SDL window procedure
WNDPROC sdl_wnd_proc = NULL;

//! Own WNDPROC
LRESULT pw_win32_wnd_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param);
#endif // WIN32


/**
 * Parse command line parameters
 * \param argc number of parameters
 * \param argv parameters array
 * \param lvl_id start level id
 * \param lvl_sz start level map size
 * \param lvl_wrap start level wrap mode
 * \return true if we need to close application
 */
bool parse_cmd_params(int argc, char* argv[], unsigned long& lvl_id, level::size& lvl_sz, bool& lvl_wrap);


/**
 * SDL timer callback
 */
Uint32 timer_callback(Uint32 interval);


/**
 * ANSI main entry point
 */
int main(int argc, char* argv[])
{
	unsigned long lvl_id;
	level::size lvl_sz;
	bool lvl_wrap;
	if (parse_cmd_params(argc, argv, lvl_id, lvl_sz, lvl_wrap))
		return 0;	//Help or version was shown

	//Initialization
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Critical error\nSDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	//Let's get some video information
	const SDL_VideoInfo* vinfo = SDL_GetVideoInfo();
	if (!vinfo) {
		fprintf(stderr, "Critical error\nUnable to get video information: %s\n", SDL_GetError());
		return 1;
	}

	//Save desktop size
	const int desktop_width = vinfo->current_w;
	const int desktop_height = vinfo->current_h;

	//Calculate minimum window sizes
	const int wnd_min_width =  PW_SCREEN_WIDTH  / 3;
	const int wnd_min_height = PW_SCREEN_HEIGHT / 3;

	//Create window
	if (!SDL_SetVideoMode(PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT, 0, SDL_OPENGL | SDL_RESIZABLE)) {
		fprintf(stderr, "Critical error\nUnable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	SDL_WM_SetCaption(PACKAGE_NAME, PACKAGE_NAME);
	image wnd_icon;
	if (wnd_icon.load_XPM(pipewalker_xpm, sizeof(pipewalker_xpm) / sizeof(pipewalker_xpm[0])))
		SDL_WM_SetIcon(wnd_icon.get_surface(), NULL);

	game& game_instance = game::instance();
	if (!game_instance.initialize(lvl_id, lvl_sz, lvl_wrap))
		return 1;

	//Timer - about 25 fps
	SDL_SetTimer(40, &timer_callback);

#ifdef WIN32
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (SDL_GetWMInfo(&wmi)) {
		//Set own window procedure
		sdl_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(wmi.window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&pw_win32_wnd_proc)));
		//Set normal icon
		static HICON icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(0));
		if (icon)
			SetClassLongPtr(wmi.window, GCL_HICON, reinterpret_cast<LONG>(icon));
	}
#endif // WIN32

	bool done = false;
	Uint8 last_mouse_state = 0;

	while (!done) {
		SDL_Event event;
		if (SDL_WaitEvent(&event) == 0) {
			fprintf(stderr, "Critical error\nSDL_WaitEvent failed: %s\n", SDL_GetError());
			return 1;
		}
		switch (event.type) {
			case SDL_MOUSEMOTION:
				{
					Sint32 x, y;
					SDL_GetMouseState(&x, &y);
					game_instance.on_mouse_move(x, y);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				//We need to save buttons state - in the SDL_MOUSEBUTTONUP event doesn't have this information
				last_mouse_state = SDL_GetMouseState(NULL, NULL);
				break;
			case SDL_MOUSEBUTTONUP:
				if (last_mouse_state) {
					game_instance.on_mouse_click(last_mouse_state);
					last_mouse_state = 0;
				}
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_F4 && (SDL_GetModState() == KMOD_LALT || SDL_GetModState() == KMOD_RALT))
					done = true;	//Alt+F4 pressed
				else
					done = game_instance.on_key_press(event.key.keysym.sym);
				break;
			case SDL_QUIT:
				done = true;
				break;
			case SDL_VIDEOEXPOSE:
				game_instance.draw_scene();
				break;
			case SDL_VIDEORESIZE:
				if (event.resize.w && event.resize.h) {
					int wnd_width = event.resize.w;
					int wnd_height = event.resize.h;

					//Set correct aspect ratio
					if (wnd_width != desktop_width && wnd_height != desktop_height) {
						if (wnd_height != vinfo->current_h)
							wnd_width = static_cast<int>(static_cast<float>(wnd_height) / PW_ASPECT_RATIO);
						else if (wnd_width != vinfo->current_w)
							wnd_height = static_cast<int>(static_cast<float>(wnd_width) * PW_ASPECT_RATIO);
						if (wnd_width < wnd_min_width || wnd_height < wnd_min_height) {
							//Set minimum window size
							wnd_width = wnd_min_width;
							wnd_height = wnd_min_height;
						}
					}

					SDL_SetVideoMode(wnd_width, wnd_height, 0, SDL_OPENGL | SDL_RESIZABLE);
					game_instance.on_window_resize(wnd_width, wnd_height);
					SDL_Event expose_event;
					expose_event.type = SDL_VIDEOEXPOSE;
					SDL_PushEvent(&expose_event);
				}
				break;
		}
	}

	game_instance.finalize();

	SDL_Quit();
	return 0;
}


#ifdef WIN32
/**
 * MS Windows entry point (See MSDN for more information)
 */
int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#ifndef NDEBUG
	//Create console for debug session
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
#endif	//NDEBUG

	const int rc = main(__argc, __argv);

#ifndef NDEBUG
	FreeConsole();
#endif	//NDEBUG

	return rc;
}
#endif // WIN32


bool parse_cmd_params(int argc, char* argv[], unsigned long& lvl_id, level::size& lvl_sz, bool& lvl_wrap)
{
	bool app_exit = false;

	lvl_id = 0;
	lvl_sz = level::sz_normal;
	lvl_wrap = true;

	for (int i = 1 /* skip executable file name */; !app_exit && i < argc; ++i) {
		const char* param_val = argv[i];
		while (*param_val && (*param_val == '-' || *param_val == '/'))
			++param_val;
		if (strcmp(param_val, "help") == 0 || strcmp(param_val, "?") == 0) {
			const char* msg =
				"Command line parameters:\n"
				" -help:    this help\n"
				" -version: show information about version\n"
				" -id:      start level with specified id (number from 1 to 99999999)\n"
				" -wrap:    set wrap mode (0: disable, 1: enable (default))\n"
				" -size:    set level map size (small, normal (default), big or extra)\n"
				"\nUsage example:\n"
				" pipewalker -id=1234 -wrap=0 -size=big\n";
#ifdef WIN32
			MessageBoxA(NULL, msg, "PipeWalker " PACKAGE_VERSION, MB_OK | MB_ICONINFORMATION);
#else
			printf("PipeWalker version " PACKAGE_VERSION "\n");
			printf(msg);
#endif // WIN32
			app_exit = true;
		}
		else if (strcmp(param_val, "version") == 0) {
#ifdef WIN32
			MessageBoxA(NULL, "PipeWalker version " PACKAGE_VERSION, "PipeWalker " PACKAGE_VERSION, MB_OK | MB_ICONINFORMATION);
#else
			printf("PipeWalker version " PACKAGE_VERSION "\n");
#endif // WIN32
			app_exit = true;
		}
		else if (strcmp(param_val, "debug") == 0)
			settings::debug_mode(true);
		else if (strncmp(param_val, "id=", sizeof("id=") - 1) == 0) {
			lvl_id = static_cast<unsigned long>(atoi(param_val + sizeof("id=") - 1));
			if (lvl_id > PW_MAX_LEVEL_NUMBER)
				lvl_id = 0;
		}
		else if (strncmp(param_val, "wrap=", sizeof("wrap=") - 1) == 0)
			lvl_wrap = *(param_val + sizeof("id=") - 1) && atoi(param_val + sizeof("id=") - 1) != 0;
		else if (strncmp(param_val, "size=", sizeof("size=") - 1) == 0) {
			const char* level_size = param_val + sizeof("size=") - 1;
			if (strcmp(level_size, "small") == 0)
				lvl_sz = level::sz_small;
			else if (strcmp(level_size, "normal") == 0)
				lvl_sz = level::sz_normal;
			else if (strcmp(level_size, "big") == 0)
				lvl_sz = level::sz_big;
			else if (strcmp(level_size, "extra") == 0)
				lvl_sz = level::sz_extra;
		}
	}

	return app_exit;
}


Uint32 timer_callback(Uint32 interval)
{
	if (game::instance().need_redisplay()) {
		//draw_scene();
		SDL_Event expose_event;
		expose_event.type = SDL_VIDEOEXPOSE;
		SDL_PushEvent(&expose_event);
	}
	return interval;
}


#ifdef WIN32
LRESULT pw_win32_wnd_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
	assert(sdl_wnd_proc);

	if (msg == WM_GETMINMAXINFO) {
		static LONG max_width = 0;
		static LONG max_height = 0;
		if (!max_height) {
			max_height = GetSystemMetrics(SM_CYMAXIMIZED);
			//Calculate correct aspect ratio
			RECT aspect;
			aspect.top = aspect.left = 0;
			aspect.right = PW_SCREEN_WIDTH;
			aspect.bottom = PW_SCREEN_HEIGHT;
			AdjustWindowRectEx(&aspect, GetWindowLongPtr(wnd, GWL_STYLE), FALSE, GetWindowLongPtr(wnd, GWL_EXSTYLE));
			const float need_aspect = static_cast<float>(aspect.bottom - aspect.top) / static_cast<float>(aspect.right - aspect.left);
			max_width = static_cast<LONG>(static_cast<float>(max_height) / need_aspect);
		}

		LPMINMAXINFO mmi = reinterpret_cast<LPMINMAXINFO>(l_param);
		mmi->ptMaxSize.x = mmi->ptMaxTrackSize.x = max_width;
		mmi->ptMaxSize.y = mmi->ptMaxTrackSize.y = max_height;
		mmi->ptMinTrackSize.x = PW_SCREEN_WIDTH / 3;
		mmi->ptMinTrackSize.y = PW_SCREEN_HEIGHT / 3;
		return 0;
	}

	return CallWindowProc(sdl_wnd_proc, wnd, msg, w_param, l_param);
}
#endif // WIN32
