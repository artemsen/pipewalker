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


//Connected sides
#define CONNECT_UNDEF	0x0
#define CONNECT_UP		0x1
#define CONNECT_RIGHT	0x2
#define CONNECT_DOWN	0x4
#define CONNECT_LEFT	0x8

class CCell
{
public:
	//! Default constructor
	CCell();
	//! Default destructor
	~CCell();

	//! Object type enum
	enum Type {
		ObjNone,		///< Free cell
		ObjStrTube,		///< straight tube
		ObjCurTube,		///< curved tube
		ObjTubeJoint,	///< tube joint
		ObjSender,		///< sender (server)
		ObjReceiver		///< receiver (client)
	};

	Type		Type;				///< Object type
	bool		State;				///< Object state (true = active, false = passive)
	int			ConnSide;			///< Connection side
	float		RotateAngle;		///< Rotate angle
	float		RotateAngleEnd;		///< Finishing rotate angle (for smooth rotate)
	float		RotateCurr;			///< Current angle of rotate (for shadow draw)
	unsigned long RotateStart;		///< Rotate start time
	bool		RotateTwice;		///< Rotate twice (180 angle)
	int			Weight;				///< Cell's weight
	bool		Used;				///< Flag 'Used' for route calculate

	/**
	 * Reset object
	 */
	void Reset();

	/**
	 * Calculate rotation angle by connected side
	 */
	void SetAngleBySide();

	/**
	 * Calculate connection side by rotate
	 * @param fClockwise diretion if rotate
	 */
	void SetConnectionSideByRotate(bool fClockwise);


	/**
	 * Start tube rotation
	 * @param fClockwise a direction flag of rotate
	 * @param fTwice twice direction flag of rotate
	 */
	void StartRotate(bool fClockwise, bool fTwice);
};
