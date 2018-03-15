/***************************************************************************
 *   Copyright (C) 2007 by Artem A. Senichev                               *
 *   artemsen@gmail.com                                                    *
 *                                                                         *
 *   This file is part of the PipeWalker game                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QPair>
#include <math.h>
#include "pipewalker.h"


/**
 * Get object at position
 * @param nXPoint X position
 * @param nYPoint Y position
 * @return pointer to CPipeWidget object
 */
CPipeWidget* CPipeWalker::getObject(int nXPoint, int nYPoint)
{
	if (nXPoint < 0 || nYPoint < 0 || nXPoint >= m_nXNumSquare || nYPoint >= m_nYNumSquare)
		return 0;	//Outside the map
	return m_vObjectMap.at(nXPoint + nYPoint * m_nYNumSquare);
}

	
/**
 * Custom map initialization
 * @param vObjectMap vector of objects
 * @param width Number of square by horizontal
 * @param width Number of square by horizontal
 */
void CPipeWalker::initializeCustom(const QVector<CPipeWidget*>& vObjectMap, int nWidth, int nHeght)
{
	m_nXNumSquare = nWidth;
	m_nYNumSquare = nHeght;
	m_vObjectMap = vObjectMap;
}


/**
 * Random map initialization
 * @param vObjectMap vector of objects
 * @param width Number of square by horizontal
 * @param width Number of square by horizontal
 * @param heght Number of square by vertical
 * @param wksnum Number of workstation on map
 * @return result (false = unable to create map (to many workstation?))
 */
bool CPipeWalker::initializeRandom(const QVector<CPipeWidget*>& vObjectMap, int nWidth, int nHeght, int nWksNum)
{
	m_nXNumSquare = nWidth;
	m_nYNumSquare = nHeght;
	m_vObjectMap = vObjectMap;
	
	bool fInstallDone = false;
	int nTryCounter(999);
	while (nTryCounter-- && !fInstallDone) {
		
		//Clear map
		clearMap();
		
		//Install server
		installServer();

		//Install workstations
		fInstallDone = true;
		for (int i = 0; i < nWksNum && fInstallDone; i++)
			fInstallDone &= installWorkstation();
	}
	if (!fInstallDone)
		clearMap();
	else {
		//Set sides angle
		for (int i = 0; i < m_nXNumSquare; i++)
			for (int j = 0; j < m_nYNumSquare; j++)
				getObject(i, j)->setAngleBySide();
	}

	return fInstallDone;
}


/**
 * Install server on map
 */
void CPipeWalker::installServer(void)
{
	m_nXServerPos = getRandom(m_nXNumSquare);
	m_nYServerPos = getRandom(m_nYNumSquare);
	CPipeWidget* pSrv = getObject(m_nXServerPos, m_nYServerPos);
	pSrv->setType(CPipeWidget::Server);
	
	//Define zero point (lan-start)
	bool fZeroPointInstalled = false;
	while (!fZeroPointInstalled) {
		m_nXZeroPoint = m_nXServerPos;
		m_nYZeroPoint = m_nYServerPos;
		switch (getRandom(4)) {
			case 0: pSrv->setConnSide(CONNECT_RIGHT);	m_nXZeroPoint += 1;	break;
			case 1: pSrv->setConnSide(CONNECT_LEFT);	m_nXZeroPoint -= 1;	break;
			case 2: pSrv->setConnSide(CONNECT_DOWN);	m_nYZeroPoint += 1;	break;
			case 3: pSrv->setConnSide(CONNECT_UP);		m_nYZeroPoint -= 1;	break;
		}
		if (m_nXZeroPoint >=0 && m_nYZeroPoint >=0 && m_nXZeroPoint < m_nXNumSquare && m_nYZeroPoint < m_nYNumSquare)
			fZeroPointInstalled = true;
	}
	
	//Set direction for zero point to server line
	CPipeWidget* pZeroPoint = getObject(m_nXZeroPoint, m_nYZeroPoint);
	switch (pSrv->getConnSide()) {
		case CONNECT_RIGHT:	pZeroPoint->setConnSide(CONNECT_LEFT);	break;
		case CONNECT_LEFT:	pZeroPoint->setConnSide(CONNECT_RIGHT);	break;
		case CONNECT_UP:	pZeroPoint->setConnSide(CONNECT_DOWN);	break;
		case CONNECT_DOWN:	pZeroPoint->setConnSide(CONNECT_UP);	break;
	}

	//Fill tables weight from zero point
	fillMapWeight();
}


