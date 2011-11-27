#include "CustomSystem.h"
#include "LuaConstants.h"
#include "LuaFixed.h"

static inline bool _check_table_param(lua_State *l, int tableidx, const char *key, bool required, int (*testfn)(lua_State *, int), int what, const char *where)
{
	lua_getfield(l, tableidx, key);
	if (!required && lua_isnil(l, -1))
		return false;
	if (!testfn(l, -1)) {
		luaL_where(l, 0);
		luaL_error(l, "%s bad argument '%s' to '%s' (%s expected, got %s)",
			lua_tostring(l, -1), key, where, lua_typename(l, what), luaL_typename(l, -2));
	}
	return true;
}

static inline bool _check_table_param(lua_State *l, int tableidx, const char *key, bool required, int what, const char *where)
{
	lua_getfield(l, tableidx, key);
	if (!required && lua_isnil(l, -1))
		return false;
	if (lua_type(l, -1) != what) {
		luaL_where(l, 0);
		luaL_error(l, "%s bad argument '%s' to '%s' (%s expected, got %s)",
			lua_tostring(l, -1), key, where, lua_typename(l, what), luaL_typename(l, -2));
	}
	return true;
}

/*
local sol = CustomSystem.New({
    name       = 'Sol',
    pos        = { 0, 0, 0 },
    govtype    = 'EARTHDEMOC',
    shortdesc  = 'The historical birthplace of humankind',
    longdesc   = [[Sol is a fine joint]],
    seed       = 23,
    primary    = {
        type   = 'STAR_G',
        radius = f(1,1),
        mass   = f(1,1),
        temp   = 5700,
    },
})
*/
int CustomSystem::l_customsystem_new(lua_State *l)
{
	LUA_DEBUG_START(l);

	if (!lua_istable(l, 1))
		luaL_typerror(l, 1, lua_typename(l, LUA_TTABLE));

	int old_top = lua_gettop(l);

	_check_table_param(l, 1, "name", true, lua_isstring, LUA_TSTRING, "CustomSystem.New");
	std::string name = lua_tostring(l, -1);

	_check_table_param(l, 1, "pos", true, LUA_TTABLE, "CustomSystem.New");
	float pos[3];
	bool badpos = false;
	if (lua_objlen(l, -1) != 3)
		badpos = true;
	else
		for (int i = 0; i < 3; i++) {
			lua_pushinteger(l, i+1);
			lua_gettable(l, -2);
			if (!lua_isnumber(l, -1)) {
				badpos = true;
				break;
			}
			pos[i] = lua_tonumber(l, -1);
			lua_pop(l, 1);
		}
	if (badpos) {
		luaL_where(l, 0);
		luaL_error(l, "%s bad argument 'pos' to 'CustomSystem.New' (3 numbers expected)", lua_tostring(l, -1));
	}

	Polit::GovType govType
		= _check_table_param(l, 1, "govtype", false, lua_isstring, LUA_TSTRING, "CustomSystem.New")
		? static_cast<Polit::GovType>(LuaConstants::GetConstant(l, "PolitGovType", lua_tostring(l, -1)))
		: Polit::GOV_NONE;
	
	std::string shortDesc;
	if (_check_table_param(l, 1, "shortdesc", false, lua_isstring, LUA_TSTRING, "CustomSystem.New"))
		shortDesc = lua_tostring(l, -1);

	std::string longDesc;
	if (_check_table_param(l, 1, "longdesc", false, lua_isstring, LUA_TSTRING, "CustomSystem.New"))
		longDesc = lua_tostring(l, -1);

	Uint32 seed = 0;
	bool wantRandSeed = true;
	if (_check_table_param(l, 1, "seed", false, lua_isnumber, LUA_TNUMBER, "CustomSystem.New")) {
		seed = lua_tointeger(l, -1);
		wantRandSeed = false;
	}

	_check_table_param(l, 1, "primary", true, LUA_TTABLE, "CustomSystem.New");
	int primary = lua_gettop(l);

	_check_table_param(l, primary, "type", true, lua_isstring, LUA_TSTRING, "CustomSystem.New primary");
	SBody::BodyType primaryType = static_cast<SBody::BodyType>(LuaConstants::GetConstant(l, "BodyType", lua_tostring(l, -1)));

	fixed primaryRadius;
	if (_check_table_param(l, primary, "radius", false, lua_isuserdata, LUA_TUSERDATA, "CustomSystem.New primary"))
		primaryRadius = *LuaFixed::GetFromLua(-1);

	fixed primaryMass;
	if (_check_table_param(l, primary, "mass", false, lua_isuserdata, LUA_TUSERDATA, "CustomSystem.New primary"))
		primaryMass = *LuaFixed::GetFromLua(-1);

	int primaryTemp = 0;
	if (_check_table_param(l, primary, "temp", false, lua_isnumber, LUA_TNUMBER, "CustomSystem.New primary"))
		primaryTemp = lua_tointeger(l, -1);
	
	lua_settop(l, old_top);

	CustomSystem *cs = new CustomSystem();
	cs->name = name;
	cs->pos = vector3f(pos);
	cs->govType = govType;
	cs->shortDesc = shortDesc;
	cs->longDesc = longDesc;
	cs->seed = seed;
	cs->wantRandSeed = wantRandSeed;
	cs->primaryType = primaryType;
	cs->primaryRadius = primaryRadius;
	cs->primaryMass = primaryMass;
	cs->primaryTemp = primaryTemp;

	LuaCustomSystem::PushToLuaGC(cs);

	/*
	printf("name: %s\n", name.c_str());
	printf("pos: %f %f %f\n", pos[0], pos[1], pos[2]);
	printf("govType: %d\n", govType);
	printf("shortDesc: %s\n", shortDesc.c_str());
	printf("longDesc: %s\n", longDesc.c_str());
	printf("seed: %d [rand: %s]\n", seed, wantRandSeed ? "true" : "false");
	printf("primaryType: %d\n", primaryType);
	printf("primaryRadius: %f\n", primaryRadius.ToDouble());
	printf("primaryMass: %f\n", primaryMass.ToDouble());
	printf("primaryTemp: %d\n", primaryTemp);
	*/

	LUA_DEBUG_END(l, 1);
	return 1;
}

