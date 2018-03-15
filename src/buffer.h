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

#include "common.h"

/**
 * CBuffer - work with memory buffer, load and save buffer to files
 */
class CBuffer
{
public:
	//! Default constructor
	CBuffer();

	/**
	 * Create buffer
	 * \param size size of the buffer
	 */
	void Create(const size_t size);

	/**
	 * Free buffer
	 */
	void Free();

	/**
	 * Load file to the buffer
	 * \param fileName input file name
	 */
	void Load(const char* fileName);

	/**
	 * Save buffer to file
	 * \param fileName output file name
	 */
	void Save(const char* fileName);

	/**
	 * Copy buffer
	 * \param copyBuf destination buffer
	 */
	void Copy(vector<unsigned char>& copyBuf) const	{ copyBuf = _Data; }

	/**
	 * Get end-of-buffer (EOB) flag
	 * \return EOB flag
	 */
	inline bool EOB() const			{ return _Offset >= _Data.size(); }

	/**
	 * Get size of the buffer
	 * \return buffer length in bytes
	 */
	inline size_t GetSize() const	{ return _Data.size(); }

	/**
	 * Get offset
	 * \return offset
	 */
	inline size_t GetOffset() const	{ return _Offset; }

	/**
	 * Set offset (current position) in buffer
	 * \param offset new offset (current position)
	 * \return false if error
	 */
	bool SetOffset(const size_t offset);

	/**
	 * Put data to the buffer at current position
	 * \param data source data pointer
	 * \param size size of the data
	 */
	void PutData(const void* data, const size_t size);

	/**
	 * Get data pointer at current position
	 * \param size data length
	 * \return data pointer (NULL if error)
	 */
	unsigned char* GetData(const size_t size);

	/**
	 * Get single string
	 * \return single string
	 */
	string GetString();

	/**
	 * Get data at current position
	 * \param val data output variable
	 * \return false if error
	 */
	template<class T> bool Get(T& val)
	{
		unsigned char* buf = GetData(sizeof(T));
		if (buf)
			val = *(reinterpret_cast<T*>(buf));
		return buf != NULL;
	}

	/**
	 * Put data to current position
	 * \param val data output variable
	 */
	template<class T> void Put(const T& val)
	{
		PutData(&val, sizeof(T));
	}

	/**
	 * Put data to current position
	 * \param val data output variable
	 */
	void Put(const string& val)
	{
		PutData(val.c_str(), val.size());
	}

private:	//Class variables
	vector<unsigned char>	_Data;		///< Data pointer
	size_t					_Offset;	///< Current position (offset) at the buffer
};