/**
 * Install workstation on map
 */
bool CPipeWalker::installWorkstation(void)
{
	int nTryCounter(9);
	while (nTryCounter--) {
	
		//Get free cells
		QVector< QPair<int, int> > vFreeCells;
		for (int i = 0; i < m_nXNumSquare; i++) {
			for (int j = 0; j < m_nYNumSquare; j++) {
				if (getObject(i, j)->getType() == CPipeWidget::None && !(i == m_nXZeroPoint && j == m_nYZeroPoint))
					vFreeCells.push_back(QPair<int, int>(i, j));
			}
		}
		if (vFreeCells.empty())
			return false;	//No more free cells

		QPair<int, int> pairWksPos = vFreeCells.at(getRandom(vFreeCells.size()));
		CPipeWidget* pWks = getObject(pairWksPos.first, pairWksPos.second);
		pWks->setType(CPipeWidget::Workstation);
	
		//qDebug("Install workstation to %i:%i", pairWksPos.first, pairWksPos.second);
		
		if (!makeRoute(pairWksPos.first, pairWksPos.second, MAX_WEIGHT, true))
			pWks->setType(CPipeWidget::None);
		else
			return makeRoute(pairWksPos.first, pairWksPos.second, MAX_WEIGHT, false);
	}
	return false;
}


/**
 * Define connection status for lines/hubs/wks
 */
void CPipeWalker::defineConnectStatus(void)
{
	//Reset conection status
	for (int i = 0; i < m_nXNumSquare; i++)
		for (int j = 0; j < m_nYNumSquare; j++)
			getObject(i, j)->setConnFlag(false);
	
	//For each server on map...
	for (int i = 0; i < m_nXNumSquare; i++) {
		for (int j = 0; j < m_nYNumSquare; j++) {
			if (getObject(i, j)->getType() == CPipeWidget::Server) {
				defineConnectStatus(i, j);
			}
		}
	}
}


/**
 * Define connection status for lines/hubs/wks
 * @param nXPoint X coordinates of zero cell (only for reqursive call)
 * @param nYPoint Y coordinates of zero cell (only for reqursive call)
 */
void CPipeWalker::defineConnectStatus(int nXPoint, int nYPoint)
{
	CPipeWidget* pObj = getObject(nXPoint, nYPoint);
	if (!pObj || pObj->getConnFlag() || pObj->rotateInProgress())
		return;
	
	pObj->setConnFlag(true);
	
	//to up
	if (pObj->getConnSide() & CONNECT_UP) {
		CPipeWidget* pNextObj = getObject(nXPoint, nYPoint - 1);
		if (pNextObj && (pNextObj->getConnSide() & CONNECT_DOWN) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nXPoint, nYPoint - 1);
	}
	
	//to down
	if (pObj->getConnSide() & CONNECT_DOWN) {
		CPipeWidget* pNextObj = getObject(nXPoint, nYPoint + 1);
		if (pNextObj && (pNextObj->getConnSide() & CONNECT_UP) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nXPoint, nYPoint + 1);
	}

	//to left
	if (pObj->getConnSide() & CONNECT_LEFT) {
		CPipeWidget* pNextObj = getObject(nXPoint - 1, nYPoint);
		if (pNextObj && (pNextObj->getConnSide() & CONNECT_RIGHT) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nXPoint - 1, nYPoint);
	}
	
	//to right
	if (pObj->getConnSide() & CONNECT_RIGHT) {
		CPipeWidget* pNextObj = getObject(nXPoint + 1, nYPoint);
		if (pNextObj && (pNextObj->getConnSide() & CONNECT_LEFT) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nXPoint + 1, nYPoint);
	}
}


