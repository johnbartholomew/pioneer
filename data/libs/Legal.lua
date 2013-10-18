-- Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
-- Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

local Character = import("Character")

-- Crimes:
--
-- - Trading Illegal Goods
-- - Weapon Discharge (firing a weapon within X km from a space station)
-- - Piracy (damaging some other ship within X km from a space station)
-- - Murder (destroying some other ship within X km from a space station)

local Legal
Legal = {
	-- character: a Character object
	-- station: a SystemBody object
	GetFine = function (character, station)
		assert(station:isa('SystemBody'))
		return 45
	end,

	-- character: a Character object
	-- station: a SystemBody object
	-- amount: the amount of fine to pay
	PayFine = function (character, station, amount)
		assert(station:isa('SystemBody'))
		print(string.format('%s is paying a $%.2f fine at %s', character.name, amount, station.name))
	end,

	-- system: a StarSystem object
	-- commodity: one of the EquipType constants
	IsCommodityLegal = function (system, commodity)
		assert(system:isa('StarSystem'))
		return true
	end,
}
return Legal
