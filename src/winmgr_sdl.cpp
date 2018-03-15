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

#ifdef PW_USE_SDL

#include "winmgr_sdl.h"
#include "texture.h"
#include "rendertext.h"
#include "game.h"
#include "image.h"
#include "../extra/pipewalker.xpm"

//SDL library
#include <SDL/SDL.h>

#ifdef WIN32
	#include <SDL/SDL_syswm.h>
#endif // WIN32


void CWinManagerSDL::PostRedisplay()
{
	const int exposeEventNum = SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, SDL_VIDEOEXPOSEMASK);
	if (exposeEventNum <= 0) {
		static Uint32 lastRedraw = SDL_GetTicks();
		const Uint32 currTick = SDL_GetTicks();
		if (currTick - lastRedraw < 30)
			SDL_Delay(30 - (currTick - lastRedraw));
		lastRedraw = SDL_GetTicks();

		SDL_Event event;
		event.type = SDL_VIDEOEXPOSE;
		SDL_PushEvent(&event);
	}
}


void CWinManagerSDL::MainLoop()
{
	while (!m_ExitProgram) {
		SDL_Event event;
		if (SDL_WaitEvent(&event) == 0)
			throw CException("SDL_WaitEvent failed: ", SDL_GetError());

		switch (event.type) {

			case SDL_MOUSEBUTTONDOWN:
				{
					Sint32 mouseX, mouseY;
					const Uint8 state = SDL_GetMouseState(&mouseX, &mouseY);

					MouseButton btn = MouseButton_None;
					if (state & SDL_BUTTON(SDL_BUTTON_LEFT))
						btn = MouseButton_Left;
					else if (state & SDL_BUTTON(SDL_BUTTON_RIGHT))
						btn = MouseButton_Right;
					else if (state & SDL_BUTTON(SDL_BUTTON_MIDDLE))
						btn = MouseButton_Middle;
					else if (state & SDL_BUTTON(SDL_BUTTON_WHEELUP))
						btn = MouseButton_WheelUp;
					else if (state & SDL_BUTTON(SDL_BUTTON_WHEELDOWN))
						btn = MouseButton_WheelDown;

					OnMouseButtonDown(mouseX, mouseY, btn);
				}
				break;

			case SDL_KEYDOWN:
				OnKeyboardKeyDown(static_cast<const char>(event.key.keysym.sym));
				break;

			case SDL_MOUSEMOTION:
				{
					Sint32 mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					OnMouseMove(mouseX, mouseY);
				}
				break;

			case SDL_VIDEOEXPOSE:
				OnRenderScene();
				break;

			case SDL_VIDEORESIZE:
				CreateGLWindow(event.resize.w, event.resize.h);
				InitializeOpenGL(event.resize.w, event.resize.h);

#ifdef WIN32
				//We need to reinitialize texture on resizing window (SDL specific)
				CTextureBank::Load(ThemeNetwork);
				CRenderText::Load();
#endif // WIN32

				OnRenderScene();
				break;

			case SDL_QUIT:
				m_ExitProgram = true;
				break;

			default:
				break;
		}
	}
}


void CWinManagerSDL::CreateGLWindow(const int width, const int height)
{
	//SDL initialization
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
		throw CException("SDL initialization failed: ", SDL_GetError());

	SDL_WM_SetCaption(PACKAGE_STRING, PACKAGE_STRING);

	static SDL_Surface* srfIcon = NULL;
	if (!srfIcon) {
		static CImage wndIcon;
		wndIcon.LoadXPM(pipewalker_xpm, sizeof(pipewalker_xpm) / sizeof(pipewalker_xpm[0]));

		static const Uint32 mask[4] = {
#ifdef PW_BYTEORDER_BIG_ENDIAN
			0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif	//PW_BYTEORDER_BIG_ENDIAN
		};
		srfIcon = SDL_CreateRGBSurfaceFrom(wndIcon.GetData(), wndIcon.GetWidth(), wndIcon.GetHeight(), wndIcon.BytesPerPixel() * 8, wndIcon.GetWidth() * wndIcon.BytesPerPixel(), mask[0], mask[1], mask[2], mask[3]);
	}
	SDL_WM_SetIcon(srfIcon, NULL);

	//GL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Let's get some video information
	static const SDL_VideoInfo* vinfo = SDL_GetVideoInfo();
	if (!vinfo)
		throw CException("Unable to get video information: ", SDL_GetError());

	//Create window
	SDL_Surface* screen = SDL_SetVideoMode(width, height, vinfo->vfmt->BitsPerPixel, SDL_OPENGL | SDL_RESIZABLE);
	if (!screen)
		throw CException("Unable to set video mode: ", SDL_GetError());
}


void CWinManagerSDL::SwapBuffers() const
{
	SDL_GL_SwapBuffers();
}


void CWinManagerSDL::OnApplicationExit()
{
	SDL_Quit();
}


void CWinManagerSDL::ShowError(const char* err)
{
#ifdef WIN32
	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version);
	::MessageBoxA(SDL_GetWMInfo(&wminfo) == 1 ? wminfo.window : NULL, err, PACKAGE_STRING " Error", MB_ICONERROR | MB_OK);
#else
	fprintf(stderr, "Critical error: ");
	fprintf(stderr, err);
	fprintf(stderr, "\n");
#endif // WIN32
}


#endif	//PW_USE_SDL
