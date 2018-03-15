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

#include <QtXml>
#include <math.h>
#include "mainwindow.h"
#include "levels.h"


/**
 * Class constructor
 */
CMainWindow::CMainWindow() : QMainWindow()
{
	//Load settings
	loadSettings();
	
	//Menu forming
	QMenu* pMenuGame = menuBar()->addMenu(tr("&Game"));
	
	Menu.NewRandom = new QAction(QIcon(":/res/btn_new.png"), tr("&New random"), this);
	Menu.NewRandom->setShortcut(tr("Ctrl+N"));
	connect(Menu.NewRandom, SIGNAL(triggered()), this, SLOT(newRndGameEvent()));
	pMenuGame->addAction(Menu.NewRandom);

	Menu.LoadMap = new QAction(QIcon(":/res/btn_load.png"), tr("&Load map..."), this);
	Menu.LoadMap->setShortcut(tr("Ctrl+L"));
	connect(Menu.LoadMap, SIGNAL(triggered()), this, SLOT(loadMap()));
	pMenuGame->addAction(Menu.LoadMap);

	Menu.Reset = new QAction(QIcon(":/res/btn_reset.png"), tr("&Reset"), this);
	Menu.Reset->setShortcut(tr("Ctrl+R"));
	connect(Menu.Reset, SIGNAL(triggered()), this, SLOT(resetGameEvent()));
	pMenuGame->addAction(Menu.Reset);
	
	pMenuGame->addSeparator()->setText(tr("Game mode"));
	QActionGroup* pMenuGroupGameType = new QActionGroup(this);
	
	Menu.ModeRandom = new QAction(tr("&Random generated"), this);
	Menu.ModeRandom->setCheckable(true);
	Menu.ModeRandom->setChecked(false);
	pMenuGroupGameType->addAction(Menu.ModeRandom);
	
	Menu.ModeLevel = new QAction(tr("&Levels"), this);
	Menu.ModeLevel->setCheckable(true);
	Menu.ModeLevel->setChecked(true);
	pMenuGroupGameType->addAction(Menu.ModeLevel);
	
	Menu.ModeEdit = new QAction(tr("&Edit"), this);
	Menu.ModeEdit->setCheckable(true);
	Menu.ModeEdit->setChecked(false);
	pMenuGroupGameType->addAction(Menu.ModeEdit);
	
	pMenuGame->addAction(Menu.ModeRandom);
	pMenuGame->addAction(Menu.ModeLevel);
	pMenuGame->addAction(Menu.ModeEdit);
	connect(pMenuGroupGameType, SIGNAL(triggered(QAction*)), this, SLOT(setGameMode(QAction*)));

	pMenuGame->addSeparator()->setText(tr("Edit mode"));
	Menu.SaveMap = new QAction(QIcon(":/res/btn_save.png"), tr("&Save map as..."), this);
	pMenuGame->addAction(Menu.SaveMap);
	connect(Menu.SaveMap, SIGNAL(triggered()), this, SLOT(saveMap()));

	pMenuGame->addSeparator();
	QAction* pActionExit = new QAction(QIcon(":/res/btn_exit.png"), tr("E&xit"), this);
	pActionExit->setShortcut(tr("Ctrl+Q"));
	connect(pActionExit, SIGNAL(triggered()), this, SLOT(close()));
	pMenuGame->addAction(pActionExit);
	
	QMenu* pMenuOptions = menuBar()->addMenu(tr("&Options"));
	QAction* pActionSoundUse = new QAction(tr("Use &Sound"), this);
	pActionSoundUse->setCheckable(true); 
	pActionSoundUse->setChecked(Settings.Sound & QSound::isAvailable()); 
	pActionSoundUse->setEnabled(QSound::isAvailable()); 
	connect(pActionSoundUse, SIGNAL(toggled(bool)), this, SLOT(setSoundFlag(bool)));
	pMenuOptions->addAction(pActionSoundUse);
	
	QMenu* pThemesMenu = new QMenu(tr("Themes"), this);
	pThemesMenu->setEnabled(false);
	pMenuOptions->addMenu(pThemesMenu);
	QMenu* pLanguageMenu = new QMenu(tr("Language"), this);
	pLanguageMenu->setEnabled(false);
	pMenuOptions->addMenu(pLanguageMenu);
		
	QAction* pActionAbout = new QAction(tr("&About"), this);
	pActionAbout->setStatusTip(tr("About programm"));
	pActionAbout->setShortcut(tr("F1"));
	connect(pActionAbout, SIGNAL(triggered()), this, SLOT(aboutEvent()));

	QMenu* pMenuHelp = menuBar()->addMenu(tr("&?"));
	pMenuHelp->addAction(pActionAbout);
	
	m_pToolBar = addToolBar("");
	m_pToolBar->addAction(Menu.NewRandom);
	m_pToolBar->addAction(Menu.Reset);
	m_pToolBar->addSeparator();
	m_pLevelComboBox = new QComboBox(m_pToolBar);
	formingLevelComboBox();
	connect(m_pLevelComboBox, SIGNAL(activated(int)), this, SLOT(newLevelGameEvent(int)));
	m_pToolBar->addWidget(m_pLevelComboBox); 
	
	loadImages();
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
		int nYCoord = y * Images.size + menuBar()->height() + m_pToolBar->height() + VERTICAL_MARGIN;
		for (unsigned int x = 0; x < Settings.MapWidth; x++) {
			int nXCoord = x * Images.size + HORIZONTAL_MARGIN;
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
			p->move(nXCoord, nYCoord);
			p->resize(Images.size, Images.size);
			m_vPipeWidgets.push_back(p);
		}
	}
	
	setGameMode(Menu.ModeLevel);

	setWindowIcon(QIcon(":/res/mainicon.png"));
	
	//Resize window to map size
	resize(Settings.MapWidth * Images.size + HORIZONTAL_MARGIN * 2,
	       Settings.MapHeight * Images.size + VERTICAL_MARGIN * 2 + menuBar()->height() + m_pToolBar->height());

	//Start new game	
	newLevelGameEvent(Settings.CurrentLevel);
}


