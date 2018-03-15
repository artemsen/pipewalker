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
	 * @return result (false = unable to create map (to many workstation?))
	 */
	bool New(void);

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
	 * Event handle for finishing rotate of the tube
	 * @param pCell pointer to object
	 * @return true if rotation on progress (twice rotation is active)
	 */
	bool OnTubeRotated(CCell* pCell);

private:	//Class variables
	bool m_fGameOver;
	unsigned int m_nXSndrPos, m_nYSndrPos;			///< Sender coordinates
	unsigned int m_nXZeroPos, m_nYZeroPos;			///< Zero point coordinates (sender out)
	CCell m_vMap[MAP_WIDTH_NUM * MAP_HEIGHT_NUM];	///< Object's map

private:
	/**
	 * Reset current map
	 */
	void ResetMap(void);

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
	 * Test and make maximal route from receiver to sender
	 * @param nXPoint X coordinate of cell
	 * @param nYPoint Y coordinate of cell
	 * @param nWeight maximal weight
	 * @param fTestOnly test route flag (true = only check if route exist)
	 * @return result (true = route found)
	 */
	bool MakeMaxRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly);

	/**
	 * Test and make minimal route from receiver to sender
	 * @param nXPoint X coordinate of cell
	 * @param nYPoint Y coordinate of cell
	 * @param nWeight maximal weight
	 * @param fTestOnly test route flag (true = only check if route exist)
	 * @return result (true = route found)
	 */
	bool MakeMinRoute(int nXPoint, int nYPoint, int nWeight, bool fTestOnly);

	/**
	 * Set connection side status
	 * @param nPrevXPoint Previous X point
	 * @param nPrevYPoint Previous Y point
	 * @param nCurrXPoint Current X point
	 * @param nCurrYPoint Current Y point
	 */
	void SetConnectStatus(int nPrevXPoint, int nPrevYPoint, int nCurrXPoint, int nCurrYPoint);


	/**
	 * Define connection status
	 * @param nXPoint X coordinates of latest connected cell
	 * @param nYPoint Y coordinates of latest connected cell
	 */
	void DefineConnectStatus(int nXPoint = -1, int nYPoint = -1);

};
