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

#include "map.h"


CMap::CMap()
: m_GameOver(false), m_WrapMode(true), m_SenderX(0), m_SenderY(0), m_ZeroX(0), m_ZeroY(0), m_MapSize(10)
{
}


void CMap::New(const MapSize mapSize, const unsigned long mapId, const bool wrapMode)
{
	m_WrapMode = wrapMode;
	m_MapSize = static_cast<unsigned short>(mapSize);
	m_CellMap.resize(m_MapSize * m_MapSize);

	srand(mapId);	//Initialize random sequence

	m_GameOver = false;

	const unsigned int receiversNum = (m_MapSize * m_MapSize) / 5;

	bool installDone = false;
	while (!installDone) {
		//Reset map
		for (vector<CCell>::iterator it = m_CellMap.begin(); it != m_CellMap.end(); ++it)
			it->Reset();

		InstallSender();

		//Install workstations
		installDone = true;
		for (unsigned int i = 0; i < receiversNum && installDone; ++i)
			installDone &= InstallReceiver();
	}
#ifndef NDEBUG
	DefineConnectStatus();
	m_GameOver = false;
#else
	ResetByRotate();
#endif
}


bool CMap::LoadMap(const MapSize mapSize, const string& descr, const bool wrapMode)
{
	m_WrapMode = wrapMode;
	m_MapSize = static_cast<unsigned short>(mapSize);
	m_CellMap.resize(m_MapSize * m_MapSize);

	vector<unsigned short> load;
	load.reserve(m_MapSize * m_MapSize);

	string::size_type start = 0;
	string::size_type end = 0;
	unsigned int val = 0;
	while ((end = descr.find (',', start)) != string::npos) {
		if (sscanf(descr.substr(start, end - start).c_str(), "%x", &val) != 1)
			return false;
		load.push_back(static_cast<unsigned short>(val));
		start = end + 1;
	}
	if (sscanf(descr.substr(start, end - start).c_str(), "%x", &val) != 1)
		return false;
	load.push_back(static_cast<unsigned short>(val));

	if (load.size() != static_cast<size_t>(m_MapSize * m_MapSize))
		return false;

	for (unsigned short y = 0; y < m_MapSize; ++y) {
		for (unsigned short x = 0; x < m_MapSize; ++x) {
			CCell& cell = GetCell(x, y);
			if (!cell.Load(load[x + y * m_MapSize]))
				return false;
			if (cell.GetCellType() == CCell::CTSender) {
				m_SenderX = x;
				m_SenderY = y;
			}
		}
	}

	DefineConnectStatus();
	return true;
}


string CMap::SaveMap() const
{
	string res;

	for (unsigned short y = 0; y < m_MapSize; ++y) {
		for (unsigned short x = 0; x < m_MapSize; ++x) {
			if (x || y)
				res += ',';
			char state[5];
			sprintf(state, "%04x", GetCell(x, y).Save());
			res += state;
		}
	}

	return res;
}


bool CMap::CheckForGameOver()
{
	m_GameOver = true;
	for (unsigned short i = 0; (i < m_MapSize * m_MapSize) && m_GameOver; ++i)
		m_GameOver = !m_CellMap[i].IsRotationInProgress() && (m_CellMap[i].GetCellType() != CCell::CTReceiver || m_CellMap[i].IsActive());
	return m_GameOver;
}


void CMap::ResetByRotate()
{
	m_GameOver = false;
	for (unsigned short i = 0; i < m_MapSize * m_MapSize; ++i) {
		if (m_CellMap[i].GetCellType() != CCell::CTFree && !m_CellMap[i].IsLocked() && (rand() % 10) > 1) {
			m_CellMap[i].Rotate((rand() % 10) > 5);
			if ((rand() % 10) > 5) //Twice rotation
				m_CellMap[i].Rotate(true);
		}
	}
	DefineConnectStatus();
}


void CMap::InstallSender()
{
	do {
		m_SenderX = rand() % m_MapSize;
		m_SenderY = rand() % m_MapSize;

	} while (!m_WrapMode && (m_SenderX == 0 || m_SenderX == m_MapSize - 1 || m_SenderY == 0 || m_SenderY == m_MapSize - 1));


	CCell& srv = GetCell(m_SenderX, m_SenderY);
	srv.SetAsSender();

	//Define zero point (sender output cell)
	m_ZeroX = m_SenderX;
	m_ZeroY = m_SenderY;
	switch (rand() % 4) {
		case 0: m_ZeroX = (m_ZeroX + 1) % m_MapSize;					break;
		case 1: m_ZeroX = m_ZeroX == 0 ? m_MapSize - 1 : m_ZeroX - 1;	break;
		case 2: m_ZeroY = (m_ZeroY + 1) % m_MapSize;					break;
		case 3: m_ZeroY = m_ZeroY == 0 ? m_MapSize - 1 : m_ZeroY - 1;	break;
		default:
			assert(false);
			break;
	}

	MakeConnection(m_SenderX, m_SenderY, m_ZeroX, m_ZeroY);
}