/**
 * Load program settings
 * @return true if settings loaded
 */
bool CMainWindow::loadSettings(void)
{
	//TODO game settings
	Settings.Sound = true;
	Settings.CurrentLevel = 0;
	Settings.MapWidth = 10;
	Settings.MapHeight = 10;
	Settings.WksNum =15;
	return true;
}


/**
 * Handler for game type change event
 * @param pEvent pointer to event
 */
void CMainWindow::setGameMode(QAction* pAction)
{
	if (pAction == Menu.ModeRandom) {
		newRndGameEvent();
		m_enuMode = Random;
	}
	else if (pAction == Menu.ModeLevel) {
		newLevelGameEvent(Settings.CurrentLevel);
		m_enuMode = Level;
	}
	else if (pAction == Menu.ModeEdit) {
		m_enuMode = Edit;
		setWindowTitle(tr("PipeWalker - Edit mode"));
	}
	m_pLevelComboBox->setEnabled(m_enuMode == Level);
	Menu.NewRandom->setEnabled(m_enuMode == Random);
	Menu.Reset->setEnabled(m_enuMode != Edit);
	Menu.SaveMap->setEnabled(m_enuMode == Edit);
	Menu.LoadMap->setEnabled(m_enuMode != Level);
	
	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end(); iter++)
		(*iter)->setEditMode(m_enuMode == Edit);
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
	
	if (m_enuMode == Edit)
		return;
	
	bool fAllWksConnected = true;
	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end() && fAllWksConnected; iter++) {
		if ((*iter)->rotateInProgress())
			fAllWksConnected = false;	//Undefined direction
		else if ((*iter)->getType() == CPipeWidget::Workstation)
			fAllWksConnected = (*iter)->getConnFlag();
	}
	
	if (fAllWksConnected) {
		m_pCongratulationDlg->show();
		if (m_enuMode == Level) {
			if (Settings.CurrentLevel < sizeof(PipeWalkerLevels) / sizeof(PipeWalkerLevels[0]) / (Settings.MapWidth * Settings.MapWidth) - 1) {
				Settings.CurrentLevel = m_pLevelComboBox->currentIndex() + 1;
				formingLevelComboBox();
				newLevelGameEvent(Settings.CurrentLevel);
			}
		}
	}

}


/**
 * Forming combobox
 */
void CMainWindow::formingLevelComboBox(void)
{
	m_pLevelComboBox->clear();
	
	QIcon pIconLockOn(":/res/lockon.png");
	QIcon pIconLockOff(":/res/lockoff.png");
	
	unsigned int nLevelsCount = sizeof(PipeWalkerLevels) / sizeof(PipeWalkerLevels[0]) / (Settings.MapWidth * Settings.MapWidth);
	for (unsigned int i = 0; i < nLevelsCount; i++)
		m_pLevelComboBox->addItem(i > Settings.CurrentLevel ? pIconLockOn : pIconLockOff, QString(tr("Level %1")).arg(i + 1));
	
	if (Settings.CurrentLevel < nLevelsCount)
		m_pLevelComboBox->setCurrentIndex(Settings.CurrentLevel);
}


