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
		
		bool fUseMaxRoute = (getRandom(10) < 5);
		
		bool fRouteExist = fUseMaxRoute ?
				makeMaxRoute(pairWksPos.first, pairWksPos.second, 0, true) :
				makeMinRoute(pairWksPos.first, pairWksPos.second, MAX_WEIGHT, true);
		
		//Clear Used flag
		clearUsed();
		
		if (!fRouteExist)
			pWks->setType(CPipeWidget::None);
		else return fUseMaxRoute ?
				makeMaxRoute(pairWksPos.first, pairWksPos.second, 0, false) :
				makeMinRoute(pairWksPos.first, pairWksPos.second, MAX_WEIGHT, false);
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
		int nCheckXPoint = nXPoint;
		int nCheckYPoint = (nYPoint - 1 < 0) ? m_nYNumSquare - 1 : nYPoint - 1;
		CPipeWidget* pNextObj = getObject(nCheckXPoint, nCheckYPoint);
		if ((pNextObj->getConnSide() & CONNECT_DOWN) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nCheckXPoint, nCheckYPoint);
	}
	
	//to down
	if (pObj->getConnSide() & CONNECT_DOWN) {
		int nCheckXPoint = nXPoint;
		int nCheckYPoint = (nYPoint + 1 >= m_nYNumSquare) ? 0 : nYPoint + 1;
		CPipeWidget* pNextObj = getObject(nCheckXPoint, nCheckYPoint);
		if ((pNextObj->getConnSide() & CONNECT_UP) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nCheckXPoint, nCheckYPoint);
	}

	//to left
	if (pObj->getConnSide() & CONNECT_LEFT) {
		int nCheckXPoint = (nXPoint - 1 < 0) ? m_nXNumSquare - 1 : nXPoint - 1;
		int nCheckYPoint = nYPoint;
		CPipeWidget* pNextObj = getObject(nCheckXPoint, nCheckYPoint);
		if ((pNextObj->getConnSide() & CONNECT_RIGHT) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nCheckXPoint, nCheckYPoint);
	}
	
	//to right
	if (pObj->getConnSide() & CONNECT_RIGHT) {
		int nCheckXPoint = (nXPoint + 1 >= m_nXNumSquare) ? 0 : nXPoint + 1;
		int nCheckYPoint = nYPoint;
		CPipeWidget* pNextObj = getObject(nCheckXPoint, nCheckYPoint);
		if ((pNextObj->getConnSide() & CONNECT_LEFT) && (!pNextObj->rotateInProgress()))
			defineConnectStatus(nCheckXPoint, nCheckYPoint);
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
 * Test and make route from workstation to server (min to max)
 * @param nXPoint X coordinate of cell
 * @param nYPoint Y coordinate of cell
 * @param nWeight maximal weight
 * @param fTestOnly test route flag (true = only check if route exist)
 * @return result (true = route found)
 */
bool CPipeWalker::makeMaxRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly)
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
				nCheckXPoint = m_nXNumSquare;
			if (nCheckXPoint >= m_nXNumSquare)
				nCheckXPoint = 0;
			if (nCheckYPoint < 0)
				nCheckYPoint = m_nYNumSquare;
			if (nCheckYPoint >= m_nYNumSquare)
				nCheckYPoint = 0;

			CPipeWidget* pObj = getObject(nCheckXPoint, nCheckYPoint);
			if (pObj->getWeight() < nMaxWeight)
				continue;
			if (pObj->getUsed())
				continue;
			if (pObj->getType() == CPipeWidget::None || pObj->getType() == CPipeWidget::Line) {
				nXMax = nCheckXPoint;
				nYMax = nCheckYPoint;
				nMaxWeight = pObj->getWeight();
			}
		}
	}
	
	if (nMaxWeight == nWeight)	//Riched max point - no more points large than last
		return makeMinRoute(nXPoint, nYPoint, MAX_WEIGHT, fTestOnly);

	CPipeWidget* pCurrObj = getObject(nXMax, nYMax);
	
	if (pCurrObj->getType() == CPipeWidget::Line && fTestOnly)
		return true;	//We can install hub at this point

	//Set Used flag
	pCurrObj->setUsed(true);
			
	if (!fTestOnly) {
		
		//Define current way direction (output for previos object)
		setConnectStatus(nXPoint, nYPoint, nXMax, nYMax);
		
		if (pCurrObj->getType() == CPipeWidget::Line) {
			pCurrObj->setType(CPipeWidget::Hub);
			return true;	//This is the end point for route
		}
		else
			pCurrObj->setType(CPipeWidget::Line);
	}
	
	return makeMaxRoute(nXMax, nYMax, nMaxWeight, fTestOnly);

}


