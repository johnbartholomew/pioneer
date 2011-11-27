#ifndef _LUASECTOR_H
#define _LUASECTOR_H

#include "LuaObject.h"
#include "galaxy/Sector.h"

// this specialisation releases the sector when lua is done with it, which
// probably causes it to be freed
template<>
class LuaAcquirer<Sector> {
public:
	virtual void OnAcquire(Sector *o) {
		o->IncRefCount();
	}
	virtual void OnRelease(Sector *o) {
		o->DecRefCount();
	}
};

typedef LuaObject<Sector> LuaSector;

#endif