/**
 * Load level description from file
 */
void CMainWindow::loadMap(void)
{
	//Get file name for load
	QString strFileName = QFileDialog::getOpenFileName(this, tr("Load level"), "", tr("XML Files (*.xml);;All Files (*)"));
	if (strFileName.isEmpty())
		return;

	QFile hFile(strFileName); 
	if (!hFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Failed to open file"), QString(tr("Failed to open file %1 for reading").arg(strFileName)));
		return;
	}

	
	QString strXMLErr;
	int nXMLErrLine, nXMLErrCol;
	
	QDomDocument xmlDoc; 
	if (!xmlDoc.setContent(&hFile, true, &strXMLErr, &nXMLErrLine, &nXMLErrCol)) { 
		QMessageBox::critical(this, tr("Failed to read file"),
				      QString(tr("Failed to read XML file %1:\nLine %2, Column %3: %4 ").arg(strFileName).arg(nXMLErrLine).arg(nXMLErrCol).arg(strXMLErr)));
		hFile.close();
		return;
	} 
  
	QDomElement xmlRoot = xmlDoc.documentElement(); 
	if (xmlRoot.tagName() != "PipeWalker") { 
		QMessageBox::critical(this, tr("Failed to read file"),
				      QString(tr("Failed to read level from file %1: unknown format (root node is %2 while expecting PipeWalker").arg(strFileName).arg(xmlRoot.tagName())));
		hFile.close();
		return;
	} 
  
	QDomNode xmlLevel = xmlRoot.firstChild(); 
	if (xmlLevel.toElement().tagName() != "Level") {
		QMessageBox::critical(this, tr("Failed to read file"),
				      QString(tr("Failed to read level from file %1: unknown format (child node is %2 while expecting Level").arg(strFileName).arg(xmlLevel.toElement().tagName())));
		hFile.close();
		return;
	}
		
	int nLevelDescLen = Settings.MapWidth * Settings.MapHeight * 3;
	
	
	QString strLevel = xmlLevel.toElement().text();
	if (strLevel.length() != nLevelDescLen) {
		QMessageBox::critical(this, tr("Failed to read file"),
				      QString(tr("Failed to read level from file %1: unknown format (length is %2 while expecting %3").arg(strFileName).arg(strLevel.length()).arg(nLevelDescLen)));
		hFile.close();
		return;
	}

	QVector<int> vMapDesc;
	for (int i = 0; i < nLevelDescLen; i += 3)
		vMapDesc.push_back(strLevel.mid(i, 3).toInt());
	loadLevel(vMapDesc);
	
	hFile.close();
}


/**
 * Save level description to file
 */
void CMainWindow::saveMap(void)
{
	//Get file name for save
	QString strFileName = QFileDialog::getSaveFileName(this, tr("Save level"), "", tr("XML Files (*.xml);;All Files (*)"));
	if (strFileName.isEmpty())
		return;

	//Forming level description
	QString strLevel;
	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end(); iter++) {
		int nCellDescr = (*iter)->getConnSide();
		nCellDescr = nCellDescr << 4;
		nCellDescr |= (int)(*iter)->getType();
		strLevel += QString("%1").arg(nCellDescr, 3, 10, QLatin1Char('0'));
	}

	
	//Forming XML
	QDomDocument xmlDoc; 
	QDomElement xmlRoot = xmlDoc.createElement("PipeWalker"); 
	QDomElement xmlLevel = xmlDoc.createElement("Level"); 
	QDomText xmlLevelDesc = xmlDoc.createTextNode(strLevel); 

	xmlDoc.appendChild(xmlRoot); 
	xmlRoot.appendChild(xmlLevel); 
	xmlLevel.appendChild(xmlLevelDesc); 
	
	/*
	//DEBUG////////////////////////////////////
	qWarning("Save debug is on!");
	QFile hFileDbg("dbg.txt"); 
	if (hFileDbg.open(QIODevice::WriteOnly | QIODevice::Text)) {
		for (unsigned int i = 0; i < Settings.MapWidth * Settings.MapHeight * 3; i += 3) {
			hFileDbg.write(QString("%1").arg(strLevel.mid(i, 3).toInt()).toAscii());
			hFileDbg.write(", ", 2);
		}
		hFileDbg.close();		
	}
	///////////////////////////////////////////
	*/

	//Save data
	QFile hFile(strFileName); 
	if (!hFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Failed to open file"), QString(tr("Failed to open file %1 for writing").arg(strFileName)));
		return;
	}

	QTextStream out(&hFile); 
	xmlDoc.save(out, 4);	//4 is indent
	
	hFile.close();
}