CCell& CMap::GetCell(const unsigned short x, const unsigned short y)
{
	//Check for Outside the map
	assert(x < m_MapSize && y < m_MapSize);
	return m_CellMap[x + y * m_MapSize];
}


const CCell& CMap::GetCell(const unsigned short x, const unsigned short y) const
{
	//Check for Outside the map
	assert(x < m_MapSize && y < m_MapSize);
	return m_CellMap[x + y * m_MapSize];
}


bool CMap::InstallReceiver()
{
	//Get free cells
	vector< pair<unsigned short, unsigned short> > freeCells;
	for (unsigned short x = 0; x < m_MapSize; ++x) {
		for (unsigned short y = 0; y < m_MapSize; ++y) {
			CCell& cell = GetCell(x, y);
			if (cell.GetCellType() == CCell::CTFree)
				freeCells.push_back(make_pair(x, y));
			cell.SetUsed(false);

		}
	}
	if (freeCells.empty())
		return true;	//No more free cells

	bool result = false;
	int tryCounter(m_MapSize * 2);
	while (tryCounter-- && !result) {

		//Backup current map state
		vector<CCell> backup;
		backup.reserve(m_MapSize * m_MapSize);
		for (unsigned short i = 0; i < m_MapSize * m_MapSize; ++i)
			backup.push_back(m_CellMap[i]);

		const int freeCellInd = rand() % freeCells.size();
		const unsigned short freeX = freeCells[freeCellInd].first;
		const unsigned short freeY = freeCells[freeCellInd].second;
		CCell& rcv = GetCell(freeX, freeY);
		rcv.SetAsReceiver();

		result = MakeRoute(freeX, freeY);

		if (!result) {
			//Restore map
			for (unsigned short i = 0; i < m_MapSize * m_MapSize; ++i)
				m_CellMap[i] = backup[i];
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
			i = 1 - (rand() % 3);
			j = 1 - (rand() % 3);
		} while ((i && j) || (!i && !j));	//Diagonal


		if (!m_WrapMode) {
			if ((j < 0 && x == 0) || (j > 0 && x == m_MapSize - 1) ||
				(i < 0 && y == 0) || (i > 0 && y == m_MapSize - 1)) {
					--tryCounter;
					continue;
			}
		}

		const unsigned short cpX = (j < 0 && x == 0) ? m_MapSize - 1 : (x + j) % m_MapSize;
		const unsigned short cpY = (i < 0 && y == 0) ? m_MapSize - 1 : (y + i) % m_MapSize;

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

	if (GetCell(nextX, nextY).GetTubeType() == CCell::TTJoiner || (nextX == m_ZeroX && nextY == m_ZeroY))
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
	if (nextX == 0 && currX == m_MapSize - 1) {
		cellCurr.AddTube(CCell::CSRight);
		cellNext.AddTube(CCell::CSLeft);
	}
	else if (nextX == m_MapSize - 1 && currX == 0) {
		cellCurr.AddTube(CCell::CSLeft);
		cellNext.AddTube(CCell::CSRight);
	}
	else if (nextY == 0 && currY == m_MapSize - 1) {
		cellCurr.AddTube(CCell::CSBottom);
		cellNext.AddTube(CCell::CSTop);
	}
	else if (nextY == m_MapSize - 1 && currY == 0) {
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
		if (m_WrapMode || y > 0) {
			const unsigned short cpY = y > 0 ? y - 1 : m_MapSize - 1;
			CCell& cellNext = GetCell(x, cpY);
			if (cellNext.IsBottomConnected())
				DefineConnectStatus(x, cpY);
		}
	}

	//to down
	if (cellCurr.IsBottomConnected()) {
		if (m_WrapMode || y < m_MapSize - 1) {
			const unsigned short cpY = (y + 1) % m_MapSize;
			CCell& cellNext = GetCell(x, cpY);
			if (cellNext.IsTopConnected())
				DefineConnectStatus(x, cpY);
		}
	}

	//to left
	if (cellCurr.IsLeftConnected()) {
		if (m_WrapMode || x > 0) {
			const unsigned short cpX = x > 0 ? x - 1 : m_MapSize - 1;
			CCell& cellNext = GetCell(cpX, y);
			if (cellNext.IsRightConnected())
				DefineConnectStatus(cpX, y);
		}
	}

	//to right
	if (cellCurr.IsRightConnected()) {
		if (m_WrapMode || x < m_MapSize - 1) {
			const unsigned short cpX = (x + 1) % m_MapSize;
			CCell& cellNext = GetCell(cpX, y);
			if (cellNext.IsLeftConnected())
				DefineConnectStatus(cpX, y);
		}
	}
}


void CMap::DefineConnectStatus()
{
	//Reset connection status
	for (unsigned short x = 0; x < m_MapSize; ++x)
		for (unsigned short y = 0; y < m_MapSize; ++y)
			GetCell(x, y).SetActive(false);

	DefineConnectStatus(m_SenderX, m_SenderY);
	CheckForGameOver();
}
