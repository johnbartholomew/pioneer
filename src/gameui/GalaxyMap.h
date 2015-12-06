// Copyright © 2008-2015 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef GAMEUI_GALAXYMAP_H
#define GAMEUI_GALAXYMAP_H

#include "ui/Context.h"
#include "ui/OverlayStack.h"

namespace GameUI {

class LabelOverlay;

class GalaxyMap : public UI::OverlayStack {
public:
	GalaxyMap(UI::Context *context);

	virtual UI::Point PreferredSize() override;
	virtual void Update() override;

	float GetZoom() const { return m_zoom; }
	GalaxyMap *SetZoom(float v);
	GalaxyMap *SetCentreSector(const vector2f &at);

	void ClearLabels();

	// Position is in sector X,Y coordinates.
	GalaxyMap *AddAreaLabel(const vector2f &at, const std::string &text);
private:
	UI::Image *m_baseImage;
	GameUI::LabelOverlay *m_labelOverlay;
	float m_zoom;
	vector2f m_centreSector;
};

}

#endif