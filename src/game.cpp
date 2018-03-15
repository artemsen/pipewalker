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

#include "game.h"


CGame::CGame(void)
: m_pMap(NULL), m_fGameOver(false), m_nMapSize(10), m_nReceiversNum(25)
{
}


CGame::~CGame(void)
{
	DestroyMap();
}


void CGame::New(const unsigned short nSize /*= 0*/, const unsigned short nReceiversNum /*= 0*/, const unsigned int nMapId /*= 0*/)
{
	//Initialize map properties
	if (nSize)
		m_nMapSize = nSize;
	if (nReceiversNum)
		m_nReceiversNum = nReceiversNum;

	//Initailze random sequence
	m_nMapID = (nMapId == 0 ? ((GetRandom(0xFFF) << 24) | GetRandom(RAND_MAX)) + GetTickCount() : nMapId);
#ifndef NDEBUG
	int nTryCount = 0;
#endif
	srand(m_nMapID);

	m_fGameOver = false;
	bool fInstallDone = false;
	while (!fInstallDone) {
#ifndef NDEBUG
		nTryCount++;
#endif
		//Recreate map
		DestroyMap();
		CreateMap();

		//Install server
		InstallSender();

		//Install workstations
		fInstallDone = true;
		for (unsigned int i = 0; i < m_nReceiversNum && fInstallDone; i++)
			fInstallDone &= InstallReceiver();
	}
#ifndef NDEBUG
	printf("Generated map %ix%i/%i witdh ID 0x%08x after %i try\n", m_nMapSize, m_nMapSize, m_nReceiversNum, m_nMapID, nTryCount);
	DefineConnectStatus();
#else
	Reset();
#endif
	return;
}


bool CGame::CheckGameOver(void)
{
	m_fGameOver = true;
	for (unsigned int i = 0; (i < m_nMapSize * m_nMapSize) && m_fGameOver; i++) {
		if (m_pMap[i].IsRotationInProgress())
			m_fGameOver = false;
		else if (m_pMap[i].Type == CCell::Receiver && !m_pMap[i].State)
			m_fGameOver = false;
	}
	return m_fGameOver;
}


void CGame::Reset(void)
{
	m_fGameOver = false;
	for (unsigned int i = 0; i < m_nMapSize * m_nMapSize; i++) {
		if (m_pMap[i].Type != CCell::Free && !m_pMap[i].IsLocked() && GetRandom(10) > 0) {
			m_pMap[i].StartRotate(GetRandom(10) > 5 ? CCell::Positive : CCell::Negative);
			if (GetRandom(10) > 3) //Twice rotation
				m_pMap[i].StartRotate(CCell::Positive);
		}
	}
	DefineConnectStatus();
}


void CGame::InstallSender(void)
{
	m_nXSndrPos = GetRandom(m_nMapSize);
	m_nYSndrPos = GetRandom(m_nMapSize);
	CCell* pSrv = GetObject(m_nXSndrPos, m_nYSndrPos);
	pSrv->Type = CCell::Sender;

	//Define zero point (lan-start)
	bool fZeroPointInstalled = false;
	while (!fZeroPointInstalled) {
		m_nXZeroPos = m_nXSndrPos;
		m_nYZeroPos = m_nYSndrPos;
		switch (GetRandom(4)) {
			case 0: pSrv->Sides = CONNECT_RIGHT;	m_nXZeroPos++;	break;
			case 1: pSrv->Sides = CONNECT_LEFT;		m_nXZeroPos--;	break;
			case 2: pSrv->Sides = CONNECT_DOWN;		m_nYZeroPos++;	break;
			case 3: pSrv->Sides = CONNECT_UP;		m_nYZeroPos--;	break;
		}
		if (m_nXZeroPos >=0 && m_nYZeroPos >=0 && m_nXZeroPos < m_nMapSize && m_nYZeroPos < m_nMapSize)
			fZeroPointInstalled = true;
	}

	//Set direction for zero point to server line
	CCell* pZeroPoint = GetObject(m_nXZeroPos, m_nYZeroPos);
	pZeroPoint->Type = CCell::Tube;
	switch (pSrv->Sides) {
		case CONNECT_RIGHT:	pZeroPoint->Sides = CONNECT_LEFT;	break;
		case CONNECT_LEFT:	pZeroPoint->Sides = CONNECT_RIGHT;	break;
		case CONNECT_UP:	pZeroPoint->Sides = CONNECT_DOWN;	break;
		case CONNECT_DOWN:	pZeroPoint->Sides = CONNECT_UP;		break;
	}

	//Fill tables weight from zero point
	FillMapWeight();

#ifndef NDEBUG
	printf("Map weight:\n");
	printf("   | ");
	for (int xt = 0; xt < m_nMapSize; xt++)
		printf("%02i ", xt);
	printf("\n");
	printf("---+------------------------------\n");
	for (int y = 0; y < m_nMapSize; y++) {
		printf("%02i | ", y);
		for (int x = 0; x < m_nMapSize; x++)
			printf("%#2i ", GetObject(x, y)->Weight);
		printf("\n");
	}
#endif	//NDEBUG
}


