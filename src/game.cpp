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
: m_fGameOver(false)
{
	srand(time(0));
}


CGame::~CGame(void)
{
}


bool CGame::New(void)
{
	m_fGameOver = false;
	bool fInstallDone = false;
	int nTryCounter = 999;
	while (nTryCounter-- && !fInstallDone) {
		ResetMap();

		//Install server
		InstallSender();

		//Install workstations
		fInstallDone = true;
		for (int i = 0; i < MAP_RCV_NUM && fInstallDone; i++)
			fInstallDone &= InstallReceiver();
	}
	if (fInstallDone) {
		//Set sides angle
		for (int i = 0; i < MAP_WIDTH_NUM; i++) {
			for (int j = 0; j < MAP_HEIGHT_NUM; j++) {
				CCell* pCell = GetObject(i, j);
				//Convert straight tube to curve
				if (pCell->Type == CCell::ObjStrTube) {
					if (pCell->ConnSide == (CONNECT_UP | CONNECT_RIGHT) ||
						pCell->ConnSide == (CONNECT_RIGHT | CONNECT_DOWN) ||
						pCell->ConnSide == (CONNECT_DOWN | CONNECT_LEFT) ||
						pCell->ConnSide == (CONNECT_LEFT | CONNECT_UP))
						pCell->Type = CCell::ObjCurTube;
				}
				pCell->SetAngleBySide();
			}
		}
#ifdef NDEBUG
		Reset();
#else
		DefineConnectStatus();
#endif
	}

	return fInstallDone;
}


void CGame::ResetMap(void)
{
	for (unsigned int i = 0; i < sizeof(m_vMap) / sizeof(m_vMap[0]); i++)
		m_vMap[i].Reset();
}


bool CGame::CheckGameOver(void)
{
	m_fGameOver = true;
	for (unsigned int i = 0; (i < sizeof(m_vMap) / sizeof(m_vMap[0])) && m_fGameOver; i++)
		m_fGameOver = m_vMap[i].RotateStart == 0 && (m_vMap[i].State || m_vMap[i].Type == CCell::ObjNone);
	return m_fGameOver;
}


void CGame::Reset(void)
{
	m_fGameOver = false;
	for (unsigned int i = 0; i < sizeof(m_vMap) / sizeof(m_vMap[0]); i++)
		m_vMap[i].StartRotate(GetRandom(10) > 5, GetRandom(10) > 3);
	DefineConnectStatus();
}


void CGame::InstallSender(void)
{
	m_nXSndrPos = GetRandom(MAP_WIDTH_NUM);
	m_nYSndrPos = GetRandom(MAP_HEIGHT_NUM);
	CCell* pSrv = GetObject(m_nXSndrPos, m_nYSndrPos);
	pSrv->Type = CCell::ObjSender;

	//Define zero point (lan-start)
	bool fZeroPointInstalled = false;
	while (!fZeroPointInstalled) {
		m_nXZeroPos = m_nXSndrPos;
		m_nYZeroPos = m_nYSndrPos;
		switch (GetRandom(4)) {
			case 0: pSrv->ConnSide = CONNECT_RIGHT;	m_nXZeroPos++;	break;
			case 1: pSrv->ConnSide = CONNECT_LEFT;	m_nXZeroPos--;	break;
			case 2: pSrv->ConnSide = CONNECT_DOWN;	m_nYZeroPos++;	break;
			case 3: pSrv->ConnSide = CONNECT_UP;	m_nYZeroPos--;	break;
		}
		if (m_nXZeroPos >=0 && m_nYZeroPos >=0 && m_nXZeroPos < MAP_WIDTH_NUM && m_nYZeroPos < MAP_HEIGHT_NUM)
			fZeroPointInstalled = true;
	}

	//Set direction for zero point to server line
	CCell* pZeroPoint = GetObject(m_nXZeroPos, m_nYZeroPos);
	switch (pSrv->ConnSide) {
		case CONNECT_RIGHT:	pZeroPoint->ConnSide = CONNECT_LEFT;	break;
		case CONNECT_LEFT:	pZeroPoint->ConnSide = CONNECT_RIGHT;	break;
		case CONNECT_UP:	pZeroPoint->ConnSide = CONNECT_DOWN;	break;
		case CONNECT_DOWN:	pZeroPoint->ConnSide = CONNECT_UP;		break;
	}

	//Fill tables weight from zero point
	FillMapWeight();
}


