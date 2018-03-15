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
#include <math.h>
#include <cassert>
#include <time.h>
#include <string>

#if defined PWTARGET_WINNT
	//Microsoft Windows NT
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "glu32.lib")
#elif defined PWTARGET_XSERVER
	//Linux/Unix using X server
	#include <sys/time.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>
	#include <X11/xpm.h>
	#include <GL/glx.h>
#else
	#error Undefined target: Please define PWTARGET_WINNT or PWTARGET_XSERVER
#endif

//OpenGL and GLu library
#include <GL/gl.h>
#include <GL/glu.h>

//Helper macros (32 bit only!)
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

//Map properties
#define MAP_WIDTH_NUM	10		///< Cells by horizontal
#define MAP_HEIGHT_NUM	10		///< Cells by vertical
#define MAP_RCV_NUM		30		///< Receivers number

#define MAX_WEIGHT		0xFFFFF	///< Maximum weight of cell

//Screen (main window) properties
#define SCREEN_WIDTH	450
#define SCREEN_HEIGHT	550

//Main window title
#define WINDOW_TITLE	"PipeWalker ver.0.3.1"


#ifdef PWTARGET_XSERVER
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
 * @param nMin minimum value
 * @return random number from range [min, max]
 */
inline unsigned int GetRandom(const unsigned int nMax, const unsigned int nMin = 0)
{
	return abs(static_cast<int>(static_cast<double>(rand()) / (RAND_MAX + 1) * (nMax - nMin) + nMin));
}
