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
#ifndef PIPEWIDGET_H
#define PIPEWIDGET_H

#include <QtGui>
#include <math.h>

/**
	@author Artem A. Senichev <artemsen@gmail.com>
*/

//Connected sides
#define CONNECT_UNDEF	0x0
#define CONNECT_UP	0x1
#define CONNECT_RIGHT	0x2
#define CONNECT_DOWN	0x4
#define CONNECT_LEFT	0x8

#define ROTATE_ANGLE		90.0	//Angle of rotate step
#define ROTATE_STEP		0.5	//Step of angle of rotate
#define MAX_WEIGHT		0xFFFFF	//Maximum weight of cell

class CPipeWidget : public QWidget
{
Q_OBJECT
public:
	CPipeWidget(QWidget* wndParent,
		QImage* pBackground,
		QImage* pLineActive,
		QImage* pLinePassive,
		QImage* pServer,
		QImage* pWksActive,
		QImage* pWksPassive,
		QImage* pHubActive,
		QImage* pHubPassive,
		QImage* pHLineActive,
		QImage* pHLinePassive,
		QImage* pCornerActive,
		QImage* pCornerPassive);

	//Object types
	enum Type	{ None = 0, Line, Hub, Workstation, Server };
	
	//Rotate object
	void rotate(bool nByClockWise, bool fShow);
	
	//Set initial angle by connected side status
	void setAngleBySide(void);
	
	//Clear state of cell
	void clear(void) { m_dRotateAngle = 0; m_fConnFlag = false; m_fUsed = false; m_enuType = None; m_nConnSide = CONNECT_UNDEF; m_nWeight = MAX_WEIGHT; }
	

protected:
	void paintEvent(QPaintEvent* pEvent);
	void mousePressEvent(QMouseEvent* pEvent);

private:
	//Images
	QImage* m_pBackground;
	QImage* m_pLineActive;
	QImage* m_pLinePassive;
	QImage* m_pServer;
	QImage* m_pWksActive;
	QImage* m_pWksPassive;
	QImage* m_pHubActive;
	QImage* m_pHubPassive;
	QImage* m_pHLineActive;
	QImage* m_pHLinePassive;
	QImage* m_pCornerActive;
	QImage* m_pCornerPassive;
	
	bool	m_fRotateClockWise;	//Rotate directions
	double	m_dRotateAngle;		//Rotate angle
	
	bool	m_fConnFlag;
	Type	m_enuType;
	int	m_nConnSide;
	int	m_nWeight;
	bool	m_fUsed;
	

public:
	inline bool rotateInProgress(void) const{ return fmod(fabs(m_dRotateAngle), ROTATE_ANGLE) != 0; }
	inline bool getConnFlag(void) const	{ return m_fConnFlag; }
	inline void setConnFlag(bool fFlag)	{ m_fConnFlag = fFlag; }
	inline Type getType(void) const		{ return m_enuType; }
	inline void setType(Type enuType)	{ m_enuType = enuType; }
	inline int getConnSide(void) const	{ return m_nConnSide; }
	inline void setConnSide(int nSide)	{ m_nConnSide = nSide; }
	inline int getWeight(void) const	{ return m_nWeight; }
	inline void setWeight(int nWeight)	{ m_nWeight = nWeight; }
	inline bool getUsed() const		{ return m_fUsed; }
	inline void setUsed(bool fFlag)		{ m_fUsed = fFlag; }
};

#endif
