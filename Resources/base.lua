-- ************** Globals ****************
-- BoardLayer = nil
Grid = nil
Player = nil
EnemyList = {}
HeroList = {}

-- ****************** Standard callbacks *******************
function InitGlobals(grid, player)
	print("* Init globals, arg type: ", --tolua.type(layer), 
										tolua.type(grid), 
                                        tolua.type(player))
	--BoardLayer = layer
	Grid = grid
    Player = player
end

--------------
-- Enemy List
function AddToEnemyList(e)
--	for i,v in ipairs(arg) do
		--print("* Init eneny list, arg " .. i .. " type: " .. tolua.type(v))
--		EnemyList[i] = v
--	end
    table.insert(EnemyList, e)
--	print("* Add to EnemyList, arg " .. #EnemyList .. " type: " .. tolua.type(e))
end

function ClearEnemyList()
	EnemyList = {}
end

-------------
-- Hero List
function AddToHeroList(h)
--    for i,v in ipairs(arg) do
--        HeroList[i] = v
--    end
    table.insert(HeroList, h)
--	print("* Add to HeroList, arg " .. #HeroList .. " type: " .. tolua.type(h))
end

function ClearHeroList()
    HeroList = {}
end

function FreeGlobals()
    Grid = nil
    Player = nil
    ClearEnemyList()
    ClearHeroList()
end

function PrintGlobalInfo()
	print("* type Grid " .. tolua.type(Grid))
	print("* type Player " .. tolua.type(Player))

    print("* size EnemyList " .. #EnemyList)
	for i,v in ipairs(EnemyList) do
		print ("    - enemy hero id " .. EnemyList[i]:getHeroId() )
	end

    print("* size HeroList " .. #HeroList)
    for i,v in ipairs(HeroList) do
        print ("    - player hero id " .. HeroList[i]:getHeroId() )
    end
end

-- ************** Misc ******************
function strToGem(str)
	if     str == "Fire" then return PH.Gem.Fire
	elseif str == "Water" then return PH.Gem.Water
	elseif str == "Wood" then return PH.Gem.Wood
	elseif str == "Dark" then return PH.Gem.Dark
	elseif str == "Light" then return PH.Gem.Light
	elseif str == "Health" then return PH.Gem.Health
	elseif str == "AllColor" then return PH.Gem.AllColor
    elseif str == "GemColorMax" then return PH.Gem.GemColorMax
    else 
        return -1987
    end
end