CCell* CGame::GetObject(int nXPoint, int nYPoint)
{
	//Check for Outside the map
	assert(nXPoint >= 0 && nYPoint >= 0 && nXPoint < m_nMapSize && nYPoint < m_nMapSize);
	assert(m_pMap != NULL);
	return &m_pMap[nXPoint + nYPoint * m_nMapSize];
}


void CGame::FillMapWeight(int nXPoint /*= 0*/, int nYPoint /*= 0*/, int nWeight /*= 1*/)
{
	if (nWeight == 1) { //First iteration in this recursion
		//Clear cell's weight
		for (unsigned int i = 0; i < m_nMapSize * m_nMapSize; i++)
			m_pMap[i].Weight = MAX_WEIGHT;

		//Set weight for zero point
		GetObject(m_nXZeroPos, m_nYZeroPos)->Weight = 0;
		nXPoint = m_nXZeroPos;
		nYPoint = m_nYZeroPos;
	}

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if ((i && j) || (!i && !j))
				continue;	//Diagonal
			int nX = nXPoint + i;
			int nY = nYPoint + j;
			if (nX < 0 || nX >= static_cast<int>(m_nMapSize) || nY < 0 || nY >= static_cast<int>(m_nMapSize))
				continue;	//Not in map scope
			CCell* pObj = GetObject(nX, nY);
			if (pObj->Weight > nWeight) {
				pObj->Weight = nWeight;
				FillMapWeight(nX, nY, nWeight + 1);
			}
		}
	}
}


bool CGame::InstallReceiver(void)
{
	//Get free cells
	int nFreeCount = 0;
	int* pFreeCoord = new int[m_nMapSize * m_nMapSize - 2];	// Maximum free cells minus server minus zero point
	unsigned int i, j;
	for (i = 0; i < m_nMapSize; i++) {
		for (j = 0; j < m_nMapSize; j++) {
			if (GetObject(i, j)->Type == CCell::Free && !(i == m_nXZeroPos && j == m_nYZeroPos))
				pFreeCoord[nFreeCount++] = MAKELONG(i, j);
		}
	}
	if (nFreeCount == 0) {
		delete[] pFreeCoord;
		return false;	//No more free cells
	}

	bool fResult = false;
	int nTryCounter(9);
	while (nTryCounter-- && !fResult) {

		//Backup current map state
		CCell* pBackupMap = new CCell[m_nMapSize * m_nMapSize];
		for (unsigned int i = 0; i < m_nMapSize * m_nMapSize; i++)
			pBackupMap[i] = m_pMap[i];

		int nFreeCell = GetRandom(nFreeCount);
		int nYFree = HIWORD(pFreeCoord[nFreeCell]);
		int nXFree = LOWORD(pFreeCoord[nFreeCell]);
		
		CCell* pRcv = GetObject(nXFree, nYFree);
		pRcv->Type = CCell::Receiver;

		bool fUseMaxRoute = (GetRandom(10) < 7);
		fResult = MakeRoute(nXFree, nYFree, fUseMaxRoute ? 0 : MAX_WEIGHT, fUseMaxRoute);

		if (!fResult) {	//Restore map
			for (unsigned int i = 0; i < m_nMapSize * m_nMapSize; i++)
				m_pMap[i] = pBackupMap[i];
		}
		else {
			//Clear Used flag
			for (unsigned int i = 0; i < m_nMapSize * m_nMapSize; i++)
				m_pMap[i].Used = false;
		}
		delete[] pBackupMap;
	}
	delete[] pFreeCoord;
	return fResult;
}


