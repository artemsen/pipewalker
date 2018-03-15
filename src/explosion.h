#pragma once

#include "common.h"


class CExplosion
{
public:
	/**
	 * Constructor
	 * \param x an initial X explosion position
	 * \param y an initial Y explosion position
	 */
	CExplosion(const float x, const float y);

	/**
	 * Render explosion
	 */
	void Render();

private:
	/**
	 * Renew explosion
	 */
	void Renew();

private:
	//! Particles description
	struct PARTICLE {
		float	Life;		///< Life value
		float	FadeSpeed;	///< Fade speed
		float	PosX;		///< Position by X axis
		float	PosY;		///< Position by Y axis
		float	SpeedX;		///< Speed by X axis
		float	SpeedY;		///< Speed by Y axis
	};

	vector<PARTICLE>	m_Particles;	///< Particle Array
	float				m_X;			///< Initial X explosion position
	float				m_Y;			///< Initial Y explosion position
	float				m_Force;		///< Explosion force
};