template <> const char *LuaObject<CustomSystem>::s_type = "CustomSystem";

template <> void LuaObject<CustomSystem>::RegisterClass()
{
	static luaL_reg l_methods[] = {
		{ "New", CustomSystem::l_customsystem_new },
		{ 0, 0 }
	};

	LuaObjectBase::CreateClass(s_type, NULL, l_methods, NULL, NULL);
}




#if 0
#include "CustomSystem.h"
#include "LuaUtils.h"
#include "PiLuaClasses.h"
#include "LuaConstants.h"
#include "galaxy/Polit.h"
#include "galaxy/SystemPath.h"

static lua_State *csLua;

static std::list<CustomSystem> custom_systems;

void CustomSystem::Init()
{
	lua_State *L = csLua = lua_open();
	luaL_openlibs(L);
	OOLUA::setup_user_lua_state(L);

	PiLuaClasses::RegisterClasses(L);
	LuaConstants::Register(L);

	OOLUA::register_class<CustomSystem>(L);
	OOLUA::register_class<CustomSBody>(L);

	lua_register(L, "load_lua", pi_load_lua);

	lua_pushstring(L, PIONEER_DATA_DIR);
	lua_setglobal(L, "CurrentDirectory");

	lua_pushcfunction(L, pi_lua_panic);
	if (luaL_loadfile(L, (std::string(PIONEER_DATA_DIR) + "/pisystems.lua").c_str())) {
		pi_lua_panic(L);
	} else {
		lua_pcall(L, 0, LUA_MULTRET, -2);
	}

	lua_close(L);
}

const std::list<const CustomSystem*> CustomSystem::GetCustomSystemsForSector(int sectorX, int sectorY, int sectorZ)
{
	std::list<const CustomSystem*> sector_systems;

	for (std::list<CustomSystem>::iterator i = custom_systems.begin(); i != custom_systems.end(); i++) {
		CustomSystem *cs = &(*i);
		if (cs->sectorX == sectorX && cs->sectorY == sectorY && cs->sectorZ == sectorZ)
			sector_systems.push_back(cs);
	}

	return sector_systems;
}

const CustomSystem* CustomSystem::GetCustomSystem(const char *name)
{
	for (std::list<CustomSystem>::iterator i = custom_systems.begin(); i != custom_systems.end(); i++) {
		CustomSystem *cs = &(*i);
		if (!cs->name.compare(name)) return cs;
	}
	return NULL;
}

const SystemPath CustomSystem::GetPathForCustomSystem(const CustomSystem* cs)
{
	const std::list<const CustomSystem*> cslist = GetCustomSystemsForSector(cs->sectorX, cs->sectorY, cs->sectorZ);
	int idx = 0;
	for (std::list<const CustomSystem*>::const_iterator i = cslist.begin(); i != cslist.end(); i++) {
		if (!(*i)->name.compare(cs->name)) break;
		idx++;
	}
	assert(idx < static_cast<int>(cslist.size()));
	return SystemPath(cs->sectorX, cs->sectorY, cs->sectorZ, idx);
}

