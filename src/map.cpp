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

#include "map.h"
#include "mtrandom.h"


CMap::CMap()
: _GameOver(false), _WrapMode(true), _SenderX(0), _SenderY(0), _ZeroX(0), _ZeroY(0), _MapSize(10)
{
}


void CMap::New(const MapSize mapSize, const unsigned long mapId, const bool wrapMode)
{
	_WrapMode = wrapMode;
	_MapSize = static_cast<unsigned short>(mapSize);
	_CellMap.resize(_MapSize * _MapSize);

	CMTRandom::Seed(mapId);		//Initialize random sequence

	const unsigned int receiversNum = (_MapSize * _MapSize) / 5;

	bool installDone = false;
	while (!installDone) {
		//Reset map
		for (vector<CCell>::iterator it = _CellMap.begin(); it != _CellMap.end(); ++it)
			it->Reset();

		InstallSender();

		//Install workstations
		installDone = true;
		for (unsigned int i = 0; i < receiversNum && installDone; ++i)
			installDone &= InstallReceiver();
	}
#ifndef NDEBUG
	DefineConnectStatus();
	_GameOver = false;
#else
	ResetByRotate();
	CheckForGameOver();
#endif
}


bool CMap::LoadMap(const MapSize mapSize, const string& descr, const bool wrapMode)
{
	_WrapMode = wrapMode;
	_MapSize = static_cast<unsigned short>(mapSize);
	_CellMap.resize(_MapSize * _MapSize);

	//Reset map
	for (vector<CCell>::iterator it = _CellMap.begin(); it != _CellMap.end(); ++it)
		it->Reset();

	vector<string> load;
	load.reserve(_MapSize * _MapSize);

	size_t start = 0;
	size_t end = 0;
	while ((end = descr.find (',', start)) != string::npos) {
		load.push_back(descr.substr(start, end - start));
		start = end + 1;
	}
	load.push_back(descr.substr(start, end - start));

	if (load.size() != static_cast<size_t>(_MapSize * _MapSize))
		return false;

	for (unsigned short y = 0; y < _MapSize; ++y) {
		for (unsigned short x = 0; x < _MapSize; ++x) {
			CCell& cell = GetCell(x, y);
			if (!cell.Load(load[x + y * _MapSize]))
				return false;
			if (cell.GetCellType() == CCell::CTSender) {
				_SenderX = x;
				_SenderY = y;
			}
		}
	}

	DefineConnectStatus();

	return true;
}


string CMap::SaveMap() const
{
	string res;

	for (unsigned short y = 0; y < _MapSize; ++y) {
		for (unsigned short x = 0; x < _MapSize; ++x) {
			if (x || y)
				res += ',';
			res += GetCell(x, y).Save();
		}
	}

	return res;
}


bool CMap::CheckForGameOver()
{
	_GameOver = true;

	for (unsigned short i = 0; (i < _MapSize * _MapSize) && _GameOver; ++i)
		_GameOver = !_CellMap[i].IsRotationInProgress() && (_CellMap[i].GetCellType() != CCell::CTReceiver || _CellMap[i].IsActive());

	if (_GameOver) {
		//Reset locks
		for (unsigned short i = 0; i < _MapSize * _MapSize; ++i)
			if (_CellMap[i].IsLocked())
				_CellMap[i].ReverseLock();
	}

	return _GameOver;
}


void CMap::ResetByRotate()
{
	_GameOver = false;
	for (unsigned short i = 0; i < _MapSize * _MapSize; ++i) {
		if (_CellMap[i].GetCellType() != CCell::CTFree && !_CellMap[i].IsLocked() && (CMTRandom::Rand() % 10) > 1) {
			_CellMap[i].Rotate((CMTRandom::Rand() % 10) > 5);
			if ((CMTRandom::Rand() % 10) > 5) //Twice rotation
				_CellMap[i].Rotate(true);
		}
	}
	DefineConnectStatus();
}