CCell* CGame::GetObject(int nXPoint, int nYPoint)
{
	//Check for Outside the map
	assert(nXPoint >= 0 && nYPoint >= 0 && nXPoint < MAP_WIDTH_NUM && nYPoint < MAP_HEIGHT_NUM);
	return &m_vMap[nXPoint + nYPoint * MAP_HEIGHT_NUM];
}


void CGame::FillMapWeight(int nXPoint /*= 0*/, int nYPoint /*= 0*/, int nWeight /*= 1*/)
{
	if (nWeight == 1) { //First iteration in this recursion
		//Clear cell's weight
		for (unsigned int i = 0; i < sizeof(m_vMap) / sizeof(m_vMap[0]); i++)
			m_vMap[i].Weight = MAX_WEIGHT;

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
			if (nX < 0 || nX >= MAP_WIDTH_NUM || nY < 0 || nY >= MAP_HEIGHT_NUM)
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
	int nTryCounter(9);
	while (nTryCounter--) {
		//Get free cells
		int nFreeCount = 0;
		int nFreeCoord[MAP_WIDTH_NUM * MAP_HEIGHT_NUM - 2];	// Maximum free cells - server - zero point
		unsigned int i, j;
		for (i = 0; i < MAP_WIDTH_NUM; i++) {
			for (j = 0; j < MAP_HEIGHT_NUM; j++) {
				if (GetObject(i, j)->Type == CCell::ObjNone && !(i == m_nXZeroPos && j == m_nYZeroPos))
					nFreeCoord[nFreeCount++] = MAKELONG(i, j);
			}
		}
		if (nFreeCount == 0)
			return false;	//No more free cells

		int nFreeCell = GetRandom(nFreeCount);
		int nYFree = HIWORD(nFreeCoord[nFreeCell]);
		int nXFree = LOWORD(nFreeCoord[nFreeCell]);
		CCell* pRcv = GetObject(nXFree, nYFree);
		pRcv->Type = CCell::ObjReceiver;

		bool fUseMaxRoute = (GetRandom(10) < 7);

		bool fRouteExist = fUseMaxRoute ?
				MakeMaxRoute(nXFree, nYFree, 0, true) :
				MakeMinRoute(nXFree, nYFree, MAX_WEIGHT, true);

		//Clear Used flag
		for (i = 0; i < sizeof(m_vMap) / sizeof(m_vMap[0]); i++)
			m_vMap[i].Used = false;

		if (!fRouteExist)
			pRcv->Type = CCell::ObjNone;
		else return fUseMaxRoute ?
				MakeMaxRoute(nXFree, nYFree, 0, false) :
				MakeMinRoute(nXFree, nYFree, MAX_WEIGHT, false);
	}
	return false;
}


bool CGame::MakeMaxRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly)
{
	int nMaxWeight(nWeight);	//Maximal weight
	int nXMax(0), nYMax(0);		//Coordinates of maximal point

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			//Searching for maximal weigth in neighbouring cell's
			if ((i && j) || (!i && !j)) continue;	//Diagonal
			int nCheckXPoint = nXPoint + i;
			int nCheckYPoint = nYPoint + j;
			if (nCheckXPoint < 0)
				nCheckXPoint = MAP_WIDTH_NUM;
			if (nCheckXPoint >= MAP_HEIGHT_NUM)
				nCheckXPoint = 0;
			if (nCheckYPoint < 0)
				nCheckYPoint = MAP_WIDTH_NUM;
			if (nCheckYPoint >= MAP_HEIGHT_NUM)
				nCheckYPoint = 0;

			CCell* pObj = GetObject(nCheckXPoint, nCheckYPoint);
			if (pObj->Weight < nMaxWeight)
				continue;
			if (pObj->Used)
				continue;
			if (pObj->Type == CCell::ObjNone || pObj->Type == CCell::ObjStrTube || pObj->Type == CCell::ObjCurTube) {
				nXMax = nCheckXPoint;
				nYMax = nCheckYPoint;
				nMaxWeight = pObj->Weight;
			}
		}
	}

	if (nMaxWeight == nWeight)	//Riched max point - no more points large than last
		return MakeMinRoute(nXPoint, nYPoint, MAX_WEIGHT, fTestOnly);

	CCell* pCurrObj = GetObject(nXMax, nYMax);

	if ((pCurrObj->Type == CCell::ObjStrTube || pCurrObj->Type == CCell::ObjCurTube) && fTestOnly)
		return true;	//We can install joint at this point

	//Set Used flag
	pCurrObj->Used = true;

	if (!fTestOnly) {

		//Define current way direction (output for previos object)
		SetConnectStatus(nXPoint, nYPoint, nXMax, nYMax);

		if (pCurrObj->Type == CCell::ObjStrTube || pCurrObj->Type == CCell::ObjCurTube) {
			pCurrObj->Type = CCell::ObjTubeJoint;
			return true;	//This is the end point for route
		}
		else
			pCurrObj->Type = CCell::ObjStrTube;
	}

	return MakeMaxRoute(nXMax, nYMax, nMaxWeight, fTestOnly);
}


