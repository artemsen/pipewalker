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

#include "common.h"

#define MAX_WEIGHT		0xFFFF	///< Maximum weight of cell

//Connected sides
#define CONNECT_UNDEF	0x00
#define CONNECT_UP		0x01
#define CONNECT_RIGHT	0x02
#define CONNECT_DOWN	0x04
#define CONNECT_LEFT	0x08


class CCell
{
public:
	//! Default constructor
	CCell(void);

	//! Default destructor
	~CCell();

	//! Object type enum
	enum Type {
		Free,			///< free cell
		Tube,			///< tube
		Sender,			///< sender (server)
		Receiver		///< receiver (client)
	};

	//! Rotate direction
	enum Direction {
		Positive,		///< positive direction (from down to up/left to right/clockwise)
		Negative		///< negative direction (from up to down/right to left/anticlockwise)
	};

	Type			Type;			///< Object type
	bool			State;			///< Object state (true = active, false = passive)
	unsigned int	Sides;			///< Connection side

	//Used for calculate routes
	int				Weight;			///< Cell's weight
	bool			Used;			///< Flag 'Used' by current route

	//Rotate description
	struct Rotation {
		Direction		Dir;		///< Rotate direction
		unsigned long	StartTime;	///< Rotate start time
		bool			Twice;		///< Rotate twice flag (180 degree)
		float			Angle;		///< Current rotate angle
	} Rotate;


public:	//Helper functions
	
	//! Assignment operator
	CCell& operator=(const CCell& cell);

	/**
	 * Reset object state
	 */
	void Reset();

	/**
	 * Reverse lock state of the cell
	 */
	void ReverseLock(void)		{ m_fLock = !m_fLock; }

	/**
	 * Get current lock state of the cell
	 * @return true if cell is locked
	 */
	bool IsLocked(void) const	{ return m_fLock; }

	/**
	 * Add tube to cell
	 * @param unSide side of the added tube
	 * @return new connected sides flags
	 */
	inline unsigned int AddTube(const unsigned int unSide) { Sides |= unSide; return Sides; }

	/**
	 * Get connected side count
	 * @return connected side count
	 */
	unsigned short GetSideCount(void) const;

	/**
	 * Check for add tube
	 * @return true if tube can be added
	 */
	bool CanAddTube(void) const { return Type == Free || (Type == Tube && GetSideCount() < 3); }

	/**
	 * Check for curved tube
	 * @return true if tube is curved
	 */
	bool IsCurved(void) const;

	/**
	 * Start tube rotation
	 * @param enuDir direction of rotation
	 */
	void StartRotate(const Direction enuDir);
	
	/**
	 * Finish rotate handler
	 */
	void EndRotate(void);

	/**
	 * Check if rotation in progress
	 * @return true if rotation in progress
	 */
	inline bool IsRotationInProgress(void) const { return (Rotate.StartTime != 0); }
	
private:
	bool	m_fLock;				///< Cell lock status
};
