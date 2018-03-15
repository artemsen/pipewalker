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
#include "../extra/pipewalker.xpm"
#ifdef WIN32
#include <SDL/SDL_syswm.h>
#endif // WIN32

//! Debug mode flag
bool DEBUG_MODE = false;


/**
 * SDL timer callback
 */
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


/**
 * ANSI main entry point
 */
int main(int argc, char* argv[])
{
	if (argc > 0 && strcmp(argv[argc - 1], "--version") == 0) {
		printf("%s version %s\n", PACKAGE_NAME, PACKAGE_VERSION);
		return 0;
	}
	if (argc > 0 && strcmp(argv[argc - 1], "--debug") == 0)
		DEBUG_MODE = true;

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
	if (!game_instance.initialize())
		return 1;

	//Timer - about 25 fps
	SDL_SetTimer(40, &timer_callback);

#ifdef WIN32
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (SDL_GetWMInfo(&wmi)) {
		//Disable 'maximize' button
		LONG_PTR ws = GetWindowLongPtr(wmi.window, GWL_STYLE);
		ws ^= WS_MAXIMIZEBOX;
		SetWindowLongPtr(wmi.window, GWL_STYLE, ws);
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
				game_instance.on_mouse_click(last_mouse_state);
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

					if (wnd_width != desktop_width && wnd_height != desktop_height) {
						//Set correct aspect ratio
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
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
#endif	//NDEBUG

	return main(__argc, __argv);
}
#endif // WIN32
