#ifndef _SECTOR_H
#define _SECTOR_H

#include "libs.h"
#include <string>
#include <vector>
#include <map>
#include "StarSystem.h"
#include "DeleteEmitter.h"
#include "CustomSystem.h"

class Sector : public DeleteEmitter, public RefCounted {
public:

	class System {
	public:
		System() : m_queriedStarSystem(false), m_isInhabited(false) {};
		~System() {};

		// Check that we've had our habitation status set
		bool IsSetInhabited() const { return m_queriedStarSystem; }
		void SetInhabited(bool inhabited) { m_isInhabited = inhabited; m_queriedStarSystem = true; }
		bool IsInhabited() const { return m_isInhabited; }
	
		// public members
		std::string name;
		vector3f p;
		int numStars;
		SBody::BodyType starType[4];
		Uint32 seed;

		const CustomSystem *customSystem;

	private:
		bool m_queriedStarSystem;
		bool m_isInhabited;
	};

	// lightyears
	static const float SIZE;

	static RefCountedPtr<Sector> Get(const SystemPath &path);
	static RefCountedPtr<Sector> Get(int x, int y, int z);

	const std::vector<System> &GetSystems() const;
	const System &GetSystem(Uint32 n) const;
	Uint32 GetNumSystems() const;

	bool GetCustomOnly() const { return m_customOnly; }
	void SetCustomOnly(bool c) { m_customOnly = c; }

	void AddSystem(const CustomSystem *customSystem);

	static float DistanceBetween(const Sector *a, int sysIdxA, const Sector *b, int sysIdxB);
	// Sector is within a bounding rectangle - used for SectorView m_sectorCache pruning.
	bool WithinBox(const int Xmin, const int Xmax, const int Ymin, const int Ymax, const int Zmin, const int Zmax) const;

	const int sx, sy, sz;

private:
	Sector(int x, int y, int z);

	//void GetCustomSystems();
	std::string GenName(System &sys, MTRand &rand) const;

	void GenerateSystems() const;

	mutable std::vector<System> m_systems;
	mutable bool m_dirty;

	// systems that were added at startup
	std::vector<const CustomSystem*> m_startupCustomSystems;

	// systems that were added during the current game
	std::vector<const CustomSystem*> m_gameCustomSystems;

	// sector should only show custom systems
	bool m_customOnly;

	// sectors that were modified at startup
	static std::map<SystemPath,Sector*> s_startupCache;

	// sectors that have been modified in the current game
	static std::map<SystemPath,Sector*> s_gameCache;
};

#endif /* _SECTOR_H */
