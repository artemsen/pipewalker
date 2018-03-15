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

#pragma once

//Standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <exception>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <iterator>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif	//WIN32

using namespace std;

#ifndef PACKAGE_STRING
	#define PACKAGE_STRING	"PipeWalker"
#endif	//PACKAGE_STRING

#ifndef DIR_GAMEDATA
	#define DIR_GAMEDATA	"./data/"
#endif	//DIR_GAMEDATA

#define PW_SCREEN_WIDTH		490	///< Initial screen (main window) width
#define PW_SCREEN_HEIGHT	580	///< Initial screen (main window) height

//OpenGL library
#include <GL/gl.h>

#ifndef __MINGW32__
#pragma warning(disable: 4996)	///The POSIX name for this item is deprecated
#endif	//__MINGW32__

//Byte order
#if defined(__hppa__) || defined(__m68k__) || defined(mc68000) || defined(__M68K) || \
	(defined(__MIPS__) && defined(__MISPEB__)) || \
	defined(__ppc__) || defined(__POWERPC__) || defined(__PPC) || defined(__sparc__)
	#define PW_BYTEORDER_BIG_ENDIAN 1
#else
	#define PW_BYTEORDER_LITTLE_ENDIAN 1
#endif

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
		fprintf(stderr, "OpenGL error: 0x\n", glErr);
		res = false;
	}
	return res;
}