void CMap::InstallSender()
{
	do {
		_SenderX = CMTRandom::Rand() % _MapSize;
		_SenderY = CMTRandom::Rand() % _MapSize;

	} while (!_WrapMode && (_SenderX == 0 || _SenderX == _MapSize - 1 || _SenderY == 0 || _SenderY == _MapSize - 1));


	CCell& srv = GetCell(_SenderX, _SenderY);
	srv.SetAsSender();

	//Define zero point (sender output cell)
	_ZeroX = _SenderX;
	_ZeroY = _SenderY;
	switch (CMTRandom::Rand() % 4) {
		case 0: _ZeroX = (_ZeroX + 1) % _MapSize;					break;
		case 1: _ZeroX = _ZeroX == 0 ? _MapSize - 1 : _ZeroX - 1;	break;
		case 2: _ZeroY = (_ZeroY + 1) % _MapSize;					break;
		case 3: _ZeroY = _ZeroY == 0 ? _MapSize - 1 : _ZeroY - 1;	break;
		default:
			assert(false);
			break;
	}

	MakeConnection(_SenderX, _SenderY, _ZeroX, _ZeroY);
}


CCell& CMap::GetCell(const unsigned short x, const unsigned short y)
{
	//Check for Outside the map
	assert(x < _MapSize && y < _MapSize);
	return _CellMap[x + y * _MapSize];
}


const CCell& CMap::GetCell(const unsigned short x, const unsigned short y) const
{
	//Check for Outside the map
	assert(x < _MapSize && y < _MapSize);
	return _CellMap[x + y * _MapSize];
}


bool CMap::InstallReceiver()
{
	//Get free cells
	vector< pair<unsigned short, unsigned short> > freeCells;
	for (unsigned short x = 0; x < _MapSize; ++x) {
		for (unsigned short y = 0; y < _MapSize; ++y) {
			CCell& cell = GetCell(x, y);
			if (cell.GetCellType() == CCell::CTFree)
				freeCells.push_back(make_pair(x, y));
			cell.SetUsed(false);

		}
	}
	if (freeCells.empty())
		return true;	//No more free cells

	bool result = false;
	int tryCounter(_MapSize * 2);
	while (tryCounter-- && !result) {

		//Backup current map state
		vector<CCell> backup;
		backup.reserve(_MapSize * _MapSize);
		for (unsigned short i = 0; i < _MapSize * _MapSize; ++i)
			backup.push_back(_CellMap[i]);

		const int freeCellInd = CMTRandom::Rand() % freeCells.size();
		const unsigned short freeX = freeCells[freeCellInd].first;
		const unsigned short freeY = freeCells[freeCellInd].second;
		CCell& rcv = GetCell(freeX, freeY);
		rcv.SetAsReceiver();

		result = MakeRoute(freeX, freeY);

		if (!result) {
			//Restore map
			for (unsigned short i = 0; i < _MapSize * _MapSize; ++i)
				_CellMap[i] = backup[i];
		}
	}

	return result;
}


bool CMap::MakeRoute(const unsigned short x, const unsigned short y)
{
	unsigned short nextX = 0, nextY = 0;	//Next coordinates

	bool result = false;

	int tryCounter(5);
	while (tryCounter && !result) {
		short i, j;
		do {
			i = 1 - (CMTRandom::Rand() % 3);
			j = 1 - (CMTRandom::Rand() % 3);
		} while ((i && j) || (!i && !j));	//Diagonal


		if (!_WrapMode) {
			if ((j < 0 && x == 0) || (j > 0 && x == _MapSize - 1) ||
				(i < 0 && y == 0) || (i > 0 && y == _MapSize - 1)) {
					--tryCounter;
					continue;
			}
		}

		const unsigned short cpX = (j < 0 && x == 0) ? _MapSize - 1 : (x + j) % _MapSize;
		const unsigned short cpY = (i < 0 && y == 0) ? _MapSize - 1 : (y + i) % _MapSize;

		CCell& cell = GetCell(cpX, cpY);
		if (!cell.GetUsed() && cell.CanAddTube()) {
			result = true;
			nextX = cpX;
			nextY = cpY;
		}

		--tryCounter;
	}

	if (!result)
		return false;	//min point - we don't have a route

	MakeConnection(x, y, nextX, nextY);
	GetCell(x, y).SetUsed(true);

	if (GetCell(nextX, nextY).GetTubeType() == CCell::TTJoiner || (nextX == _ZeroX && nextY == _ZeroY))
		return true;

	return MakeRoute(nextX, nextY);
}


