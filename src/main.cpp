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

#include "common.h"
#include "synchro.h"
#include "sound.h"
#include "game.h"
#include "texture.h"
#include "rendertext.h"

#if defined PW_USE_SDL		//SDL library
	#include "winmgr_sdl.h"
#elif defined PW_USE_WIN	//Microsoft Windows
	#include "winmgr_win.h"
#else
	#error Undefined window manager (PW_USE_SDL or PW_USE_WIN must be defined)
#endif


#ifdef WIN32
/****************************************************************/
/*                   MS Windows entry point                     */
/****************************************************************/
int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#ifndef NDEBUG
	//Create console for debug session
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
#endif
#else
/****************************************************************/
/*                       ANSI C entry point                     */
/****************************************************************/
int main(int /*argc*/, char** /*argv*/)
{
#endif
	//Program result status
	int resultStatus = EXIT_SUCCESS;

	CGame game;

#if defined PW_USE_SDL		//SDL library
	CWinManagerSDL winMgr(game);
#elif defined PW_USE_WIN	//Microsoft Windows
	CWinManagerWin winMgr(game);
#else
	#error Undefined window manager (PW_USE_SDL or PW_USE_WIN must be defined)
#endif

	try {
		winMgr.CreateGLWindow(PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT);
		winMgr.InitializeOpenGL(PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT);

		CSoundBank::Load();
		CSynchro::Start();
		CTextureBank::Load(game.Settings().Theme);
		CRenderText::Load();

		game.Initialize(winMgr);
		winMgr.MainLoop();
		game.Finalize();
	}
	catch (const CException& ex) {
		winMgr.ShowError(ex.what());
		resultStatus = EXIT_FAILURE;
	}
	catch (exception& ex) {
		winMgr.ShowError(ex.what());
		resultStatus = EXIT_FAILURE;
	}
	catch (...) {
		winMgr.ShowError("Unknown fatal error");
		resultStatus = EXIT_FAILURE;
	}

	CRenderText::Free();
	CTextureBank::Free();
	CSoundBank::Free();

	winMgr.OnApplicationExit();
	return resultStatus;
}
