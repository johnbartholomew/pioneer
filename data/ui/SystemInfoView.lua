local Engine = import("Engine")
local Game = import("Game")
local Lang = import("Lang")
local ui = Engine.ui
local l = Lang.GetResource("ui-core")

local iconsContainer = ui:Align('TOP_LEFT')

local sysInfoView =
	ui:Grid(2,1)
		:SetCell(0,0, ui:Margin(20, 'ALL', ui:Scroller(iconsContainer)))
		:SetCell(1,0, ui:Margin(20, 'ALL', ui:Label('System Information'):SetFont('HEADING_LARGE')))

local ICON_MAP = {
	BROWN_DWARF = 'icons/object_brown_dwarf.png',
	WHITE_DWARF = 'icons/object_white_dwarf.png',
	STAR_M = 'icons/object_star_m.png',
	STAR_K = 'icons/object_star_k.png',
	STAR_G = 'icons/object_star_g.png',
	STAR_F = 'icons/object_star_f.png',
	STAR_A = 'icons/object_star_a.png',
	STAR_B = 'icons/object_star_b.png',
	STAR_O = 'icons/object_star_b.png', -- shares B graphic for now
	STAR_M_GIANT = 'icons/object_star_m_giant.png',
	STAR_K_GIANT = 'icons/object_star_k_giant.png',
	STAR_G_GIANT = 'icons/object_star_g_giant.png',
	STAR_F_GIANT = 'icons/object_star_f_giant.png',
	STAR_A_GIANT = 'icons/object_star_a_giant.png',
	STAR_B_GIANT = 'icons/object_star_b_giant.png',
	STAR_O_GIANT = 'icons/object_star_o.png', -- uses old O type graphic
	STAR_M_SUPER_GIANT = 'icons/object_star_m_super_giant.png',
	STAR_K_SUPER_GIANT = 'icons/object_star_k_super_giant.png',
	STAR_G_SUPER_GIANT = 'icons/object_star_g_super_giant.png',
	STAR_F_SUPER_GIANT = 'icons/object_star_g_super_giant.png', -- shares G graphic for now
	STAR_A_SUPER_GIANT = 'icons/object_star_a_super_giant.png',
	STAR_B_SUPER_GIANT = 'icons/object_star_b_super_giant.png',
	STAR_O_SUPER_GIANT = 'icons/object_star_b_super_giant.png', -- uses B type graphic for now
	STAR_M_HYPER_GIANT = 'icons/object_star_m_hyper_giant.png',
	STAR_K_HYPER_GIANT = 'icons/object_star_k_hyper_giant.png',
	STAR_G_HYPER_GIANT = 'icons/object_star_g_hyper_giant.png',
	STAR_F_HYPER_GIANT = 'icons/object_star_f_hyper_giant.png',
	STAR_A_HYPER_GIANT = 'icons/object_star_a_hyper_giant.png',
	STAR_B_HYPER_GIANT = 'icons/object_star_b_hyper_giant.png',
	STAR_O_HYPER_GIANT = 'icons/object_star_b_hyper_giant.png', -- uses B type graphic for now
	STAR_M_WF = 'icons/object_star_m_wf.png',
	STAR_B_WF = 'icons/object_star_b_wf.png',
	STAR_O_WF = 'icons/object_star_o_wf.png',
	STAR_S_BH = 'icons/object_star_bh.png',
	STAR_IM_BH = 'icons/object_star_smbh.png',
	STAR_SM_BH = 'icons/object_star_smbh.png',
	PLANET_ASTEROID = 'icons/object_planet_asteroid.png',
	STARPORT_ORBITAL = 'icons/object_orbital_starport.png',
	PLANET_GAS_GIANT = function (body)
		local mass, averageTemp = body.mass, body.averageTemp
		local iconBase
		if mass > 1.8e27 then -- e.g., Jupiter
			iconBase = 'icons/object_planet_large_gas_giant'
		elseif mass > 4.5e26 then -- e.g., Saturn
			iconBase = 'icons/object_planet_medium_gas_giant'
		else -- e.g., Uranus, Neptune
			iconBase = 'icons/object_planet_small_gas_giant'
		end
		if averageTemp > 1000 then
			return iconBase .. '_hot.png'
		else
			return iconBase .. '.png'
		end
	end,
	PLANET_TERRESTRIAL = function (body)
		local mass, averageTemp = body.mass, body.averageTemp
		local volatileGas, volatileLiquid, volatileIces =
				body.volatileGas, body.volatileLiquid, body.volatileIces
		local volcanicity, atmosOxidising = body.volcanicity, body.atmosOxidising
		local life = body.life
		if volatileLiquid > 0.7 then
			if averageTemp > 250 then return 'icons/object_planet_water.png'
			else return 'icons/object_planet_ice.png' end
		elseif life > 0.9 and volatileGas > 0.6 then return 'icons/object_planet_life.png'
		elseif life > 0.8 and volatileGas > 0.5 then return 'icons/object_planet_life6.png'
		elseif life > 0.7 and volatileGas > 0.45 then return 'icons/object_planet_life7.png'
		elseif life > 0.6 and volatileGas > 0.4 then return 'icons/object_planet_life8.png'
		elseif life > 0.5 and volatileGas > 0.3 then return 'icons/object_planet_life4.png'
		elseif life > 0.4 and volatileGas > 0.2 then return 'icons/object_planet_life5.png'
		elseif life > 0.1 then
			if volatileGas > 0.2 then return 'icons/object_planet_life2.png'
			else return 'icons/object_planet_life3.png' end
		elseif mass < 6e23 then return 'icons/object_planet_small.png'
		elseif mass < 6e22 then return 'icons/object_planet_dwarf.png'
		elseif volatileLiquid < 0.1 and volatileGas > 0.2 then return 'icons/object_planet_desert.png'
		elseif volatileIces + volatileLiquid > 0.6 then
			if volatileIces > volatileLiquid or averageTemp < 250 then return 'icons/object_planet_ice.png'
			else return 'icons/object_planet_water.png' end
		elseif volatileGas > 0.5 then
			if atmosOxidising > 0.5 then
				if averageTemp > 300 then
					return 'icons/object_planet_co2_2.png'
				elseif averageTemp > 250 then
					if volatileLiquid > 0.3 and volatileGas > 0.2 then
						return 'icons/object_planet_co2_4.png'
					else
						return 'icons/object_planet_co2_3.png'
					end
				else
					return 'icons/object_planet_co2.png'
				end
			else
				if averageTemp > 300 then return 'icons/object_planet_methane3.png'
				elseif averageTemp > 250 then return 'icons/object_planet_methane2.png'
				else return 'icons/object_planet_methane.png' end
			end
		elseif volatileLiquid > 0.1 and volatileGas < 0.1 then return 'icons/object_planet_ice.png'
		elseif volcanicity > 0.7 then return 'icons/object_planet_volcanic.png'
		else return 'icons/object_planet_small.png' end
	end,
}