bool CGame::MakeRoute(unsigned int nXPoint, unsigned int nYPoint, int nWeight, bool fUseMaxRoute)
{
	//Searching for maximal/minimal weigth and coordinates of neighbouring cell's
	int nMinX = -1, nMinY = -1, nMinWeight = -1;	//Minimal coordinates/weight of new next cell
	int nMaxX = -1, nMaxY = -1, nMaxWeight = -1;	//Maximal coordinates/weight of new next cell
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if ((i && j) || (!i && !j)) continue;	//Diagonal
			int nCheckXPoint = nXPoint + i;
			int nCheckYPoint = nYPoint + j;
			if (nCheckXPoint < 0)
				nCheckXPoint = m_nMapSize - 1;
			if (nCheckXPoint >= static_cast<int>(m_nMapSize))
				nCheckXPoint = 0;
			if (nCheckYPoint < 0)
				nCheckYPoint = m_nMapSize - 1;
			if (nCheckYPoint >= static_cast<int>(m_nMapSize))
				nCheckYPoint = 0;

			CCell* pObj = GetObject(nCheckXPoint, nCheckYPoint);

			if (pObj->Used || !pObj->CanAddTube())
				continue;

			if (nMaxWeight == -1 || pObj->Weight >= nMaxWeight) {
				nMaxX = nCheckXPoint;
				nMaxY = nCheckYPoint;
				nMaxWeight = pObj->Weight;
			}
			if (nMinWeight == -1 || pObj->Weight <= nMinWeight) {
				nMinX = nCheckXPoint;
				nMinY = nCheckYPoint;
				nMinWeight = pObj->Weight;
			}
		}
	}

	//Set coordinates and weight of new next cell
	unsigned int nNewXPoint = (fUseMaxRoute ? nMaxX : nMinX);
	unsigned int nNewYPoint = (fUseMaxRoute ? nMaxY : nMinY);
	int nNewWeight = (fUseMaxRoute ? nMaxWeight : nMinWeight);

	if (nNewWeight == -1) {	//New point not found...
		if (fUseMaxRoute)
			return MakeRoute(nXPoint, nYPoint, MAX_WEIGHT, false);	//Search minimal route
		else
			return false;	//Riched min point - we don't have a route
	}

	CCell* pCurrObj = GetObject(nNewXPoint, nNewYPoint);
	pCurrObj->Used = true;
	pCurrObj->Type = CCell::Tube;

	//Add tube to current cell
	CCell* pPrevObj = GetObject(nXPoint, nYPoint);

	//Crossover
	if (nNewXPoint == 0 && nXPoint == m_nMapSize - 1) {
		pPrevObj->AddTube(CONNECT_RIGHT);
		pCurrObj->AddTube(CONNECT_LEFT);
	}
	else if (nNewXPoint == m_nMapSize - 1 && nXPoint == 0) {
		pPrevObj->AddTube(CONNECT_LEFT);
		pCurrObj->AddTube(CONNECT_RIGHT);
	}
	else if (nNewYPoint == 0 && nYPoint == m_nMapSize - 1) {
		pPrevObj->AddTube(CONNECT_DOWN);
		pCurrObj->AddTube(CONNECT_UP);
	}
	else if (nNewYPoint == m_nMapSize - 1 && nYPoint == 0) {
		pPrevObj->AddTube(CONNECT_UP);
		pCurrObj->AddTube(CONNECT_DOWN);
	}
	//Usualy
	else if (nNewXPoint < nXPoint) {
		pPrevObj->AddTube(CONNECT_LEFT);
		pCurrObj->AddTube(CONNECT_RIGHT);
	}
	else if (nNewXPoint > nXPoint) {
		pPrevObj->AddTube(CONNECT_RIGHT);
		pCurrObj->AddTube(CONNECT_LEFT);
	}
	else if (nNewYPoint < nYPoint) {
		pPrevObj->AddTube(CONNECT_UP);
		pCurrObj->AddTube(CONNECT_DOWN);
	}
	else if (nNewYPoint > nYPoint) {
		pPrevObj->AddTube(CONNECT_DOWN);
		pCurrObj->AddTube(CONNECT_UP);
	}

	if (!pCurrObj->CanAddTube())
		return true;

	return nNewWeight == 0 ? true : MakeRoute(nNewXPoint, nNewYPoint, nNewWeight, fUseMaxRoute);
}


void CGame::RotateTube(CCell* pCell, const CCell::Direction enuDir)
{
	assert(pCell);
	if (!pCell->IsLocked()) {
		pCell->StartRotate(enuDir);
		DefineConnectStatus();
	}
}


void CGame::OnTubeRotated(CCell* pCell)
{
	assert(pCell);
	pCell->EndRotate();
	DefineConnectStatus();
	CheckGameOver();
}


void CGame::DefineConnectStatus(int nXPoint /*= -1*/, int nYPoint /*= -1*/)
{
	if (nXPoint < 0) {
		//This is the first recursive call
		unsigned int i, j;
		//Reset conection status
		for (i = 0; i < m_nMapSize * m_nMapSize; i++)
			m_pMap[i].State = false;

		//For each sender on map (but it is only one :-))
		for (i = 0; i < m_nMapSize; i++)
			for (j = 0; j < m_nMapSize; j++)
				if (GetObject(i, j)->Type == CCell::Sender)
					DefineConnectStatus(i, j);
		return;
	}

	CCell* pObj = GetObject(nXPoint, nYPoint);
	if (pObj->State || pObj->IsRotationInProgress())
		return;	//Already connected or rotate in progress

	pObj->State = true;

	//to up
	if (pObj->Sides & CONNECT_UP) {
		int nCheckXPoint = nXPoint;
		int nCheckYPoint = (nYPoint - 1 < 0) ? m_nMapSize - 1 : nYPoint - 1;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->Sides & CONNECT_DOWN)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to down
	if (pObj->Sides & CONNECT_DOWN) {
		int nCheckXPoint = nXPoint;
		int nCheckYPoint = (nYPoint + 1 >= static_cast<int>(m_nMapSize)) ? 0 : nYPoint + 1;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->Sides & CONNECT_UP)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to left
	if (pObj->Sides & CONNECT_LEFT) {
		int nCheckXPoint = (nXPoint - 1 < 0) ? m_nMapSize - 1 : nXPoint - 1;
		int nCheckYPoint = nYPoint;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->Sides & CONNECT_RIGHT)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to right
	if (pObj->Sides & CONNECT_RIGHT) {
		int nCheckXPoint = (nXPoint + 1 >= static_cast<int>(m_nMapSize)) ? 0 : nXPoint + 1;
		int nCheckYPoint = nYPoint;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->Sides & CONNECT_LEFT)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}
}