/**
 * Fill map by weight for path search
 * @param nXPoint X coordinates of zero cell (only for reqursive call)
 * @param nYPoint Y coordinates of zero cell (only for reqursive call)
 * @param nWeight weight of cell (only for reqursive call)
 */
void CPipeWalker::fillMapWeight(int nXPoint /*= 0*/, int nYPoint /*= 0*/, int nWeight /*= 1*/)
{
	if (nWeight == 1) { //First iteration in this requrs
		//Clear cell's weight
		for (int i = 0; i < m_nXNumSquare; i++)
			for (int j = 0; j < m_nYNumSquare; j++)
				getObject(i, j)->setWeight(MAX_WEIGHT);
		
		//Set weight for zero point
		getObject(m_nXZeroPoint, m_nYZeroPoint)->setWeight(0);
		nXPoint = m_nXZeroPoint;
		nYPoint = m_nYZeroPoint;
	}
	
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if ((i && j) || (!i && !j)) continue;	//Diagonal
			CPipeWidget* pObj = getObject(nXPoint + i, nYPoint + j);
			if (pObj && pObj->getWeight() > nWeight) {
				pObj->setWeight(nWeight);
				fillMapWeight(nXPoint + i, nYPoint + j, nWeight + 1);
			}
		}
	}
}


/**
 * Test and make route from workstation to server
 * @param nXPoint X coordinate of cell
 * @param nYPoint Y coordinate of cell
 * @param nWeight minimal weight
 * @param fTestOnly test route flag (true = onle check route exist)
 * @return result (true = route found)
 */
bool CPipeWalker::makeRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly)
{
	int nMinWeight(MAX_WEIGHT);	//Minimal weight
	int nXMin(0), nYMin(0);		//Coordinates of minimal point
	
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			//Searching for minimal weigth in neighbouring cell's
			if ((i && j) || (!i && !j)) continue;	//Diagonal
			CPipeWidget* pObj = getObject(nXPoint + i, nYPoint + j);
			if (!pObj)
				continue;	//Outside the map
			if (pObj->getWeight() > nWeight)
				continue;
			if (pObj->getType() == CPipeWidget::None || pObj->getType() == CPipeWidget::Line) {
				nXMin = nXPoint + i;
				nYMin = nYPoint + j;
				nMinWeight = pObj->getWeight();
			}
		}
	}
	
	//qDebug("Min weight found: %i:%i (%i) from  %i:%i", nXMin, nYMin, nMinWeight, x, y);
	
	if (nMinWeight == MAX_WEIGHT)
		return false;	//No route

	CPipeWidget* pCurrObj = getObject(nXMin, nYMin);
	
	if (pCurrObj->getType() == CPipeWidget::Line && fTestOnly)
		return true;	//We can install hub at this point

	if (!fTestOnly) {
		
		//Define current way direction (output for previos object)
		CPipeWidget* pPrevObj = getObject(nXPoint, nYPoint);
		if (nXMin < nXPoint) {
			pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_LEFT);
			pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_RIGHT);
		}
		else if (nXMin > nXPoint) {
			pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_RIGHT);
			pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_LEFT);
		}
		else if (nYMin < nYPoint) {
			pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_UP);
			pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_DOWN);
		}
		else if (nYMin > nYPoint) {
			pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_DOWN);
			pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_UP);
		}
		
		if (pCurrObj->getType() == CPipeWidget::Line) {
			pCurrObj->setType(CPipeWidget::Hub);
			nMinWeight = 0;	//This is the end point for route
		}
		else
			pCurrObj->setType(CPipeWidget::Line);
	}
	
	//if its no end - requrs
	return (nMinWeight == 0 ? true : makeRoute(nXMin, nYMin, nMinWeight, fTestOnly));
}