local function pickIcon(body)
	local filepath = ICON_MAP[body.type]
	if type(filepath) == 'function' then
		filepath = filepath(body)
	end
	return filepath
end

local FLIP_DIR = { ['HBox'] = 'VBox', ['VBox'] = 'HBox' }

local function collectGridWidgets(body, offset, icons, labels)
	local icon = pickIcon(body)
	if icon ~= nil then
		icons[#icons+1] =
			ui:Align('LEFT', ui:Margin(offset, 'LEFT', ui:Image(icon, {'PRESERVE_ASPECT'})))
			--ui:Align('MIDDLE', )
		labels[#labels+1] =
			ui:Align('LEFT', ui:Margin(offset, 'LEFT', ui:Label(body.name)))
	end

	local children = body:GetChildren()
	local noffset = offset + 20
	for i = 1, #children do
		collectGridWidgets(children[i], noffset, icons, labels)
	end
end

local function buildIconGrid(root)
	local icons, labels = {}, {}
	collectGridWidgets(root, 0, icons, labels)
	local grid = ui:Table():SetRowAlignment('CENTER'):SetColumnAlignment('CENTER')
	for i = 1, #icons do
		grid:AddRow({icons[i], labels[i]})
	end
	return grid
end

local function buildIconTree(body, dir, level)
	level = level or '0'
	local icon = pickIcon(body)
	local iconWidget
	if icon ~= nil then
		iconWidget =
			ui:HBox(2):PackEnd({
				ui:Align('MIDDLE', ui:Image(icon, {'PRESERVE_ASPECT'})),
				ui:Align('LEFT', ui:Label(body.name)),
			})
	end
	print(string.format('%s %s (%s)', string.rep('+', level), body.name, tostring(icon)))

	local children = body:GetChildren()
	local nlevel = level + 1
	local ndir = dir --FLIP_DIR[dir]
	local subwidgets = {}
	for i = 1, #children do
		local sw = buildIconTree(children[i], ndir, nlevel)
		if sw ~= nil then
			subwidgets[#subwidgets+1] = sw
		end
	end

	if #subwidgets > 0 then
		return ui[dir](ui, 5):PackEnd(iconWidget):PackEnd(subwidgets)
	else
		return iconWidget -- may be nil
	end
end

local function bodyIconImage(body)
	local icon = pickIcon(body)
	if icon ~= nil then
		return ui:Image(icon, {'PRESERVE_ASPECT'})
	else
		return nil
	end
end

local function buildMagicMoonLayout(body)
	local icon = bodyIconImage(body)
	if icon == nil then return nil end
	local rows = { ui:Align('TOP', icon) }
	local children = body:GetChildren()
	for i = 1, #children do
		local icon = bodyIconImage(children[i])
		if icon ~= nil then
			rows[#rows + 1] = ui:Align('TOP', icon)
		end
	end
	return ui:VBox(1):PackEnd(rows)
end

local function buildMagicPlanetLayout(body)
	local icon = bodyIconImage(body)
	if icon == nil then return nil end
	local cols = {}
	local children = body:GetChildren()
	for i = 1, #children do
		local tree = buildMagicMoonLayout(children[i])
		if tree ~= nil then
			cols[#cols + 1] = tree
		end
	end

	return ui:HBox(5):PackEnd({
			ui:Align('LEFT', icon),
			ui:VBox(2):PackEnd({
				ui:Margin(2, 'VERTICAL', ui:Align('TOP_LEFT', body.name)),
				ui:HBox(2):PackEnd(cols)
			})
		})
end

local function buildMagicStarLayout(body)
	local rows = {
		ui:HBox(10):PackEnd({
			ui:Align('LEFT', bodyIconImage(body)),
			ui:Margin(10, 'TOP', ui:Align('TOP_LEFT', body.name))
		})
	}
	local children = body:GetChildren()
	for i = 1, #children do
		local tree = buildMagicPlanetLayout(children[i])
		if tree ~= nil then
			rows[#rows + 1] = tree
		end
	end
	return ui:VBox(5):PackEnd(rows)
end

local currentSystem

ui.templates.SystemInfoView = function ()
	if currentSystem ~= Game.system then
		currentSystem = Game.system
		iconsContainer:SetInnerWidget(buildMagicStarLayout(currentSystem.rootBody))
		--iconsContainer:SetInnerWidget(buildIconGrid(currentSystem.rootBody))
		--iconsContainer:SetInnerWidget(buildIconTree(currentSystem.rootBody, 'VBox'))
	end
	return sysInfoView
end
