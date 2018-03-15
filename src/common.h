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

#pragma once

//Standard C libraries
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <cassert>
#include <exception>
#include <string>
#include <map>
#include <vector>
#include <list>


using namespace std;

#ifndef PACKAGE_STRING
	#define PACKAGE_STRING		"PipeWalker 0.5.1"
#endif	//PACKAGE_STRING

#ifndef DIR_GAMEDATA
	#define DIR_GAMEDATA		"./data/"
#endif	//DIR_GAMEDATA

#ifndef DIR_TEXTURES
	#define DIR_TEXTURES		DIR_GAMEDATA "textures/"
#endif	//DIR_TEXTURES


#define SCREEN_HEIGHT	500		///< Initial screen (main window) height
#define SCREEN_WIDTH	420		///< Initial screen (main window) width


#if defined WIN32		//Microsoft Windows NT
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif	//WIN32

#if defined WIN32 && !defined NDEBUG		//Microsoft Windows NT (MS VS debug session): make release in MinGW32!
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "glu32.lib")
	#pragma comment(lib, "SDL.lib")
	#pragma comment(lib, "SDLmain.lib")
#endif


//SDL library
#include <SDL/SDL.h>
//OpenGL library
#include <GL/gl.h>
#include <GL/glu.h>

#include "exception.h"


/**
 * Check for OpenGL error
 * \return true if errors exist
 */
inline bool CheckGLError()
{
	bool res = true;
	GLenum glErr;
	while ((glErr = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error: 0x%x: %s", glErr, gluErrorString(glErr));
		res = false;
	}
	return res;
}