const SystemPath CustomSystem::GetPathForCustomSystem(const char* name)
{
	return GetPathForCustomSystem(GetCustomSystem(name));
}

CustomSystem::CustomSystem(std::string s, OOLUA::Lua_table t)
{
	name = s;

	numStars = 0;

	std::string stype;
	bool done = false;
	for (int i=0 ; i<4; i++) {
		int type = SBody::TYPE_GRAVPOINT;
		if (t.safe_at(i+1, stype)) {
			type = LuaConstants::GetConstant(csLua, "BodyType", stype.c_str());
			if ( type < SBody::TYPE_STAR_MIN || type > SBody::TYPE_STAR_MAX ) {
				printf("system star %d does not have a valid star type\n", i+1);
				assert(0);
			}
		}
		primaryType[i] = static_cast<SBody::BodyType>(type);

		if (type == SBody::TYPE_GRAVPOINT) done = true;
		if (!done) numStars++;
	}

	seed = 0;
	govType = Polit::GOV_NONE;
}

static void _add_children_to_sbody(lua_State* L, CustomSBody* sbody, OOLUA::Lua_table children)
{
	int i=1;
	while (1) {
		CustomSBody *kid;

		if (!children.safe_at(i++, kid))
			break;

		if (kid == NULL) {
			luaL_error(L,
				"invalid element (must be CustomSBody or table of CustomSBody)\n"
				"invalid element is child of CustomSBody '%s'", sbody->name.c_str());
		}

		while (1) {
			OOLUA::Lua_table sub;

			if (!children.safe_at(i, sub))
				break;

			if (!sub.valid())
				break;

			_add_children_to_sbody(L, kid, sub);
			i++;
			continue;
		}

		sbody->children.push_back(*kid);
	}
}

CustomSystem *CustomSystem::l_govtype(std::string st)
{
	govType = static_cast<Polit::GovType>(LuaConstants::GetConstant(csLua, "PolitGovType", st.c_str()));
	return this;
}

void CustomSystem::l_bodies(lua_State* L, CustomSBody& primary_star, OOLUA::Lua_table children)
{
	if ( primary_star.type < SBody::TYPE_STAR_MIN || primary_star.type > SBody::TYPE_STAR_MAX )
		luaL_error(L, "first body does not have a valid star type");
	if ( primary_star.type != primaryType[0] )
		luaL_error(L, "first body is not of same type as system primary star");

	_add_children_to_sbody(L, &primary_star, children);
	sBody = primary_star;
}

void CustomSystem::l_add_to_sector(int x, int y, int z, pi_vector& v)
{
	sectorX = x;
	sectorY = y;
	sectorZ = z;
	pos = v;

	custom_systems.push_back(*this);
}

EXPORT_OOLUA_FUNCTIONS_0_CONST(CustomSystem)
EXPORT_OOLUA_FUNCTIONS_6_NON_CONST(CustomSystem, seed, govtype, short_desc, long_desc, bodies, add_to_sector)

CustomSBody::CustomSBody(std::string s, std::string stype)
{
	name = s;
	type = static_cast<SBody::BodyType>(LuaConstants::GetConstant(csLua, "BodyType", stype.c_str()));

	if ( type < SBody::TYPE_MIN || type > SBody::TYPE_MAX ) {
		printf("body '%s' does not have a valid type\n", s.c_str());
		assert(0);
	}

	seed = averageTemp = 0;
	latitude = longitude = 0.0;
	want_rand_offset = true;
	want_rand_seed = true;
}

EXPORT_OOLUA_FUNCTIONS_0_CONST(CustomSBody)

// this is the same as EXPORT_OOLUA_FUNCTIONS_*_NON_CONST. oolua doesn't
// provide a macro for that many members, and the varargs version seems to
// fail after 16 parameters
CLASS_LIST_MEMBERS_START_OOLUA_NON_CONST(CustomSBody)
LUA_MEMBER_FUNC_9(OOLUA::Proxy_class<CustomSBody>, seed, radius, mass, temp, semi_major_axis, eccentricity, orbital_offset, latitude, inclination)
LUA_MEMBER_FUNC_9(OOLUA::Proxy_class<CustomSBody>, longitude, rotation_period, axial_tilt, height_map, metallicity, volcanicity, atmos_density, atmos_oxidizing, ocean_cover)
LUA_MEMBER_FUNC_2(OOLUA::Proxy_class<CustomSBody>, ice_cover, life)
CLASS_LIST_MEMBERS_END
#endif
