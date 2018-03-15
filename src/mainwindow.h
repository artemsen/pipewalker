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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "pipewalker.h"
#include "pipewidget.h"


/**
	@author Artem A. Senichev <artemsen@gmail.com>
*/

#define HORIZONTAL_MARGIN	10	//Margin for left and rigth fields
#define VERTICAL_MARGIN		10	//Margin for top and bottom fields


class CMainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		CMainWindow();

	protected:
		void customEvent(QEvent* pEvent);

	private slots:
		void setSoundFlag(bool fChecked) { Settings.Sound = fChecked; }
		void newRndGameEvent(void);
		void resetGameEvent(void);
		void aboutEvent(void);
		void changeSkinEvent(int nSkinId);

	private:
		//Load images
		bool loadImages(int nSkinId = 0);
		bool loadImage(const QString& strFileName, QImage* pNormal, QImage* pGrayscale = 0);

	private:
		QAction*	m_pActionNewRandom;
		QAction*	m_pActionReset;
		QAction*	m_pActionUseSound;
		QAction*	m_pActionAbout;
		
		QToolBar*		m_pToolBar;
		QComboBox*		m_pSkinComboBox;
		
		QDialog*		m_pCongratulationDlg;
		
		CPipeWalker		m_objPipe;
		QVector<CPipeWidget*>	m_vPipeWidgets;
		QSound*			m_pClatzSound;
		
		//Images
		struct {
			QImage  Background;
			QImage  Server;
			QImage  WksActive;
			QImage  WksPassive;
			QImage  HubActive;
			QImage  HubPassive;
			QImage  HLineActive;
			QImage  HLinePassive;
			QImage  LineActive;
			QImage  LinePassive;
			QImage  CornerActive;
			QImage  CornerPassive;
			int     size;
		} Images;
		
		//Settings
		struct {
			bool Sound;
			unsigned int MapWidth;
			unsigned int MapHeight;
			unsigned int WksNum;
		} Settings;
	
};

#endif
