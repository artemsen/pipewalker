/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
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

//Standart C libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cassert>


#if defined PW_SYSTEM_WINNT		//Microsoft Windows NT
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#pragma comment(lib, "opengl32.lib")
	#define PACKAGE_STRING "PipeWalker 0.4.3"
	#define PW_GAMEDATA_DIR "."
#elif defined PW_SYSTEM_NIX		//Linux/Unix using X server
	#include <sys/time.h>
#else
	#error Undefined target: Please define PW_SYSTEM_WINNT or PW_SYSTEM_NIX
#endif

#if defined PW_USE_GLUT && defined PW_SYSTEM_WINNT
	//Using GLUT
	#pragma comment(lib, "glut32.lib")
#endif


//OpenGL library
#include <GL/gl.h>

//Helper macros
#ifndef MAKEWORD
	#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))
#endif
#ifndef MAKELONG
	#define MAKELONG(a, b)      ((long)(((unsigned short)(a)) | ((unsigned long)((unsigned short)(b))) << 16))
#endif
#ifndef LOWORD
	#define LOWORD(l)           ((unsigned short)(l))
#endif
#ifndef HIWORD
	#define HIWORD(l)           ((unsigned short)(((unsigned long)(l) >> 16) & 0xFFFF))
#endif
#ifndef LOBYTE
	#define LOBYTE(w)           ((unsigned char)(w))
#endif
#ifndef HIBYTE
	#define HIBYTE(w)           ((unsigned char)(((unsigned short)(w) >> 8) & 0xFF))
#endif

#define PW_ROTATION_TIME	300		///< Time in ms for rotate operation
#define PW_ROTATION_ANGLE	90.0f		///< Single angle
#define PW_SCREEN_HEIGHT	640		///< Initial screen (main window) height
#define PW_SCREEN_WIDTH		480		///< Initial screen (main window) width
#define PW_WINDOW_TITLE		PACKAGE_STRING	///< Main window title

#ifndef PW_SYSTEM_WINNT
/**
 * Analog of function GetTickCount() by MS Windows for unix systems (used to calculate time for animation)
 * @return milliseconds
 */
inline unsigned long GetTickCount(void)
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif


/**
 * Generate random number
 * @param nMax maximum value
 * @return random number from range [0, max]
 */
inline int GetRandom(const unsigned int nMax)
{
	return abs(static_cast<int>(static_cast<double>(rand()) / (RAND_MAX + 1) * nMax));
}
