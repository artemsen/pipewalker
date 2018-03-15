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

#include <math.h>
#include "mainwindow.h"


/**
 * Class constructor
 */
CMainWindow::CMainWindow() : QMainWindow()
{
	Settings.MapWidth = 10;
	Settings.MapHeight = 10;
	Settings.WksNum = 25;

	//Toolbar forming
	m_pToolBar = addToolBar("");
	m_pToolBar->setMovable(false);
	
	m_pActionNewRandom = new QAction(QIcon(":/res/btn_new.png"), tr("&New"), this);
	m_pActionNewRandom->setShortcut(tr("Ctrl+N"));
	connect(m_pActionNewRandom, SIGNAL(triggered()), this, SLOT(newRndGameEvent()));
	
	m_pActionReset = new QAction(QIcon(":/res/btn_reset.png"), tr("&Reset"), this);
	m_pActionReset->setShortcut(tr("Ctrl+R"));
	connect(m_pActionReset, SIGNAL(triggered()), this, SLOT(resetGameEvent()));

	m_pActionUseSound = new QAction(QIcon(":/res/btn_sound.png"), tr("Use &Sound"), this);
	m_pActionUseSound->setCheckable(true); 
	m_pActionUseSound->setChecked(QSound::isAvailable()); 
	m_pActionUseSound->setEnabled(QSound::isAvailable() & false); //Not avialable :-(
	connect(m_pActionUseSound, SIGNAL(toggled(bool)), this, SLOT(setSoundFlag(bool)));
	
	m_pActionAbout = new QAction(QIcon(":/res/mainicon.png"), tr("&About"), this);
	m_pActionAbout->setStatusTip(tr("About programm"));
	m_pActionAbout->setShortcut(tr("F1"));
	connect(m_pActionAbout, SIGNAL(triggered()), this, SLOT(aboutEvent()));

	m_pSkinComboBox = new QComboBox(m_pToolBar);
	m_pSkinComboBox->addItem(QString(tr("Standart")));
	m_pSkinComboBox->addItem(QString(tr("Small")));
	connect(m_pSkinComboBox, SIGNAL(activated(int)), this, SLOT(changeSkinEvent(int)));
	
	m_pToolBar->addAction(m_pActionNewRandom);
	m_pToolBar->addAction(m_pActionReset);
	m_pToolBar->addSeparator();
	m_pToolBar->addAction(m_pActionUseSound);
	m_pToolBar->addWidget(m_pSkinComboBox); 
	m_pToolBar->addSeparator();
	m_pToolBar->addAction(m_pActionAbout);

	m_pClatzSound = new QSound(":/res/sound.wav");

	//Congratulation dialog
	m_pCongratulationDlg = new QDialog(this);
	QLabel* pCDlgLabel1 = new QLabel(tr("Congratulations!"), m_pCongratulationDlg);
	pCDlgLabel1->setGeometry(QRect(0, 0, 300, 20));
	pCDlgLabel1->setAlignment(Qt::AlignCenter);
	pCDlgLabel1->setFont(QFont("Times", 16, QFont::Bold, true));;
	QLabel* pCDlgLabel2 = new QLabel(tr("You solve it!"), m_pCongratulationDlg);
	pCDlgLabel2->setGeometry(QRect(0, 20, 300, 20));
	pCDlgLabel2->setAlignment(Qt::AlignCenter);
	pCDlgLabel2->setFont(QFont("Times", 12, QFont::Bold, true));;
	QPushButton* pCDlgBtn = new QPushButton(tr("OK"), m_pCongratulationDlg);
	pCDlgBtn->setGeometry(QRect(50, 40, 200, 20));
	connect(pCDlgBtn, SIGNAL(clicked()), m_pCongratulationDlg, SLOT(close()));
	m_pCongratulationDlg->setWindowTitle(tr("Congratulation!"));
	m_pCongratulationDlg->setModal(true);
	
	//Create new widget map
	for (unsigned int y = 0; y < Settings.MapHeight; y++) {
		for (unsigned int x = 0; x < Settings.MapWidth; x++) {
			CPipeWidget* p = new CPipeWidget(this,
					&Images.Background, 
					&Images.LineActive,
					&Images.LinePassive,
					&Images.Server,
					&Images.WksActive,
					&Images.WksPassive,
					&Images.HubActive,
					&Images.HubPassive,
					&Images.HLineActive,
					&Images.HLinePassive,
					&Images.CornerActive,
					&Images.CornerPassive);
			m_vPipeWidgets.push_back(p);
		}
	}
	
	//Set Standart theme by default
	changeSkinEvent(0);
	
	setWindowIcon(QIcon(":/res/mainicon.png"));
	setWindowTitle(tr("PipeWalker"));
	
	//Start new game	
	newRndGameEvent();
}


/**
 * Handler custom event
 * @param pEvent pointer to event
 */
void CMainWindow::customEvent(QEvent* /*pEvent*/)
{
	if (Settings.Sound)
		m_pClatzSound->play();
	
	//Define connect status
	m_objPipe.defineConnectStatus();
	update();
	
	bool fAllWksConnected = true;
	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end() && fAllWksConnected; iter++) {
		if ((*iter)->rotateInProgress())
			fAllWksConnected = false;	//Undefined direction
		else if ((*iter)->getType() == CPipeWidget::Workstation)
			fAllWksConnected = (*iter)->getConnFlag();
	}
	
	if (fAllWksConnected)
		m_pCongratulationDlg->show();
}


/**
 * Handler change skin event
 */
