// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef ORBIT_H
#define ORBIT_H

#include "libs.h"
#include <vector3.h>
#include <matrix4x4.h>

class Orbit
{
public:
	Orbit() {}

	vector3d OrbitalPosAtTime(double t) const;
	vector3d EvenSpacedPosAtTime(double t) const;

	void SetShapeSemiMajor(double semi_major_axis, double eccentricity = 0.0)
	{
		m_semiMajorAxis = semi_major_axis;
		m_eccentricity = eccentricity;
	}

	void SetShapeApsides(double apoapsis, double periapsis)
	{
		const double e = (apoapsis - periapsis) / (apoapsis + periapsis);
		SetShapeSemiMajor((apoapsis + periapsis) * 0.5, e);
	}

	void SetOrientation(const matrix4x4d &orient);
	void SetOrientation(double inclination, double longitude_at_ascending_node = 0.0, double argument_of_periapsis = 0.0);

	void SetPeriod(double period) { m_period = period; }
	void SetPeriodFromPrimaryMass(double mass)
	{
		// mass in kg; assumes the secondary has negligible mass
		double a3 = m_semiMajorAxis * m_semiMajorAxis * m_semiMajorAxis;
		m_period = (2.0*M_PI) * sqrt(a3 / (G * mass));
	}

	void SetMeanAnomalyAtEpoch(double mean_anomaly) { m_meanAnomalyT0 = mean_anomaly; }

	// retrieve orbital parameters in various forms
	double GetPeriapsis() const { return m_semiMajorAxis * (1.0 - m_eccentricity); }
	double GetApoapsis() const { return m_semiMajorAxis * (1.0 + m_eccentricity); }
	double GetSemiMajorAxis() const { return m_semiMajorAxis; }
	double GetEccentricity() const { return m_eccentricity; }

	double GetPeriod() const { return m_period; }

	const matrix4x4d &GetOrientation() const { return m_orient; }

#if 0 // not implemented -- add these if you need them
	double GetInclination() const;
	double GetLongitudeAtAscendingNode() const;
	double GetArgumentOfPeriapsis() const;
#endif
private:
	// Shape of the orbit
	double m_eccentricity;
	double m_semiMajorAxis; // metres

	// Orbital phase offset
	double m_meanAnomalyT0; // mean anomaly at epoch

	// Minimal information about the primary
	double m_period; // seconds

	// Orientation of the orbit relative to the primary
	// given a point v on the orbit, specified in the X-Y plane,
	// with the primary at 0,0,0 and the major axis at
	// v' = m_orient * v; where v is in the X-Y plane and v' is in the reference frame of the primary)
	matrix4x4d m_orient;
};

#if 0
struct Orbit {
	Orbit(): orbitalPhaseAtStart(0.0) {};
	vector3d OrbitalPosAtTime(double t) const;
	// 0.0 <= t <= 1.0. Not for finding orbital pos
	vector3d EvenSpacedPosAtTime(double t) const;
	/* duplicated from SystemBody... should remove probably */
	double eccentricity;
	double semiMajorAxis;
	double orbitalPhaseAtStart; // 0 to 2 pi radians
	/* dup " " --------------------------------------- */
	double period; // seconds
	matrix4x4d rotMatrix;
};
#endif

#endif
