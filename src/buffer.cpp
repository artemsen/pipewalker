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

#include "buffer.h"
#include <errno.h>


CBuffer::CBuffer()
:	m_Offset(0)
{
}


void CBuffer::Free()
{
	m_Data.clear();
	m_Offset = 0;
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
		rewind(file);

		m_Data.resize(static_cast<size_t>(size));

		if (fread(&m_Data[0], 1, GetSize(), file) != static_cast<size_t>(size))
			throw errno;
	}
	catch(const int& ex) {
		if (file)
			fclose(file);
		throw CException("Unable to load file ", fileName, ": ", strerror(ex));
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

		if (fwrite(&m_Data[0], 1, m_Data.size(), file) != m_Data.size())
			throw errno;
	}
	catch(const int& ex) {
		if (file)
			fclose(file);
		throw CException("Unable to save file ", fileName, ": ", strerror(ex));
	}

	if (file)
		fclose(file);
}


bool CBuffer::SetOffset(const size_t offset)
{
	const bool result = (offset < m_Data.size());
	if (result)
		m_Offset = offset;
	return result;
}


void CBuffer::PutData(const void* data, const size_t size)
{
	if (size != 0) {
		assert(data);

		m_Data.resize(m_Data.size() + size);
		memcpy(&m_Data[m_Offset], data, size);
		m_Offset += size;
	}
}


unsigned char* CBuffer::GetData(const size_t size)
{
	if (size + m_Offset > m_Data.size())
		return NULL;
	unsigned char* ptr = &m_Data[m_Offset];
	m_Offset += size;
	return ptr;
}
