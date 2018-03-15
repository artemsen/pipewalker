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
#ifndef PIPEWALKER_H
#define PIPEWALKER_H

#include <QVector>
#include "pipewidget.h"

/**
	@author Artem A. Senichev <artemsen@gmail.com>
*/
class CPipeWalker{
public:

	//Constructor/destructor
	CPipeWalker()  {};
	~CPipeWalker() {};

	//Custom map initialization
	void initializeCustom(const QVector<CPipeWidget*>& vObjectMap, int nWidth, int nHeght);
	
	//Random map initialization
	bool initializeRandom(const QVector<CPipeWidget*>& vObjectMap, int nWidth, int nHeght, int nWksNum);

	//Define connection status for lines/hubs/wks
	void defineConnectStatus(void);

	//Get random number from range [min, max]
	inline int getRandom(int nMax, int nMin = 0) const { return abs(int((double)rand() / (RAND_MAX + 1) * (nMax - nMin) + nMin)); }

private:	//Class variables
	int m_nXNumSquare, m_nYNumSquare;	//Map properties (size)
	int m_nXServerPos, m_nYServerPos;	//Server coordinates
	int m_nXZeroPoint, m_nYZeroPoint;	//Zero point coordinates
	
	QVector<CPipeWidget*> m_vObjectMap;	//Object's map
	
private:	//Internal class helper functions
	
	//Get object at position
	CPipeWidget* getObject(int nXPoint, int nYPoint);
	
	//Clear map
	void clearMap(void) { for (QVector<CPipeWidget*>::iterator iter = m_vObjectMap.begin(); iter != m_vObjectMap.end(); iter++) (*iter)->clear(); }
	
	//Install workstation on map
	bool installWorkstation(void);
	
	//Install server on map
	void installServer(void);
	
	//Fill map by weight for path search
	void fillMapWeight(int nXPoint = 0, int nYPoint = 0, int nWeight = 1);
	
	//Test and make route from workstation to server
	bool makeRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly);

	//Define connection status for lines/hubs/wks
	void defineConnectStatus(int nXPoint, int nYPoint);
};

#endif
