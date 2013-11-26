#ifndef PROXIMITY_TRIGGER_H
#define PROXIMITY_TRIGGER_H

#include "libs.h"
#include "Object.h"

class ProximityTrigger {
public:
	ProximityTrigger(Body *body, double distance, Object::Type type);

	Body *GetBody() const { return m_body; }

	sigc::signal<void, Body*, Body*, double> onEnterRegion;
	sigc::signal<void, Body*, Body*, double> onLeaveRegion;

private:
	Body *m_body;
	double m_distance;
	Object::Type m_type;
};

#endif