void CMainWindow::changeSkinEvent(int nSkinId)
{
	loadImages(nSkinId);
	
	//Move and resize widgets
	for (unsigned int y = 0; y < Settings.MapHeight; y++) {
		int nYCoord = y * Images.size + m_pToolBar->height() + VERTICAL_MARGIN;
		for (unsigned int x = 0; x < Settings.MapWidth; x++) {
			int nXCoord = x * Images.size + HORIZONTAL_MARGIN;
			CPipeWidget* pWidget = m_vPipeWidgets.at(x + y * Settings.MapHeight);
			pWidget->move(nXCoord, nYCoord);
			pWidget->resize(Images.size, Images.size);
		}
	}

	//Resize main window to map size
	resize(Settings.MapWidth * Images.size + HORIZONTAL_MARGIN * 2,
	       Settings.MapHeight * Images.size + VERTICAL_MARGIN * 2 + m_pToolBar->height());
	update();
}


/**
 * Handler about event
 */
void CMainWindow::aboutEvent(void)
{
	QMessageBox::about(this, tr("About PipeWalker"), 
			   tr(
				"<center>"
				"<h1>PipeWalker</h1>"
				"<b>ver.0.2.1</b><br>"
				"Author: Artem A. Senichev<br>"
				"<a href=\"http://sourceforge.net/projects/pipewalker\">http://sourceforge.net/projects/pipewalker</a>"
				"</center>"
			     ));
}


/**
 * Handler for new game menu event
 */
void CMainWindow::newRndGameEvent(void)
{
	//Clear map
	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end(); iter++)
		(*iter)->clear();

	if (!m_objPipe.initializeRandom(m_vPipeWidgets, Settings.MapWidth, Settings.MapHeight, Settings.WksNum))
		QMessageBox::warning(this, tr("Unable to create map"), tr("Sorry, unable to create map!"));		
	
	//Reset rotrate state
	resetGameEvent();
}


/**pEvent
 * Handler for reset game menu event
 */
void CMainWindow::resetGameEvent(void)
{
	/*
	m_objPipe.defineConnectStatus();
	update();
	return;
	*/

	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end(); iter++)
		for (int r = 0; r < m_objPipe.getRandom(3); r++)
			(*iter)->rotate(m_objPipe.getRandom(2) - 1, false);
	m_objPipe.defineConnectStatus();
	update();
}


/**
 * Load images
 * @param nSkinId skin id
 * @return true if all images loaded
 */
bool CMainWindow::loadImages(int nSkinId /*= 0*/)
{
	char* pszImgBkg;
	char* pszImgSrv;
	char* pszImgWks;
	char* pszImgHub;
	char* pszImgLine;
	char* pszImgCorner;
	char* pszImgHLine;
	
	switch (nSkinId) {
		case 1:		//Small theme
			pszImgBkg =    ":/res/skin_small/background.png";
			pszImgSrv =    ":/res/skin_small/server.png";
			pszImgWks =    ":/res/skin_small/workstation.png";
			pszImgHub =    ":/res/skin_small/hub.png";
			pszImgLine =   ":/res/skin_small/line.png";
			pszImgCorner = ":/res/skin_small/corner.png";
			pszImgHLine =  ":/res/skin_small/hline.png";
			break;
		default:	//Standart theme
			pszImgBkg =    ":/res/skin_std/background.png";
			pszImgSrv =    ":/res/skin_std/server.png";
			pszImgWks =    ":/res/skin_std/workstation.png";
			pszImgHub =    ":/res/skin_std/hub.png";
			pszImgLine =   ":/res/skin_std/line.png";
			pszImgCorner = ":/res/skin_std/corner.png";
			pszImgHLine =  ":/res/skin_std/hline.png";
			break;
	}
	
	if (!loadImage(pszImgBkg, &Images.Background))
		return false;
	if (!loadImage(pszImgSrv, &Images.Server))
		return false;
	if (!loadImage(pszImgWks, &Images.WksActive, &Images.WksPassive))
		return false;	
	if (!loadImage(pszImgHub, &Images.HubActive, &Images.HubPassive))
		return false;
	if (!loadImage(pszImgLine, &Images.LineActive, &Images.LinePassive))
		return false;
	if (!loadImage(pszImgCorner, &Images.CornerActive, &Images.CornerPassive))
		return false;
	if (!loadImage(pszImgHLine, &Images.HLineActive, &Images.HLinePassive))
		return false;

	Images.size = Images.Background.width();
	
	return true;
}


/**
 * Load image from file or resource
 * @param strFileName file name
 * @param pNormal pointer to normal image
 * @param pGrayscale pointer to grayscale image
 * @return true if image loaded
 */
bool CMainWindow::loadImage(const QString& strName, QImage* pNormal, QImage* pGrayscale /*= 0*/)
{
	if (!pNormal->load(strName)) {
		QMessageBox::critical(this, tr("Failed to load image"), QString(tr("Failed to load image from %1").arg(strName)));
		return false;
	}
	if (pGrayscale) {
		//Convert to grayscale
		*pGrayscale = pNormal->convertToFormat(QImage::Format_ARGB32);

		for (int y = 0; y < pGrayscale->height(); y++) {
			for (int x = 0; x < pGrayscale->width(); x++) {
				int pixel = pGrayscale->pixel(x, y);
				int gray = qGray(pixel);
				int alpha = qAlpha(pixel);
				pGrayscale->setPixel(x, y, qRgba(gray, gray, gray, alpha));
			}
		}
	}
	return true;
}

