## Sid Meier's Civilization 4
## CvTraitInterface.py (added by this mod)
##
## The DLL calls the functions below for the Python hooks of the traits (Assets/XML/Civilizations/CIV4TraitInfos.xml).
## A hook is a Python expression evaluated here, for each player that has the trait (traits given by civics, buildings
## and techs with FreeTraits count too). The names an expression can use:
##   PyOnGameStart     pPlayer (CyPlayer), eTrait: once at the start of the game, after the starting gold and units
##                     (not in scenarios, which do not give the starting items)
##   PyOnBeginTurn     pPlayer, eTrait: at the start of each turn of the player (with onBeginPlayerTurn of CvEventManager)
##   PyOnCityFounded   pPlayer, eTrait, pCity (CyCity): after the player founds the city
##   PyOnCityAcquired  pPlayer, eTrait, pCity, iOldOwner, bConquest, bTrade (1 or 0): after the player acquires the city
##                     by conquest, trade or culture. A conquered city can still be razed afterwards.
## Write common functions below and give them values in the XML, e.g.
##   <PyOnGameStart>gainGold(pPlayer, scaleBySpeed(100, 'Train'))</PyOnGameStart>
## The hooks change the game and are called on every computer of a multiplayer game: use CyGame().getSorenRandNum for
## random numbers. Describe the effects in the Help of the trait (the trait text shows it).

from CvPythonExtensions import *

gc = CyGlobalContext()

def onGameStart(argsList):
	pPlayer, eTrait = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnGameStart())

def onBeginTurn(argsList):
	pPlayer, eTrait = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnBeginTurn())

def onCityFounded(argsList):
	pPlayer, eTrait, pCity = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnCityFounded())

def onCityAcquired(argsList):
	pPlayer, eTrait, pCity, iOldOwner, bConquest, bTrade = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnCityAcquired())

## Common functions: values

def scaleBySpeed(iValue, szPercent):
	"iValue scaled by a percent of the game speed (CIV4GameSpeedInfo.xml). szPercent is the tag without i and Percent: 'Train' (iTrainPercent: 100 at normal speed, 150 at epic), 'Construct', 'Research', 'Growth', 'GreatPeople', etc."
	kSpeed = gc.getGameSpeedInfo(CyGame().getGameSpeedType())
	return iValue * getattr(kSpeed, 'get' + szPercent + 'Percent')() / 100

def startEra():
	"The era the game started in (0: ancient)"
	return CyGame().getStartEra()

def isEveryTurns(iTurns):
	"True on every iTurns-th game turn"
	return CyGame().getGameTurn() % iTurns == 0

def capitalPlot(pPlayer):
	"The plot of the capital (None without a capital)"
	pCity = pPlayer.getCapitalCity()
	if pCity.isNone():
		return None
	return pCity.plot()

## Common functions: effects

def gainGold(pPlayer, iGold):
	"The player gains gold"
	pPlayer.changeGold(iGold)

def createUnits(pPlayer, szUnitClass, iNum, pPlot):
	"Creates iNum units of the unit class (the unit of the player's civilization) on the plot. Nothing without a plot."
	eUnit = gc.getCivilizationInfo(pPlayer.getCivilizationType()).getCivilizationUnits(gc.getInfoTypeForString(szUnitClass))
	if eUnit == -1 or pPlot is None or pPlot.isNone():
		return
	for i in range(iNum):
		pPlayer.initUnit(eUnit, pPlot.getX(), pPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

def addBuilding(pCity, szBuildingClass):
	"The city gets the building of the building class (the building of the owner's civilization)"
	pOwner = gc.getPlayer(pCity.getOwner())
	eBuilding = gc.getCivilizationInfo(pOwner.getCivilizationType()).getCivilizationBuildings(gc.getInfoTypeForString(szBuildingClass))
	if eBuilding != -1:
		pCity.setNumRealBuilding(eBuilding, 1)