/**
 * Test and make route from workstation to server (max to min)
 * @param nXPoint X coordinate of cell
 * @param nYPoint Y coordinate of cell
 * @param nWeight minimal weight
 * @param fTestOnly test route flag (true = only check if route exist)
 * @return result (true = route found)
 */
bool CPipeWalker::makeMinRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly)
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
				nCheckXPoint = m_nXNumSquare;
			if (nCheckXPoint >= m_nXNumSquare)
				nCheckXPoint = 0;
			if (nCheckYPoint < 0)
				nCheckYPoint = m_nYNumSquare;
			if (nCheckYPoint >= m_nYNumSquare)
				nCheckYPoint = 0;

			CPipeWidget* pObj = getObject(nCheckXPoint, nCheckYPoint);
			if (pObj->getWeight() >= nMinWeight)
				continue;
			if (pObj->getUsed())
				continue;
			if (pObj->getType() == CPipeWidget::None || pObj->getType() == CPipeWidget::Line) {
				nXMin = nCheckXPoint;
				nYMin = nCheckYPoint;
				nMinWeight = pObj->getWeight();
			}
		}
	}
	
	//qDebug("Min weight found: %i:%i (%i) from  %i:%i", nXMin, nYMin, nMinWeight, nXPoint, nYPoint);
	
	if (nMinWeight == nWeight)
		return false;	//No route

	CPipeWidget* pCurrObj = getObject(nXMin, nYMin);
	
	//Set Used flag
	pCurrObj->setUsed(true);

	if (pCurrObj->getType() == CPipeWidget::Line && fTestOnly)
		return true;	//We can install hub at this point

	if (!fTestOnly) {
		
		//Define current way direction (output for previos object)
		setConnectStatus(nXPoint, nYPoint, nXMin, nYMin);
		
		if (pCurrObj->getType() == CPipeWidget::Line) {
			pCurrObj->setType(CPipeWidget::Hub);
			return true;	//This is the end point for route
		}
		else
			pCurrObj->setType(CPipeWidget::Line);
	}
	
	//if its no end - requrs
	return (nMinWeight == 0 ? true : makeMinRoute(nXMin, nYMin, nMinWeight, fTestOnly));
}


/**
 * Set connection side status
 * @param nPrevXPoint Previous X point
 * @param nPrevYPoint Previous Y point
 * @param nCurrXPoint Current X point
 * @param nCurrYPoint Current Y point
 */
void CPipeWalker::setConnectStatus(int nPrevXPoint, int nPrevYPoint, int nCurrXPoint, int nCurrYPoint)
{
	CPipeWidget* pCurrObj = getObject(nCurrXPoint, nCurrYPoint);
	CPipeWidget* pPrevObj = getObject(nPrevXPoint, nPrevYPoint);
	
	//Crossover
	if (nCurrXPoint == 0 && nPrevXPoint == m_nXNumSquare - 1) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_RIGHT);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_LEFT);
	}	
	else if (nCurrXPoint == m_nXNumSquare -1 && nPrevXPoint == 0) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_LEFT);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_RIGHT);
	}
	else if (nCurrYPoint == 0 && nPrevYPoint == m_nYNumSquare -1 ) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_DOWN);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_UP);
	}
	else if (nCurrYPoint == m_nYNumSquare -1 && nPrevYPoint == 0) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_UP);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_DOWN);
	}
	//Usualy
	else if (nCurrXPoint < nPrevXPoint) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_LEFT);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_RIGHT);
	}
	else if (nCurrXPoint > nPrevXPoint) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_RIGHT);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_LEFT);
	}
	else if (nCurrYPoint < nPrevYPoint) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_UP);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_DOWN);
	}
	else if (nCurrYPoint > nPrevYPoint) {
		pPrevObj->setConnSide(pPrevObj->getConnSide() | CONNECT_DOWN);
		pCurrObj->setConnSide(pCurrObj->getConnSide() | CONNECT_UP);
	}
}