bool CGame::MakeMinRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly)
{
	int nMinWeight(nWeight);	//Minimal weight
	int nXMin(0), nYMin(0);		//Coordinates of minimal point

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			//Searching for minimal weigth in neighbouring cell's
			if ((i && j) || (!i && !j)) continue;	//Diagonal
			int nCheckXPoint = nXPoint + i;
			int nCheckYPoint = nYPoint + j;
			if (nCheckXPoint < 0)
				nCheckXPoint = MAP_WIDTH_NUM;
			if (nCheckXPoint >= MAP_HEIGHT_NUM)
				nCheckXPoint = 0;
			if (nCheckYPoint < 0)
				nCheckYPoint = MAP_WIDTH_NUM;
			if (nCheckYPoint >= MAP_HEIGHT_NUM)
				nCheckYPoint = 0;

			CCell* pObj = GetObject(nCheckXPoint, nCheckYPoint);
			if (pObj->Weight >= nMinWeight)
				continue;
			if (pObj->Used)
				continue;
			if (pObj->Type == CCell::ObjNone || pObj->Type == CCell::ObjStrTube || pObj->Type == CCell::ObjCurTube) {
				nXMin = nCheckXPoint;
				nYMin = nCheckYPoint;
				nMinWeight = pObj->Weight;
			}
		}
	}

	if (nMinWeight == nWeight)
		return false;	//No route

	CCell* pCurrObj = GetObject(nXMin, nYMin);

	//Set Used flag
	pCurrObj->Used = true;

	if ((pCurrObj->Type == CCell::ObjStrTube || pCurrObj->Type == CCell::ObjCurTube) && fTestOnly)
		return true;	//We can install hub at this point

	if (!fTestOnly) {

		//Define current way direction (output for previos object)
		SetConnectStatus(nXPoint, nYPoint, nXMin, nYMin);

		if (pCurrObj->Type == CCell::ObjStrTube || pCurrObj->Type == CCell::ObjCurTube) {
			pCurrObj->Type = CCell::ObjTubeJoint;
			return true;	//This is the end point for route
		}
		else
			pCurrObj->Type = CCell::ObjStrTube;
	}

	//if its no end - recursive call
	return (nMinWeight == 0 ? true : MakeMinRoute(nXMin, nYMin, nMinWeight, fTestOnly));
}


