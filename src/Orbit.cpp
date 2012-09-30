// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Orbit.h"

vector3d Orbit::OrbitalPosAtTime(double t) const
{
	const double e = eccentricity;
	const double M_t0 = Orbit::orbitalPhaseAtStart; // mean anomaly at t = 0
	const double M = 2.0*M_PI*t / period + M_t0; // mean anomaly
	// eccentric anomaly
	// NR method to solve for E: M = E-sin(E)
	double E = M;
	for (int iter=5; iter > 0; --iter) {
		E = E - (E-e*(sin(E))-M) / (1.0 - e*cos(E));
	}
	// heliocentric distance
	double r = semiMajorAxis * (1.0 - e*cos(E));
	// true anomaly (angle of orbit position)
	double cos_v = (cos(E) - e) / (1.0 - e*cos(E));
	double sin_v = (sqrt(1.0-e*e)*sin(E))/ (1.0 - e*cos(E));

	vector3d pos = vector3d(-cos_v*r, sin_v*r, 0);
	pos = rotMatrix * pos;
	return pos;
}

// used for stepping through the orbit in small fractions
// therefore the orbital phase at game start (mean anomalty at t = 0)
// does not need to be taken into account
vector3d Orbit::EvenSpacedPosAtTime(double t) const
{
	const double e = eccentricity;
	const double M = 2*M_PI*t;
	const double v = 2*atan(sqrt((1+e)/(1-e)) * tan(M/2.0));
	const double r = semiMajorAxis * (1 - e*e) / (1 + e*cos(v));
	vector3d pos = vector3d(-cos(v)*r, sin(v)*r, 0);
	pos = rotMatrix * pos;
	return pos;
}
