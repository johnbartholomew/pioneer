-- Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
-- Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

local Character = import("Character")

local Legal
Legal = {
	-- character: a Character object
	-- station: a SystemBody object
	GetFine = function (character, station)
		return 45
	end,

	-- character: a Character object
	-- station: a SystemBody object
	-- amount: the amount of fine to pay
	PayFine = function (character, station, amount)
		print(string.format('%s is paying a $%.2f fine at %s', character.name, amount, station.name))
	end,

	-- system: a StarSystem object
	-- commodity: one of the EquipType constants
	IsCommodityLegal = function (system, commodity)
		return true
	end,
}
return Legal
