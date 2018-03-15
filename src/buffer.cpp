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

#include "buffer.h"
#include <errno.h>


CBuffer::CBuffer()
:	_Offset(0)
{
}


void CBuffer::Free()
{
	_Data.clear();
	_Offset = 0;
}


void CBuffer::Load(const char* fileName)
{
	assert(fileName);

	Free();

	FILE* file = NULL;

	try {
		file = fopen(fileName, "rb");
		if (file == NULL)
			throw errno;
		if (fseek(file, 0, SEEK_END) != 0)
			throw errno;
		const long size = ftell(file);
		if (size < 0)
			throw errno;
		fseek(file, 0, SEEK_SET);

		_Data.resize(static_cast<size_t>(size));

		if (fread(&_Data[0], 1, GetSize(), file) != static_cast<size_t>(size))
			throw errno;
	}
	catch(const int& ex) {
		if (file)
			fclose(file);
		string errDescr = "Unable to load file ";
		errDescr += fileName;
		errDescr += ": ";
		errDescr += strerror(ex);
		throw CException(errDescr.c_str());
	}

	if (file)
		fclose(file);
}


void CBuffer::Save(const char* fileName)
{
	assert(fileName);

	FILE* file = NULL;

	try {
		file = fopen(fileName, "wb");
		if (file == NULL)
			throw errno;

		if (fwrite(&_Data[0], 1, _Data.size(), file) != _Data.size())
			throw errno;
	}
	catch(const int& ex) {
		if (file)
			fclose(file);
		string errDescr = "Unable to save file ";
		errDescr += fileName;
		errDescr += ": ";
		errDescr += strerror(ex);
		throw CException(errDescr.c_str());
	}

	if (file)
		fclose(file);
}


bool CBuffer::SetOffset(const size_t offset)
{
	const bool result = (offset < _Data.size());
	if (result)
		_Offset = offset;
	return result;
}


void CBuffer::PutData(const void* data, const size_t size)
{
	if (size != 0) {
		assert(data);

		_Data.resize(_Data.size() + size);
		memcpy(&_Data[_Offset], data, size);
		_Offset += size;
	}
}


unsigned char* CBuffer::GetData(const size_t size)
{
	if (size + _Offset > _Data.size())
		return NULL;
	unsigned char* ptr = &_Data[_Offset];
	_Offset += size;
	return ptr;
}


string CBuffer::GetString()
{
	string result;
	char c;
	while (Get(c) && c != '\n')
		result += c;
	return result;
}