/**
 * Construct defined standart level
 * @param nLevel level number
 */
void CMainWindow::newLevelGameEvent(int nLevel)
{
	if ((unsigned int)nLevel > Settings.CurrentLevel ||
	    (unsigned int)nLevel >= sizeof(PipeWalkerLevels) / sizeof(PipeWalkerLevels[0]) / (Settings.MapWidth * Settings.MapWidth)) {
		m_pLevelComboBox->setCurrentIndex(Settings.CurrentLevel);
		return;
	}
	
	setWindowTitle(QString(tr("PipeWalker - Level %1")).arg(nLevel + 1));
	
	QVector<int> vLevelDesc;
	unsigned int nStartPos = nLevel * Settings.MapWidth * Settings.MapHeight;
	unsigned int nEndPos = nStartPos + Settings.MapWidth * Settings.MapHeight;
	for (unsigned int i = nStartPos; i < nEndPos; i++)
		vLevelDesc.push_back(PipeWalkerLevels[i]);
	
	loadLevel(vLevelDesc);
}


/**
 * Construct level from description
 * @param vLevelDesc level description
 */
void CMainWindow::loadLevel(const QVector<int>& vLevelDesc)
{
	m_objPipe.initializeCustom(m_vPipeWidgets, Settings.MapWidth, Settings.MapHeight);
		
	QVector<CPipeWidget*>::iterator iterWidget = m_vPipeWidgets.begin();
	
	for (QVector<int>::const_iterator iter = vLevelDesc.begin(); iter != vLevelDesc.end() && iterWidget != m_vPipeWidgets.end(); iter++) {
		(*iterWidget)->clear();
		
		int nCellDescr = *iter;
		(*iterWidget)->setType(CPipeWidget::Type(nCellDescr & 0xF));
		nCellDescr = nCellDescr >> 4;
		(*iterWidget)->setConnSide(nCellDescr);
		(*iterWidget)->setAngleBySide();
		
		iterWidget++;
	}
	
	m_objPipe.defineConnectStatus();
	resetGameEvent();
	
	update();
}


/**
 * Handler about event
 */
void CMainWindow::aboutEvent(void)
{
	QMessageBox::about(this, tr("About"),
			   tr("<center><h2>PipeWalker</h2><p>ver.0.1.2<p><i>Author: Artem A. Senichev</i></center>"));
}


/**
 * Handler for close program event
 * @param pEvent pointer to event
 */
void CMainWindow::closeEvent(QCloseEvent *pEvent)
{
	int nConfirm = QMessageBox::Yes; /*QMessageBox::question(
	this, tr("Confirm"),
	tr("Do you really wont to quit?"),
	QMessageBox::Yes | QMessageBox::No,
	QMessageBox::Yes,
	QMessageBox::Cancel | QMessageBox::Escape);*/
	if (nConfirm == QMessageBox::Yes)
		pEvent->accept();
	else
		pEvent->ignore();
}


/**
 * Handler for new game menu event
 */
void CMainWindow::newRndGameEvent(void)
{
	setWindowTitle(tr("PipeWalker - Random map"));
	
	//Clear map
	for (QVector<CPipeWidget*>::iterator iter = m_vPipeWidgets.begin(); iter != m_vPipeWidgets.end(); iter++)
		(*iter)->clear();

	if (!m_objPipe.initializeRandom(m_vPipeWidgets, Settings.MapWidth, Settings.MapHeight, Settings.WksNum))
		QMessageBox::warning(this, tr("Unable to create map"), tr("Sorry, unable to create map!"));		
	
	//Reset rotrate state
	resetGameEvent();
}


/**
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
 * @return true if all images loaded
 */
bool CMainWindow::loadImages(void)
{
	if (!loadImage(":/res/img/background.png", &Images.Background))
		return false;
	if (!loadImage(":/res/img/server.png", &Images.Server))
		return false;
	if (!loadImage(":/res/img/workstation.png", &Images.WksActive, &Images.WksPassive))
		return false;	
	if (!loadImage(":/res/img/hub.png", &Images.HubActive, &Images.HubPassive))
		return false;
	if (!loadImage(":/res/img/line.png", &Images.LineActive, &Images.LinePassive))
		return false;
	if (!loadImage(":/res/img/corner.png", &Images.CornerActive, &Images.CornerPassive))
		return false;
	if (!loadImage(":/res/img/hline.png", &Images.HLineActive, &Images.HLinePassive))
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

