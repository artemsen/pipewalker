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


class CException
{
public:
	CException(const char* descr)	: exeptionDescr(descr) {}
	CException(const string& descr)	: exeptionDescr(descr) {}
	CException(const char* descr1, const char* descr2)	: exeptionDescr(string(descr1) + string(descr2)) {}
	CException(const char* descr1, const char* descr2, const char* descr3)	: exeptionDescr(string(descr1) + string(descr2) + string(descr3)) {}
	CException(const char* descr1, const char* descr2, const char* descr3, const char* descr4)	: exeptionDescr(string(descr1) + string(descr2) + string(descr3) + string(descr4)) {}

	const char* what() const { return exeptionDescr.c_str(); }

private:
	string exeptionDescr;
};
