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

//! Cell state description (used to load/save map)
// struct CellState {
// 
// };

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
	inline void ReverseLock()					{ m_Lock = !m_Lock; }

	/**
	 * Get current lock state of the cell
	 * \return true if cell is locked
	 */
	inline bool IsLocked() const				{ return m_Lock; }

	/**
	 * Get tube type of the cell
	 * \return tube type
	 */
	inline TubeType GetTubeType() const			{ assert(m_CellType != CTFree); return m_TubeType; }

	/**
	 * Get cell type of the cell
	 * \return cell type
	 */
	inline CellType GetCellType() const			{ return m_CellType; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsTopConnected() const			{ return m_ConnTop; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsBottomConnected() const		{ return m_ConnBottom; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsLeftConnected() const			{ return m_ConnLeft; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool IsRightConnected() const		{ return m_ConnRight; }

	/**
	 * Set Sender cell type of the cell
	 */
	inline void SetAsSender()					{ assert(m_CellType == CTFree); m_CellType = CTSender; }

	/**
	 * Set Receiver cell type of the cell
	 */
	inline void SetAsReceiver()					{ assert(m_CellType == CTFree); m_CellType = CTReceiver; }

	/**
	 * Get status of the cell
	 * \return cell status (true if connected/active)
	 */
	inline bool IsActive() const				{ return m_State; }

	/**
	 * Set status of the cell
	 * \param type cell status (true if connected/active)
	 */
	inline void SetActive(const bool type)		{ m_State = type; }

	/**
	 * Get current angle of the cell
	 * \return current angle of the cell in degrees
	 */
	inline float GetAngle() const				{ return m_Angle; }

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
	bool GetUsed() const						{ return m_Used; }

	/**
	 * Set current use flag of the cell
	 * \param weight a new use flag of the cell
	 */
	void SetUsed(const bool used)				{ m_Used = used; }

	/**
	 * Start rotation
	 * \param dir rotation direction (true = clockwise)
	 */
	void Rotate(const bool dir);

	/**
	 * Check if rotation in progress
	 * \return true if rotation in progress
	 */
	inline bool IsRotationInProgress() const	{ return m_Rotate.StartTime != 0; }

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
	TubeType		m_TubeType;			///< Tube type
	CellType		m_CellType;			///< Cell type
	float			m_Angle;			///< Angle
	bool			m_State;			///< Cell connection state (true = active, false = passive)
	bool			m_Lock;				///< Cell lock status
	bool			m_ConnTop;			///< Top connection state
	bool			m_ConnBottom;		///< Bottom connection state
	bool			m_ConnLeft;			///< Left connection state
	bool			m_ConnRight;		///< Right connection state
	bool			m_Used;				///< Cell route use flag

	//Rotate description
	struct TUBE_ROTATION {
		TUBE_ROTATION() : StartTime(0) {}
		bool			Direction;		///< Rotate direction
		unsigned int	StartTime;		///< Rotate start time
		bool			Twice;			///< Rotate twice flag (180 degree)
		float			InitAngle;		///< Init rotate angle
		float			NeedAngle;		///< Needed rotate angle
	} m_Rotate;
};
