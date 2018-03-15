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


/**
 * Exception wrapper
 */
class CException : public exception
{
public:
	/**
	 * Constructor
	 * \param descr exception description
	 */
	CException(const char* descr) throw()
	{
		try {
			_WhatException = new char[strlen(descr) + 1];
			strcpy(_WhatException, descr);
		}
		catch (...)	{
			if (_WhatException)
				delete[] _WhatException;
			_WhatException = NULL;
		}
	}

	//Destructor
	~CException() throw()
	{
		if (_WhatException)
			delete[] _WhatException;
	}

	//From exception
	const char* what() const throw()
	{
		return _WhatException ? _WhatException : "Unknown error";
	}

private:
	char* _WhatException;	///< Exception description
};