void CGame::SetConnectStatus(int nPrevXPoint, int nPrevYPoint, int nCurrXPoint, int nCurrYPoint)
{
	CCell* pCurrObj = GetObject(nCurrXPoint, nCurrYPoint);
	CCell* pPrevObj = GetObject(nPrevXPoint, nPrevYPoint);

	//Crossover
	if (nCurrXPoint == 0 && nPrevXPoint == MAP_WIDTH_NUM - 1) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_RIGHT;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_LEFT;
	}
	else if (nCurrXPoint == MAP_WIDTH_NUM -1 && nPrevXPoint == 0) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_LEFT;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_RIGHT;
	}
	else if (nCurrYPoint == 0 && nPrevYPoint == MAP_HEIGHT_NUM -1 ) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_DOWN;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_UP;
	}
	else if (nCurrYPoint == MAP_HEIGHT_NUM -1 && nPrevYPoint == 0) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_UP;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_DOWN;
	}
	//Usualy
	else if (nCurrXPoint < nPrevXPoint) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_LEFT;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_RIGHT;
	}
	else if (nCurrXPoint > nPrevXPoint) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_RIGHT;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_LEFT;
	}
	else if (nCurrYPoint < nPrevYPoint) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_UP;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_DOWN;
	}
	else if (nCurrYPoint > nPrevYPoint) {
		pPrevObj->ConnSide = pPrevObj->ConnSide | CONNECT_DOWN;
		pCurrObj->ConnSide = pCurrObj->ConnSide | CONNECT_UP;
	}
}


bool CGame::OnTubeRotated(CCell* pCell)
{
	bool fClockwise = pCell->RotateAngleEnd > pCell->RotateAngle;
	pCell->SetConnectionSideByRotate(fClockwise);
	pCell->RotateStart = 0;
	pCell->RotateAngle = pCell->RotateAngleEnd;

	if (pCell->RotateAngle == 360.0f)
		pCell->RotateAngle = 0.0f;

	bool fTwiceRotate = pCell->RotateTwice;
	if (fTwiceRotate) {
		pCell->RotateTwice = false;
		pCell->StartRotate(fClockwise, false);
	}
	else
		DefineConnectStatus();
	return fTwiceRotate;
}


void CGame::DefineConnectStatus(int nXPoint /*= -1*/, int nYPoint /*= -1*/)
{
	if (nXPoint < 0) {
		//This is the first recursive call
		int i, j;
		//Reset conection status
		for (i = 0; i < MAP_WIDTH_NUM; i++)
			for (j = 0; j < MAP_HEIGHT_NUM; j++)
				GetObject(i, j)->State = false;

		//For each sender on map (but is only one :-))
		for (i = 0; i < MAP_WIDTH_NUM; i++)
			for (j = 0; j < MAP_HEIGHT_NUM; j++)
				if (GetObject(i, j)->Type == CCell::ObjSender)
					DefineConnectStatus(i, j);
		return;
	}

	CCell* pObj = GetObject(nXPoint, nYPoint);
	if (pObj->State || pObj->RotateStart > 0)
		return;	//Already connected or rotate in progress

	pObj->State = true;

	//to up
	if (pObj->ConnSide & CONNECT_UP) {
		int nCheckXPoint = nXPoint;
		int nCheckYPoint = (nYPoint - 1 < 0) ? MAP_HEIGHT_NUM - 1 : nYPoint - 1;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->ConnSide & CONNECT_DOWN)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to down
	if (pObj->ConnSide & CONNECT_DOWN) {
		int nCheckXPoint = nXPoint;
		int nCheckYPoint = (nYPoint + 1 >= MAP_HEIGHT_NUM) ? 0 : nYPoint + 1;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->ConnSide & CONNECT_UP)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to left
	if (pObj->ConnSide & CONNECT_LEFT) {
		int nCheckXPoint = (nXPoint - 1 < 0) ? MAP_WIDTH_NUM - 1 : nXPoint - 1;
		int nCheckYPoint = nYPoint;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->ConnSide & CONNECT_RIGHT)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to right
	if (pObj->ConnSide & CONNECT_RIGHT) {
		int nCheckXPoint = (nXPoint + 1 >= MAP_WIDTH_NUM) ? 0 : nXPoint + 1;
		int nCheckYPoint = nYPoint;
		CCell* pNextObj = GetObject(nCheckXPoint, nCheckYPoint);
		if (pNextObj->ConnSide & CONNECT_LEFT)
			DefineConnectStatus(nCheckXPoint, nCheckYPoint);
	}
}
