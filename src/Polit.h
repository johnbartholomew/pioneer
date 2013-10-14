// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _POLIT_H
#define _POLIT_H

#include "EquipType.h"
#include "Serializer.h"

class StarSystem;
class SysPolit;
class Ship;

namespace Polit {
	enum PolitEcon { // <enum scope='Polit' name=PolitEcon prefix=ECON_ public>
		ECON_NONE,
		ECON_VERY_CAPITALIST,
		ECON_CAPITALIST,
		ECON_MIXED,
		ECON_PLANNED,
		ECON_MAX // <enum skip>
	};

	enum GovType { // <enum scope='Polit' name=PolitGovType prefix=GOV_ public>
		GOV_INVALID, // <enum skip>
		GOV_NONE,
		GOV_EARTHCOLONIAL,
		GOV_EARTHDEMOC,
		GOV_EMPIRERULE,
		GOV_CISLIBDEM,
		GOV_CISSOCDEM,
		GOV_LIBDEM,
		GOV_CORPORATE,
		GOV_SOCDEM,
		GOV_EARTHMILDICT,
		GOV_MILDICT1,
		GOV_MILDICT2,
		GOV_EMPIREMILDICT,
		GOV_COMMUNIST,
		GOV_PLUTOCRATIC,
		GOV_DISORDER,
		GOV_MAX, // <enum skip>
		GOV_RAND_MIN = GOV_NONE+1, // <enum skip>
		GOV_RAND_MAX = GOV_MAX-1, // <enum skip>
	};

	void GetSysPolitStarSystem(const StarSystem *s, const fixed human_infestedness, SysPolit &outSysPolit);

	// These call out to Lua and should be removed in the future,
	// when there is no more core code that knows about fines.
	//
	// These functions take non-const SystemBody objects, because they
	// pass those objects to Lua, and LuaObject<T>::PushToLua requires
	// a non-const object.
	Sint64 PlayerGetFine(SystemBody *station);
	void PlayerPayFine(SystemBody *station, Sint64 fine);

	bool IsCommodityLegal(StarSystem *system, const Equip::Type t);
}

class SysPolit {
public:
	const char *GetGovernmentDesc() const;
	const char *GetEconomicDesc() const;

	Polit::GovType govType;
	fixed lawlessness;
};

#endif /* _POLIT_H */
