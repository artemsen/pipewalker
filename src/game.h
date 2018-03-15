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

#pragma once

#include "cell.h"


class CGame
{
public:
	//! Default constructor
	CGame(void);

	//! Default destructor
	~CGame();

	/**
	 * Initialize new random map
	 * @param nSize map size (map is [nSize * nSize])
	 * @param nReceiversNum number of receivers on map
	 * @param nMapId map ID (begin value for srand())
	 */
	void New(const unsigned short nSize = 0, const unsigned short nReceiversNum = 0, const unsigned int nMapId = 0);

	/**
	 * Reset current map
	 */
	void Reset(void);

	/**
	 * Get "game over" flag
	 * @return "game over" flag
	 */
	bool IsGameOver(void) const	{ return m_fGameOver; }

	/**
	 * Check cells status for "game over" (Define connection status for all receivers)
	 * @return "game over" flag
	 */
	bool CheckGameOver(void);

	/**
	 * Get object's cell
	 * @param nXPoint an X coordinate of cell
	 * @param nYPoint an Y coordinate of cell
	 * @return pointer to object cell
	 */
	CCell* GetObject(const int nXPoint, const int nYPoint);

	/**
	 * Rotate the tube
	 * @param pCell pointer to object
	 * @param enuDir rotate direction
	 */
	void RotateTube(CCell* pCell, const CCell::Direction enuDir);

	/**
	 * Event handle for finishing rotate of the tube
	 * @param pCell pointer to object
	 */
	void OnTubeRotated(CCell* pCell);

	/**
	 * Get current map size
	 * @return current map size
	 */
	unsigned short GetMapSize(void) const	{ return m_nMapSize; }

	/**
	 * Get current map ID
	 * @return current map ID
	 */
	unsigned int GetMapID(void) const		{ return m_nMapID; }


private:	//Class variables
	CCell* m_pMap;							///< Object's map
	bool m_fGameOver;						///< Game over flag
	unsigned int m_nXSndrPos, m_nYSndrPos;	///< Sender coordinates
	unsigned int m_nXZeroPos, m_nYZeroPos;	///< Zero point coordinates (sender out)
	unsigned int m_nMapID;					///< Map ID
	unsigned int m_nMapSize;				///< Map size
	unsigned int m_nReceiversNum;			///< Number of receivers on map

private:
	/**
	 * Create new map
	 */
	void CreateMap(void)	{ assert(m_pMap == NULL); m_pMap = new CCell[m_nMapSize * m_nMapSize]; }

	/**
	 * Destroy current map
	 */
	void DestroyMap(void)	{ if (m_pMap) { delete[] m_pMap; m_pMap = NULL; } }

	/**
	 * Install receiver on map
	 * @return result (false if unable to install)
	 */
	bool InstallReceiver(void);

	/**
	 * Install sender on map
	 */
	void InstallSender(void);

	/**
	 * Fill map by weight for path search
	 * @param nXPoint an X coordinate of start cell
	 * @param nYPoint an Y coordinate of start cell
	 * @param nWeight minimal weight of cell
	 */
	void FillMapWeight(int nXPoint = 0, int nYPoint = 0, int nWeight = 1);

	/**
	 * Test and make route from receiver to sender
	 * @param nXPoint X coordinate of cell
	 * @param nYPoint Y coordinate of cell
	 * @param nWeight maximal/minimal weight
	 * @param fUseMaxRoute use max route algorithm at first
	 * @return result (true = route found)
	 */
	bool MakeRoute(unsigned int nXPoint, unsigned int nYPoint, int nWeight, bool fUseMaxRoute);

	/**
	 * Define connection status
	 * @param nXPoint X coordinates of latest connected cell
	 * @param nYPoint Y coordinates of latest connected cell
	 */
	void DefineConnectStatus(int nXPoint = -1, int nYPoint = -1);

};
