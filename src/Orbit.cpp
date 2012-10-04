// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Orbit.h"

vector3d Orbit::OrbitalPosAtTime(double t) const
{
	const double e = m_eccentricity;
	const double M_t0 = m_meanAnomalyT0;
	const double M = (2.0*M_PI) * t / period + M_t0; // mean anomaly

	// eccentric anomaly
	// NR method to solve for E: M = E-sin(E)
	double E = M;
	for (int iter=5; iter > 0; --iter) {
		E = E - (E-e*(sin(E))-M) / (1.0 - e*cos(E));
	}

	// distance from primary
	const double r = m_semiMajorAxis * (1.0 - e*cos(E));
	// true anomaly (angle of orbit position)
	const double cos_v = (cos(E) - e) / (1.0 - e*cos(E));
	const double sin_v = (sqrt(1.0-e*e)*sin(E))/ (1.0 - e*cos(E));

	return m_orient * vector3d(-cos_v*r, sin_v*r, 0);
}

// used for stepping through the orbit in small fractions
// therefore the orbital phase at game start (mean anomalty at t = 0)
// does not need to be taken into account
vector3d Orbit::EvenSpacedPosAtTime(double t) const
{
	const double e = m_eccentricity;
	const double M = (2.0*M_PI) * t;
	const double v = 2.0*atan(sqrt((1+e)/(1-e)) * tan(M/2.0));
	const double r = m_semiMajorAxis * (1 - e*e) / (1 + e*cos(v));
	return m_orient * vector3d(-cos(v)*r, sin(v)*r, 0);
}

void Orbit::SetOrientation(double inclination, double longitude_at_ascending_node, double argument_of_periapsis)
{
	m_orient = matrix4x4d::RotateXMatrix(-0.5*M_PI + inclination);
	if (!is_zero_general(longitude_at_ascending_node))
		m_orient = matrix4x4d::RotateYMatrix(longitude_at_ascending_node) * m_orient;
	if (!is_zero_general(argument_of_periapsis))
		m_orient = matrix4x4d::RotateZMatrix(argument_of_periapsis) * m_orient;
}