void CMap::MakeConnection(const unsigned short currX, const unsigned short currY, const unsigned short nextX, const unsigned short nextY)
{
	assert(currX == nextX || currY == nextY);
	assert(currX != nextX || currY != nextY);

	CCell& cellCurr = GetCell(currX, currY);
	CCell& cellNext = GetCell(nextX, nextY);

	//Wrapping
	if (nextX == 0 && currX == _MapSize - 1) {
		cellCurr.AddTube(CCell::CSRight);
		cellNext.AddTube(CCell::CSLeft);
	}
	else if (nextX == _MapSize - 1 && currX == 0) {
		cellCurr.AddTube(CCell::CSLeft);
		cellNext.AddTube(CCell::CSRight);
	}
	else if (nextY == 0 && currY == _MapSize - 1) {
		cellCurr.AddTube(CCell::CSBottom);
		cellNext.AddTube(CCell::CSTop);
	}
	else if (nextY == _MapSize - 1 && currY == 0) {
		cellCurr.AddTube(CCell::CSTop);
		cellNext.AddTube(CCell::CSBottom);
	}
	//Non-wrapping
	else if (nextX < currX) {
		cellCurr.AddTube(CCell::CSLeft);
		cellNext.AddTube(CCell::CSRight);
	}
	else if (nextX > currX) {
		cellCurr.AddTube(CCell::CSRight);
		cellNext.AddTube(CCell::CSLeft);
	}
	else if (nextY < currY) {
		cellCurr.AddTube(CCell::CSTop);
		cellNext.AddTube(CCell::CSBottom);
	}
	else if (nextY > currY) {
		cellCurr.AddTube(CCell::CSBottom);
		cellNext.AddTube(CCell::CSTop);
	}
}


void CMap::DefineConnectStatus(const unsigned short x, const unsigned short y)
{
	CCell& cellCurr = GetCell(x, y);
	if (cellCurr.IsActive() || cellCurr.IsRotationInProgress())
		return;	//Already connected or rotate in progress

	cellCurr.SetActive(true);

	//to up
	if (cellCurr.IsTopConnected()) {
		if (_WrapMode || y > 0) {
			const unsigned short cpY = y > 0 ? y - 1 : _MapSize - 1;
			CCell& cellNext = GetCell(x, cpY);
			if (cellNext.IsBottomConnected())
				DefineConnectStatus(x, cpY);
		}
	}

	//to down
	if (cellCurr.IsBottomConnected()) {
		if (_WrapMode || y < _MapSize - 1) {
			const unsigned short cpY = (y + 1) % _MapSize;
			CCell& cellNext = GetCell(x, cpY);
			if (cellNext.IsTopConnected())
				DefineConnectStatus(x, cpY);
		}
	}

	//to left
	if (cellCurr.IsLeftConnected()) {
		if (_WrapMode || x > 0) {
			const unsigned short cpX = x > 0 ? x - 1 : _MapSize - 1;
			CCell& cellNext = GetCell(cpX, y);
			if (cellNext.IsRightConnected())
				DefineConnectStatus(cpX, y);
		}
	}

	//to right
	if (cellCurr.IsRightConnected()) {
		if (_WrapMode || x < _MapSize - 1) {
			const unsigned short cpX = (x + 1) % _MapSize;
			CCell& cellNext = GetCell(cpX, y);
			if (cellNext.IsLeftConnected())
				DefineConnectStatus(cpX, y);
		}
	}
}


void CMap::DefineConnectStatus()
{
	//Reset connection status
	for (unsigned short x = 0; x < _MapSize; ++x)
		for (unsigned short y = 0; y < _MapSize; ++y)
			GetCell(x, y).SetActive(false);

	DefineConnectStatus(_SenderX, _SenderY);
	CheckForGameOver();
}
