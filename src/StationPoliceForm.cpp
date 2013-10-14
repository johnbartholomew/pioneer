// Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "StationPoliceForm.h"
#include "Pi.h"
#include "SpaceStation.h"
#include "Player.h"
#include "FaceVideoLink.h"
#include "ShipCpanel.h"
#include "SpaceStationView.h"
#include "Lang.h"
#include "StringF.h"

void StationPoliceForm::OnOptionClicked(int option)
{
	SpaceStation *station = Pi::player->GetDockedWith();
	SetTitle(stringf(Lang::SOMEWHERE_POLICE, formatarg("station", station->GetLabel())));
	SetFaceFlags(FaceVideoLink::ARMOUR);
	SetFaceSeed(Random(station->GetSystemBody()->seed).Int32());
	SetMessage(Lang::WE_HAVE_NO_BUSINESS_WITH_YOU);

	if (option)
		Close();
}
