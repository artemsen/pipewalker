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

#include <QPainter>
#include "pipewidget.h"

CPipeWidget::CPipeWidget(QWidget* wndParent,
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
		QImage* pCornerPassive)
	: QWidget(wndParent),
	  m_pBackground(pBackground),
	  m_pLineActive(pLineActive),
	  m_pLinePassive(pLinePassive),
	  m_pServer(pServer),
	  m_pWksActive(pWksActive),
	  m_pWksPassive(pWksPassive),
	  m_pHubActive(pHubActive),
	  m_pHubPassive(pHubPassive),
	  m_pHLineActive(pHLineActive),
	  m_pHLinePassive(pHLinePassive),
	  m_pCornerActive(pCornerActive),
	  m_pCornerPassive(pCornerPassive)
{
	clear();
}


/**
 * Handler for paint event
 * @param pEvent pointer to event
 */
void CPipeWidget::paintEvent(QPaintEvent* /*pEvent*/)
{
	QPainter painter(this); 
	
	painter.drawImage(0, 0, *m_pBackground);
	
	/*
	//Debug
	QString dbg; // = QString(tr("%1 ")).arg(m_nWeight);
	if (m_nConnSide == CONNECT_UNDEF)	dbg += "-";
	if (m_nConnSide & CONNECT_UP)		dbg += "U";
	if (m_nConnSide & CONNECT_DOWN)		dbg += "D";
	if (m_nConnSide & CONNECT_LEFT)		dbg += "L";
	if (m_nConnSide & CONNECT_RIGHT)	dbg += "R";
	painter.drawText(3, 13, dbg);
	*/

	if (getType() == CPipeWidget::None)
		return;

	bool fRotateInProgress = rotateInProgress();
	
	//Line/hub painting
	if (fRotateInProgress) {
		if (m_fRotateClockWise)
			m_dRotateAngle += ROTATE_STEP;
		else
			m_dRotateAngle -= ROTATE_STEP;
	}
	else if (fabs(m_dRotateAngle) == 4 * ROTATE_ANGLE)	//Full circle
		m_dRotateAngle = 0.0;

	QImage img;
	switch (getType()) {
		case None: break;
		case Line:
			if (getConnSide() == (CONNECT_UP | CONNECT_DOWN) || getConnSide() == (CONNECT_RIGHT | CONNECT_LEFT))
				img = getConnFlag() && !fRotateInProgress ? *m_pLineActive : *m_pLinePassive;
			else
				img = getConnFlag() && !fRotateInProgress ? *m_pCornerActive : *m_pCornerPassive;
			break;
		case Hub:
			img = getConnFlag() && !fRotateInProgress ? *m_pHubActive : *m_pHubPassive;
			break;
		case Workstation:
		case Server:
			img = getConnFlag() && !fRotateInProgress ? *m_pHLineActive : *m_pHLinePassive;
			break;
	} 
	
	QMatrix matrix;
	matrix.rotate(m_dRotateAngle);
	img = img.transformed(matrix);
	painter.drawImage(0, 0, img);
	
	//Objects (server/workstations) painting
	if (getType() == Server)
		painter.drawImage(0, 0, *m_pServer);
	else if (getType() == Workstation)
		painter.drawImage(0, 0, getConnFlag() && !fRotateInProgress ? *m_pWksActive : *m_pWksPassive);
	
	if (fRotateInProgress) {
		if (!rotateInProgress()) {
			//End of rotate (last iteration) - send event about it to parent
			QEvent e(QEvent::User);
			parent()->event(&e);
		}
		update();
	}
}


/**
 * Handler for mouse press button event
 * @param pEvent pointer to event
 */
void CPipeWidget::mousePressEvent(QMouseEvent* pEvent)
{
	if (pEvent->button() == Qt::LeftButton || pEvent->button() == Qt::RightButton) {
		rotate(pEvent->button() == Qt::RightButton, true);
		update();
	}
}

/**
 * Rotate object
 * @param nByClockWise Direction flag
 * @param fShow show rotate flag
 */
void CPipeWidget::rotate(bool fByClockWise, bool fShow)
{
	if (rotateInProgress())
		return;	//Already in progress
		
	m_fRotateClockWise = fByClockWise;
	if (m_fRotateClockWise) {
		m_dRotateAngle += fShow ? ROTATE_STEP : ROTATE_ANGLE;
		m_nConnSide = (m_nConnSide << 1) | (m_nConnSide >> 3);
	}
	else {
		m_dRotateAngle -= fShow ? ROTATE_STEP : ROTATE_ANGLE;
		m_nConnSide = (m_nConnSide >> 1) | (m_nConnSide << 3);
	}
	m_nConnSide &= 0xF;
}


/**
 * Set initial angle by connected side status
 */
void CPipeWidget::setAngleBySide(void)
{
	switch (getType()) {
		case None: break;
		case Line:
			switch (m_nConnSide) {
				case (CONNECT_UP | CONNECT_DOWN):			m_dRotateAngle = 0 * ROTATE_ANGLE;	break;
				case (CONNECT_UP | CONNECT_RIGHT):			m_dRotateAngle = 0 * ROTATE_ANGLE;	break;
				case (CONNECT_RIGHT | CONNECT_DOWN):			m_dRotateAngle = 1 * ROTATE_ANGLE;	break;
				case (CONNECT_RIGHT | CONNECT_LEFT):			m_dRotateAngle = 1 * ROTATE_ANGLE;	break;
				case (CONNECT_DOWN | CONNECT_LEFT):			m_dRotateAngle = 2 * ROTATE_ANGLE;	break;
				case (CONNECT_LEFT | CONNECT_UP):			m_dRotateAngle = 3 * ROTATE_ANGLE;	break;
			}
			break;
		case Hub:
			switch (m_nConnSide) {
				case (CONNECT_UP | CONNECT_RIGHT | CONNECT_DOWN):	m_dRotateAngle = 0 * ROTATE_ANGLE;	break;
				case (CONNECT_RIGHT | CONNECT_DOWN | CONNECT_LEFT):	m_dRotateAngle = 1 * ROTATE_ANGLE;	break;
				case (CONNECT_DOWN | CONNECT_LEFT | CONNECT_UP):	m_dRotateAngle = 2 * ROTATE_ANGLE;	break;
				case (CONNECT_LEFT | CONNECT_UP | CONNECT_RIGHT):	m_dRotateAngle = 3 * ROTATE_ANGLE;	break;
			}
			break;
		case Workstation:
		case Server:
			switch (m_nConnSide) {
				case CONNECT_UP:					m_dRotateAngle = 0 * ROTATE_ANGLE;	break;
				case CONNECT_RIGHT:					m_dRotateAngle = 1 * ROTATE_ANGLE;	break;
				case CONNECT_DOWN:					m_dRotateAngle = 2 * ROTATE_ANGLE;	break;
				case CONNECT_LEFT:					m_dRotateAngle = 3 * ROTATE_ANGLE;	break;
			}
			break;
	}
}

