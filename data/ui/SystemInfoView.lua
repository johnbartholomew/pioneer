local Engine = import("Engine")
local Game = import("Game")
local Lang = import("Lang")
local ui = Engine.ui
local l = Lang.GetResource("ui-core")
local TabView = import("ui/TabView")

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
		local volcanicity, atmosOxidizing = body.volcanicity, body.atmosOxidizing
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
			if atmosOxidizing > 0.5 then
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

local function iconWidget(ui, iconPath)
	return ui:Image(iconPath, {'PRESERVE_ASPECT'}):SetNaturalSize(1.5)
end

local FLIP_DIR = { ['HBox'] = 'VBox', ['VBox'] = 'HBox' }

local function collectGridWidgets(body, offset, icons, labels)
	local icon = pickIcon(body)
	if icon ~= nil then
		icons[#icons+1] =
			ui:Align('LEFT', ui:Margin(offset, 'LEFT', iconWidget(ui, icon)))
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
				ui:Align('MIDDLE', iconWidget(ui, icon)),
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
		return iconWidget(ui, icon)
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

	local info =
		ui:VBox(2):PackEnd({
			ui:Align('TOP_LEFT', body.name),
			ui:HBox(2):PackEnd(cols)
		})
	return icon, info
end

local function buildMagicStarLayout(body)
	local stars
	if body.type == 'GRAVPOINT' then
		stars = body:GetChildren()
	else
		stars = { body }
	end
	local rows = {}
	for istar = 1, #stars do
		local star = stars[istar]
		rows[#rows + 1] = ui:Align('MIDDLE', ui:Label(star.name):SetFont('HEADING_SMALL'))
		rows[#rows + 1] = ui:Align('MIDDLE', bodyIconImage(star))
		local children = star:GetChildren()
		if #children > 0 then
			local planetTable = ui:Table()
					:SetRowAlignment('CENTER')
					:SetColumnAlignment('LEFT')
					:SetColumnSpacing(5)
					:SetRowSpacing(2)
			for ichild = 1, #children do
				local icon, info = buildMagicPlanetLayout(children[ichild])
				if icon ~= nil then
					planetTable:AddRow({ui:Align('MIDDLE', icon), info})
				end
			end
			rows[#rows + 1] = planetTable
		end
	end
	return ui:Expand('HORIZONTAL', ui:VBox(5):PackEnd(rows))
end

local currentSystem
local systemInfoTab = ui:Label('System info goes here')
local bodyInfoTab = ui:Label('Body info goes here')

local infoTabs = TabView.New()
infoTabs:AddTab({
	id = 'System', title = 'System', icon = 'Star',
	template = function () return systemInfoTab; end
})
infoTabs:AddTab({
	id = 'Body', title = 'Body', icon = 'Planet',
	template = function () return bodyInfoTab; end
})

local iconsContainer = ui:Scroller():SetFont('SMALL')
local sysInfoView =
	ui:Margin(4, 'ALL',
		ui:Grid({3,7}, 1)
			:SetCell(0,0, ui:Margin(2, 'RIGHT', ui:Background(iconsContainer)))
			:SetCell(1,0, ui:Margin(2, 'LEFT', ui:Background(infoTabs)))
	)

ui.templates.SystemInfoView = function ()
	local sys = Game.player:GetHyperspaceTarget() or Game.system
	if sys:isa('SystemPath') then
		sys = sys:SystemOnly():GetStarSystem()
	end
	if sys ~= currentSystem then
		currentSystem = sys
		iconsContainer:SetInnerWidget(buildMagicStarLayout(sys.rootBody))
	end
	return sysInfoView
end
