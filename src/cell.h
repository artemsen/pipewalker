/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
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


class CCell
{
public:
	//! Default constructor
	CCell();

	//! Tube type
	enum TubeType {
		TTNone,			///< Free cell
		TTHalf,			///< Half tube (sender or received installed)
		TTStraight,		///< Straight tube
		TTCurved,		///< Curved tube
		TTJoiner		///< Joiner (T)
	};

	//! Cell type
	enum CellType {
		CTFree,			///< Free cell
		CTTube,			///< Tube
		CTSender,		///< Sender
		CTReceiver		///< Receiver
	};

	//! Connection sides
	enum ConnectionSide {
		CSTop,			///< Top
		CSBottom,		///< Bottom
		CSLeft,			///< Left
		CSRight			///< Right
	};

public:	//Helper functions
	/**
	 * Reset cell state
	 */
	void Reset();

	/**
	 * Save cell state as text
	 * \return cell description
	 */
	string Save() const;

	/**
	 * Load cell state from text
	 * \param state cell description
	 * \return false if load failed
	 */
	bool Load(const string& state);

	/**
	 * Reverse lock state of the cell
	 */
	inline void ReverseLock()					{ _Lock = !_Lock; }

	/**
	 * Get current lock state of the cell
	 * \return true if cell is locked
	 */
	inline bool IsLocked() const				{ return _Lock; }

	/**
	 * Get tube type of the cell
	 * \return tube type
	 */
	inline TubeType GetTubeType() const			{ assert(_CellType != CTFree); return _TubeType; }

	/**
	 * Get cell type of the cell
	 * \return cell type
	 */
	inline CellType GetCellType() const			{ return _CellType; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsTopConnected() const			{ return _ConnTop; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsBottomConnected() const		{ return _ConnBottom; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsLeftConnected() const			{ return _ConnLeft; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsRightConnected() const		{ return _ConnRight; }

	/**
	 * Set Sender cell type of the cell
	 */
	inline void SetAsSender()					{ assert(_CellType == CTFree); _CellType = CTSender; }

	/**
	 * Set Receiver cell type of the cell
	 */
	inline void SetAsReceiver()					{ assert(_CellType == CTFree); _CellType = CTReceiver; }

	/**
	 * Get status of the cell
	 * \return cell status (true if connected/active)
	 */
	inline bool IsActive() const				{ return _State; }

	/**
	 * Set status of the cell
	 * \param type cell status (true if connected/active)
	 */
	inline void SetActive(const bool type)		{ _State = type; }

	/**
	 * Get current angle of the cell
	 * \return current angle of the cell in degrees
	 */
	inline float GetAngle() const				{ return _Angle; }

	/**
	 * Add tube to cell
	 * \param side side of the added tube
	 */
	void AddTube(const ConnectionSide side);

	/**
	 * Check for add tube possibility
	 * \return true if tube can be added
	 */
	bool CanAddTube() const;

	/**
	 * Get current use flag of the cell
	 * \return current use flag of the cell
	 */
	bool GetUsed() const						{ return _Used; }

	/**
	 * Set current use flag of the cell
	 * \param weight a new use flag of the cell
	 */
	void SetUsed(const bool used)				{ _Used = used; }

	/**
	 * Start rotation
	 * \param dir rotation direction (true = clockwise)
	 */
	void Rotate(const bool dir);

	/**
	 * Check if rotation in progress
	 * \return true if rotation in progress
	 */
	inline bool IsRotationInProgress() const	{ return _Rotate.StartTime != 0; }

	/**
	 * Calculate rotation angle
	 * \return true if rotation has been finished
	 */
	bool ProcessRotation();

private:
	/**
	 * Get connected side count
	 * \return connected side count
	 */
	unsigned char GetTubeSideCount() const;

private:	//Class variables
	TubeType		_TubeType;			///< Tube type
	CellType		_CellType;			///< Cell type
	float			_Angle;				///< Angle
	bool			_State;				///< Cell connection state (true = active, false = passive)
	bool			_Lock;				///< Cell lock status
	bool			_ConnTop;			///< Top connection state
	bool			_ConnBottom;		///< Bottom connection state
	bool			_ConnLeft;			///< Left connection state
	bool			_ConnRight;			///< Right connection state
	bool			_Used;				///< Cell route use flag

	//Rotate description
	struct TUBE_ROTATION {
		TUBE_ROTATION() : StartTime(0) {}
		bool			Direction;		///< Rotate direction
		unsigned int	StartTime;		///< Rotate start time
		bool			Twice;			///< Rotate twice flag (180 degree)
		float			InitAngle;		///< Init rotate angle
		float			NeedAngle;		///< Needed rotate angle
	} _Rotate;
};
