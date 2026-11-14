## Sid Meier's Civilization 4
## CvHookUtils.py (added by this mod)
##
## Common functions for the Python expressions of the XML: the hooks of the traits (EntryPoints/CvTraitInterface.py),
## of buildings, promotions, techs, civics and units and PyCondition (EntryPoints/CvHookInterface.py), and the spells
## (EntryPoints/CvSpellInterface.py). Those modules import everything here, so an expression can call these functions
## directly, e.g. <PyOnBuilt>gainGold(pPlayer, 30)</PyOnBuilt>. Add new common functions here.
## Names are given as strings ('UNITCLASS_WARRIOR'), so the XML stays readable; tools\check_refs.ps1 checks them.
## The functions that change the game are called on every computer of a multiplayer game: use CyGame().getSorenRandNum
## for random numbers.

from CvPythonExtensions import *

gc = CyGlobalContext()

## Values

def scaleBySpeed(iValue, szPercent):
	"iValue scaled by a percent of the game speed (CIV4GameSpeedInfo.xml). szPercent is the tag without i and Percent: 'Train' (iTrainPercent: 100 at normal speed, 150 at epic), 'Construct', 'Research', 'Growth', 'GreatPeople', 'GoldenAge', etc."
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

def isOnFeature(pUnit, szFeature):
	"True if the unit is on a plot with the feature ('FEATURE_FOREST')"
	return pUnit.plot().getFeatureType() == gc.getInfoTypeForString(szFeature)

## Effects

def gainGold(pPlayer, iGold):
	"The player gains gold (loses it when iGold is negative)"
	pPlayer.changeGold(iGold)

def createUnits(pPlayer, szUnitClass, iNum, pPlot):
	"Creates iNum units of the unit class (the unit of the player's civilization) on the plot. Nothing without a plot."
	eUnit = pPlayer.getCivilizationUnit(gc.getInfoTypeForString(szUnitClass))
	if eUnit == -1 or pPlot is None or pPlot.isNone():
		return
	for i in range(iNum):
		pPlayer.initUnit(eUnit, pPlot.getX(), pPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

def addBuilding(pCity, szBuildingClass):
	"The city gets the building of the building class (the building of the owner's civilization)"
	pOwner = gc.getPlayer(pCity.getOwner())
	eBuilding = pOwner.getCivilizationBuilding(gc.getInfoTypeForString(szBuildingClass))
	if eBuilding != -1:
		pCity.setNumRealBuilding(eBuilding, 1)

def addCulture(pCity, iCulture):
	"The city gains culture for its owner (the plots of its culture range too)"
	pCity.changeCulture(pCity.getOwner(), iCulture, True)

def addPromotion(pUnit, szPromotion):
	"The unit gains the promotion"
	pUnit.setHasPromotion(gc.getInfoTypeForString(szPromotion), True)

def healUnit(pUnit, iPercent):
	"The unit heals iPercent of its max hit points"
	pUnit.changeDamage(-(pUnit.maxHitPoints() * iPercent / 100), PlayerTypes.NO_PLAYER)
