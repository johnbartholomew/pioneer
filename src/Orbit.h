#ifndef ORBIT_H
#define ORBIT_H

#include <vector3.h>
#include <matrix4x4.h>

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
