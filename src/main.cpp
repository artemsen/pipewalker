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


#include "common.h"
#include "synchro.h"
#include "sound.h"
#include "game.h"
#include "texture.h"
#include "settings.h"

#ifdef WIN32
#include "PipeWalkerRes.h"
#endif // WIN32

#define SCREEN_WIDTH	490		///< Initial screen (main window) width
#define SCREEN_HEIGHT	580		///< Initial screen (main window) height


/****************************************************************/
/*                      ANSI C main routine                     */
/****************************************************************/
extern "C" int main(int /*argc*/, char** /*argv*/)
{
	//Program result status
	int resultStatus = EXIT_SUCCESS;

	try {
#if defined WIN32 && !defined NDEBUG
		//Create console for debug session
		AllocConsole();
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);
#endif

		//SDL initialization - step 1
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
			throw CException("Video initialization failed: ", SDL_GetError());
		//SDL initialization - step 2 (audio)
		CSoundBank::Initialize();

		SDL_WM_SetCaption(PACKAGE_STRING, PACKAGE_STRING);
#ifdef WIN32
		HINSTANCE handle = ::GetModuleHandle(NULL);
		HICON icon = ::LoadIcon(handle, MAKEINTRESOURCE(IDI_PIPEWALKER));
		SDL_SysWMinfo wminfo;
		SDL_VERSION(&wminfo.version);
		if (SDL_GetWMInfo(&wminfo) == 1)
			::SetClassLong(wminfo.window, GCL_HICON, reinterpret_cast<LONG>(icon));
#else
		SDL_WM_SetIcon(SDL_LoadBMP(DIR_GAMEDATA "PipeWalker.bmp"), NULL);
#endif // WIN32

		//GL attributes
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		//Let's get some video information
		const SDL_VideoInfo* vinfo = SDL_GetVideoInfo();
		if (!vinfo)
			throw CException("Unable to get video information: ", SDL_GetError());

		//Create window
		SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, vinfo->vfmt->BitsPerPixel, SDL_OPENGL);
		if (!screen)
			throw CException("Unable to set video mode: ", SDL_GetError());

		//Initialize OpenGL subsystem
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDepthFunc(GL_LEQUAL);
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		//We will use only arrays to draw
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Setup perspective
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		static const GLdouble aspect = static_cast<GLdouble>(SCREEN_WIDTH) / static_cast<GLdouble>(SCREEN_HEIGHT);
		gluPerspective(70.0, aspect, 1.0, 100.0);
		gluLookAt(0.0, 0.0, 9.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

#ifndef NDEBUG
		fprintf(stdout, "GL initialized\n");
		fprintf(stdout, "GL renderer: %s\n", glGetString(GL_RENDERER));
		fprintf(stdout, "GL version:  %s\n", glGetString(GL_VERSION));
		fprintf(stdout, "GL vendor:   %s\n", glGetString(GL_VENDOR));
#endif // _NDEBUG

		CSynchro::Start();
		CTextureBank::Initialize();
		CSettings::Load();

		CGame game;
		game.DoMainLoop();

		CTextureBank::Free();
		CSettings::Save();

#ifdef WIN32
		::DestroyIcon(icon);
#endif	//WIN32

	}
	catch (const CException& ex) {
		fprintf(stderr, "Error: %s\n", ex.what());
		resultStatus = EXIT_FAILURE;
	}
	catch (exception& ex) {
		fprintf(stderr, "Unhandled std exception: %s\n", ex.what());
		resultStatus = EXIT_FAILURE;
	}
	catch (...) {
		fprintf(stderr, "Unknown fatal error\n");
		resultStatus = EXIT_FAILURE;
	}

	SDL_Quit();
	return resultStatus;
}
