#include "LuaObject.h"
#include "LuaSector.h"
#include "LuaUtils.h"
#include "LuaConstants.h"
#include "CustomSystem.h"

static int l_sector_new(lua_State *l)
{
	int x = luaL_checkinteger(l, 1);
	int y = luaL_checkinteger(l, 2);
	int z = luaL_checkinteger(l, 3);
	RefCountedPtr<Sector> s = Sector::Get(x, y, z);
	LuaSector::PushToLua(s.Get());
	return 1;
}

static int l_sector_get_systems(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);

	LUA_DEBUG_START(l);

	lua_newtable(l);

	const std::vector<Sector::System> systems = s->GetSystems();
	for (std::vector<Sector::System>::const_iterator i = systems.begin(); i != systems.end(); i++) {
		lua_pushinteger(l, lua_objlen(l, -1)+1);

		lua_newtable(l);
		pi_lua_settable(l, "name", (*i).name.c_str());

		lua_pushstring(l, "pos");
		lua_newtable(l);
		pi_lua_settable(l, "x", (*i).p.x);
		pi_lua_settable(l, "y", (*i).p.y);
		pi_lua_settable(l, "z", (*i).p.z);
		lua_rawset(l, -3);

		lua_pushstring(l, "stars");
		lua_newtable(l);
		for (int j = 0; j < (*i).numStars; j++) {
			lua_pushinteger(l, lua_objlen(l, -1)+1);
			lua_pushstring(l, LuaConstants::GetConstantString(l, "BodyType", (*i).starType[j]));
			lua_rawset(l, -3);
		}
		lua_rawset(l, -3);

		lua_rawset(l, -3);
	}

	LUA_DEBUG_END(l, 1);

	return 1;
}

static int l_sector_set_custom_only(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);
	bool c = lua_toboolean(l, 2);
	s->SetCustomOnly(c);
	return 0;
}

static int l_sector_add_system(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);
	CustomSystem *cs = LuaCustomSystem::GetFromLua(2);
	LuaCustomSystem::ReleaseOwnership(cs);
	s->AddSystem(cs);
	return 0;
}

static int l_sector_attr_x(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);
	lua_pushinteger(l, s->sx);
	return 1;
}

static int l_sector_attr_y(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);
	lua_pushinteger(l, s->sy);
	return 1;
}

static int l_sector_attr_z(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);
	lua_pushinteger(l, s->sz);
	return 1;
}

static int l_sector_attr_custom_only(lua_State *l)
{
	Sector *s = LuaSector::GetFromLua(1);
	lua_pushboolean(l, s->GetCustomOnly());
	return 1;
}

template <> const char *LuaObject<Sector>::s_type = "Sector";

template <> void LuaObject<Sector>::RegisterClass()
{
	static luaL_reg l_methods[] = {
		{ "New",           l_sector_new             },
		{ "GetSystems",    l_sector_get_systems     },
		{ "SetCustomOnly", l_sector_set_custom_only },
		{ "AddSystem",     l_sector_add_system      },
		{ 0, 0 }
	};

	static luaL_reg l_attrs[] = {
		{ "x",          l_sector_attr_x },
		{ "y",          l_sector_attr_y },
		{ "z",          l_sector_attr_z },
		{ "customOnly", l_sector_attr_custom_only },
		{ 0, 0 }
	};

	LuaObjectBase::CreateClass(s_type, NULL, l_methods, l_attrs, NULL);
}
