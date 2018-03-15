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

#pragma once

//Standard libraries
#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <fstream>
#include <complex>
#include <algorithm>

using namespace std;

#ifdef WIN32
#include <windows.h>
#endif	//WIN32

#include <SDL/SDL.h>


#define PW_SCREEN_WIDTH		520	///< Initial screen (main window) width
#define PW_SCREEN_HEIGHT	620	///< Initial screen (main window) height
