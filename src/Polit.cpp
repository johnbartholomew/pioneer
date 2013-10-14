// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "libs.h"
#include "Pi.h"
#include "Polit.h"
#include "galaxy/StarSystem.h"
#include "galaxy/Sector.h"
#include "Factions.h"
#include "Space.h"
#include "Ship.h"
#include "ShipCpanel.h"
#include "SpaceStation.h"
#include "EquipType.h"
#include "Lang.h"
#include "StringF.h"
#include "Game.h"
#include "LuaManager.h"
#include "EnumStrings.h"

namespace Polit {

static const Uint32 POLIT_SEED = 0x1234abcd;
static const Uint32 POLIT_SALT = 0x8732abdf;

const char *s_econDesc[ECON_MAX] = {
	Lang::NO_ESTABLISHED_ORDER,
	Lang::HARD_CAPITALIST,
	Lang::CAPITALIST,
	Lang::MIXED_ECONOMY,
	Lang::PLANNED_ECONOMY
};

struct politDesc_t {
	const char *description;
	int rarity;
	PolitEcon econ;
	fixed baseLawlessness;
};
static politDesc_t s_govDesc[GOV_MAX] = {
	{ "<invalid turd>",							0,		ECON_NONE,				fixed(1,1) },
	{ Lang::NO_CENTRAL_GOVERNANCE,				0,		ECON_NONE,				fixed(1,1) },
	{ Lang::EARTH_FEDERATION_COLONIAL_RULE,		2,		ECON_CAPITALIST,		fixed(3,10) },
	{ Lang::EARTH_FEDERATION_DEMOCRACY,			3,		ECON_CAPITALIST,		fixed(15,100) },
	{ Lang::IMPERIAL_RULE,						3,		ECON_PLANNED,			fixed(15,100) },
	{ Lang::LIBERAL_DEMOCRACY,					2,		ECON_CAPITALIST,		fixed(25,100) },
	{ Lang::SOCIAL_DEMOCRACY,					2,		ECON_MIXED,				fixed(20,100) },
	{ Lang::LIBERAL_DEMOCRACY,					2,		ECON_CAPITALIST,		fixed(25,100) },
	{ Lang::CORPORATE_SYSTEM,					2,		ECON_CAPITALIST,		fixed(40,100) },
	{ Lang::SOCIAL_DEMOCRACY,					2,		ECON_MIXED,				fixed(25,100) },
	{ Lang::MILITARY_DICTATORSHIP,				5,		ECON_CAPITALIST,		fixed(40,100) },
	{ Lang::MILITARY_DICTATORSHIP,				6,		ECON_CAPITALIST,		fixed(25,100) },
	{ Lang::MILITARY_DICTATORSHIP,				6,		ECON_MIXED,				fixed(25,100) },
	{ Lang::MILITARY_DICTATORSHIP,				5,		ECON_MIXED,				fixed(40,100) },
	{ Lang::COMMUNIST,							10,		ECON_PLANNED,			fixed(25,100) },
	{ Lang::PLUTOCRATIC_DICTATORSHIP,			4,		ECON_VERY_CAPITALIST,	fixed(45,100) },
	{ Lang::VIOLENT_ANARCHY,					2,		ECON_NONE,				fixed(90,100) },
};


void GetSysPolitStarSystem(const StarSystem *s, const fixed human_infestedness, SysPolit &outSysPolit)
{
	SystemPath path = s->GetPath();
	const Uint32 _init[5] = { Uint32(path.sectorX), Uint32(path.sectorY), Uint32(path.sectorZ), path.systemIndex, POLIT_SEED };
	Random rand(_init, 5);

	Sector sec(path.sectorX, path.sectorY, path.sectorZ);

	GovType a = GOV_INVALID;

	/* from custom system definition */
	if (sec.m_systems[path.systemIndex].customSys) {
		Polit::GovType t = sec.m_systems[path.systemIndex].customSys->govType;
		a = t;
	}
	if (a == GOV_INVALID) {
		if (path == SystemPath(0,0,0,0)) {
			a = Polit::GOV_EARTHDEMOC;
		} else if (human_infestedness > 0) {
			// attempt to get the government type from the faction
			a = s->GetFaction()->PickGovType(rand);

			// if that fails, either no faction or a faction with no gov types, then pick something at random
			if (a == GOV_INVALID) {
				a = static_cast<GovType>(rand.Int32(GOV_RAND_MIN, GOV_RAND_MAX));
			}
		} else {
			a = GOV_NONE;
		}
	}

	outSysPolit.govType = a;
	outSysPolit.lawlessness = s_govDesc[a].baseLawlessness * rand.Fixed();
}

Sint64 PlayerGetFine(SystemBody *station)
{
	assert(station && station->GetSuperType() == SystemBody::SUPERTYPE_STARPORT);

	Sint64 fine = 0;

	lua_State *l = Lua::manager->GetLuaState();
	int pristine_stack = lua_gettop(l);

	pi_lua_import(l, "Legal");
	if (!lua_isnil(l, -1)) { lua_getfield(l, -1, "GetFine"); }

	if (lua_isfunction(l, -1)) {
		pi_lua_import(l, "Character");
		lua_getfield(l, -1, "persistent");
		lua_remove(l, -2); // remove the 'Character' table
		lua_getfield(l, -1, "player");
		lua_remove(l, -2); // remove the 'persistent' table

		LuaObject<SystemBody>::PushToLua(station);

		lua_call(l, 2, 1); // call Legal.GetFine(player, station)
		if (lua_isnumber(l, -1)) {
			fine = lua_tonumber(l, -1) * 100.0;
		}
	} else {
		fprintf(stderr, "Legal.GetCharacterFine doesn't exist or isn't a function\n");
	}

	lua_settop(l, pristine_stack);
	return fine;
}

void PlayerPayFine(SystemBody *station, Sint64 payment)
{
	assert(station && station->GetSuperType() == SystemBody::SUPERTYPE_STARPORT);

	lua_State *l = Lua::manager->GetLuaState();
	int pristine_stack = lua_gettop(l);

	pi_lua_import(l, "Legal");
	if (!lua_isnil(l, -1)) { lua_getfield(l, -1, "PayFine"); }

	if (lua_isfunction(l, -1)) {
		pi_lua_import(l, "Character");
		lua_getfield(l, -1, "persistent");
		lua_remove(l, -2); // remove the 'Character' table
		lua_getfield(l, -1, "player");
		lua_remove(l, -2); // remove the 'persistent' table

		LuaObject<SystemBody>::PushToLua(station);

		lua_pushnumber(l, payment * 0.01);

		lua_call(l, 3, 0); // call Legal.PayFine(player, station, payment)
	} else {
		fprintf(stderr, "Legal.PayFine doesn't exist or isn't a function\n");
	}

	lua_settop(l, pristine_stack);
}

bool IsCommodityLegal(StarSystem *system, const Equip::Type type)
{
	bool legal = true;
	lua_State *l = Lua::manager->GetLuaState();
	int pristine_stack = lua_gettop(l);

	pi_lua_import(l, "Legal");
	if (!lua_isnil(l, -1)) { lua_getfield(l, -1, "IsCommodityLegal"); }

	if (lua_isfunction(l, -1)) {
		LuaObject<StarSystem>::PushToLua(system);
		lua_pushstring(l, EnumStrings::GetString("EquipType", type));
		lua_call(l, 2, 1); // call Legal.IsCommodityLegal(system, type)
		if (!lua_isnoneornil(l, -1)) {
			legal = lua_toboolean(l, -1);
		}
	} else {
		fprintf(stderr, "Legal.IsCommodityLegal doesn't exist or isn't a function\n");
	}

	lua_settop(l, pristine_stack);
	return legal;
}

}

const char *SysPolit::GetGovernmentDesc() const
{
	return Polit::s_govDesc[govType].description;
}

const char *SysPolit::GetEconomicDesc() const
{
	return Polit::s_econDesc[ Polit::s_govDesc[govType].econ ];
}
