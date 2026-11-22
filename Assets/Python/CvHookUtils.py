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

def isCoastalCity(pCity):
	"The city touches the sea (an ocean, not a lake)"
	return pCity.isCoastal(gc.getMIN_WATER_SIZE_FOR_OCEAN())

def isOverseasCity(pCity):
	"The city is in another region than the capital of its owner (the regions of this mod: CyPlot.isOverseas)"
	return pCity.plot().isOverseas(pCity.getOwner())

def isNewWorldCity(pCity):
	"The city is in a region where no player started (CyPlot.isNewWorld)"
	return pCity.plot().isNewWorld()

def isBuildingClassIn(eBuilding, lszBuildingClasses):
	"True if the building is of one of the building classes (['BUILDINGCLASS_GRANARY', 'BUILDINGCLASS_AQUEDUCT']), unique buildings too"
	iBuildingClass = gc.getBuildingInfo(eBuilding).getBuildingClassType()
	for szBuildingClass in lszBuildingClasses:
		if iBuildingClass == gc.getInfoTypeForString(szBuildingClass):
			return True
	return False

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

def createExecutives(pPlayer, eCorporation, iNum, pCity):
	"Creates iNum units that spread the corporation (the executive of the player's civilization) in the city"
	for iUnitClass in range(gc.getNumUnitClassInfos()):
		eUnit = pPlayer.getCivilizationUnit(iUnitClass)
		if eUnit != -1 and gc.getUnitInfo(eUnit).getCorporationSpreads(eCorporation) > 0:
			createUnits(pPlayer, gc.getUnitClassInfo(iUnitClass).getType(), iNum, pCity.plot())
			return

def createMissionaries(pPlayer, eReligion, iNum, pCity):
	"Creates iNum units that spread the religion (the missionary of the player's civilization) in the city"
	for iUnitClass in range(gc.getNumUnitClassInfos()):
		eUnit = pPlayer.getCivilizationUnit(iUnitClass)
		if eUnit != -1 and gc.getUnitInfo(eUnit).getReligionSpreads(eReligion) > 0:
			createUnits(pPlayer, gc.getUnitClassInfo(iUnitClass).getType(), iNum, pCity.plot())
			return

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

def createCityConscripts(pPlayer, iMinPopulation):
	"""In the capital and in each city of at least iMinPopulation, one unit of what that city can conscript now
	(CyCity.getConscriptUnit). The player gets a message with the number. Returns the number of units"""
	iNum = 0
	(pCity, iter) = pPlayer.firstCity(False)
	while pCity is not None and not pCity.isNone():
		if pCity.isCapital() or pCity.getPopulation() >= iMinPopulation:
			eUnit = pCity.getConscriptUnit()
			if eUnit != -1:
				pPlayer.initUnit(eUnit, pCity.getX(), pCity.getY(), UnitAITypes.UNITAI_CITY_DEFENSE, DirectionTypes.DIRECTION_SOUTH)
				iNum += 1
		(pCity, iter) = pPlayer.nextCity(iter, False)
	if iNum > 0:
		szText = CyTranslator().getText("TXT_KEY_MISC_CITY_CONSCRIPTS_CREATED", (iNum, ))
		CyInterface().addMessage(pPlayer.getID(), True, gc.getEVENT_MESSAGE_TIME(), szText, "AS2D_UNIT_BUILD_UNIT", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, gc.getInfoTypeForString("COLOR_GREEN"), -1, -1, False, False)
	return iNum

def makeCityPlotHills(pCity, szTerrain):
	"The plot of the city becomes a hill of the terrain ('TERRAIN_PLAINS'), e.g. from PyOnCityFounded. A feature that cannot stay on it is removed"
	pPlot = pCity.plot()
	if not pPlot.isHills():
		pPlot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
	eTerrain = gc.getInfoTypeForString(szTerrain)
	if eTerrain != -1 and pPlot.getTerrainType() != eTerrain:
		pPlot.setTerrainType(eTerrain, True, True)
	eFeature = pPlot.getFeatureType()
	if eFeature != -1 and not pPlot.canHaveFeature(eFeature):
		pPlot.setFeatureType(FeatureTypes.NO_FEATURE, -1)

def gainTraitForTurns(pPlayer, szTrait, iTurns):
	"The player has the trait for iTurns turns (CyPlayer.setTraitTurns). If it is already temporary, the longer of the two counts (no stacking). Scale by game speed with scaleBySpeed(iTurns, 'GoldenAge') if wanted"
	eTrait = gc.getInfoTypeForString(szTrait)
	if eTrait == -1 or iTurns <= 0:
		return
	pPlayer.setTraitTurns(eTrait, max(pPlayer.getTraitTurns(eTrait), iTurns))

def healUnit(pUnit, iPercent):
	"The unit heals iPercent of its max hit points"
	pUnit.changeDamage(-(pUnit.maxHitPoints() * iPercent / 100), PlayerTypes.NO_PLAYER)

# The promotions of createPartisans: the first one at the culture level poor, all six at legendary
PARTISAN_PROMOTIONS = ['PROMOTION_COMBAT1', 'PROMOTION_COMBAT2', 'PROMOTION_COMBAT3', 'PROMOTION_COMBAT4', 'PROMOTION_COMBAT5', 'PROMOTION_COMBAT6']

def isPartisanPlot(pPlot, pPlayer):
	"A land plot where createPartisans can place a unit of the player: not impassable, no city, no unit at war with the player"
	if pPlot is None or pPlot.isNone():
		return False
	if pPlot.isWater() or pPlot.isImpassable() or pPlot.isCity():
		return False
	pTeam = gc.getTeam(pPlayer.getTeam())
	for i in range(pPlot.getNumUnits()):
		if pTeam.isAtWar(pPlot.getUnit(i).getTeam()):
			return False
	return True

def createPartisans(pPlayer, pCity, iNum, iCultureLevel):
	"""Creates iNum units of what the capital of the player can conscript now (CyCity.getConscriptUnit) on random plots next to
	the city (2 plots away if none of them is free, see isPartisanPlot). Each unit gains the Combat promotions up to the culture
	level (PARTISAN_PROMOTIONS: Combat I at poor ... Combat VI at legendary). The player gets a message. Nothing without a capital,
	a unit to conscript or a free plot. For PyOnCityLost (pCity is no longer the player's)"""
	pCapital = pPlayer.getCapitalCity()
	if pCapital is None or pCapital.isNone():
		return
	eUnit = pCapital.getConscriptUnit()
	if eUnit == -1 or iNum <= 0:
		return
	listPlots = []
	for iRange in (1, 2):
		for iDX in range(-iRange, iRange + 1):
			for iDY in range(-iRange, iRange + 1):
				if max(abs(iDX), abs(iDY)) == iRange:
					pPlot = plotXY(pCity.getX(), pCity.getY(), iDX, iDY)
					if isPartisanPlot(pPlot, pPlayer):
						listPlots.append(pPlot)
		if len(listPlots) > 0:
			break
	if len(listPlots) == 0:
		return
	listPromotions = []
	for i in range(min(iCultureLevel, len(PARTISAN_PROMOTIONS))):
		listPromotions.append(gc.getInfoTypeForString(PARTISAN_PROMOTIONS[i]))
	for i in range(iNum):
		pPlot = listPlots[CyGame().getSorenRandNum(len(listPlots), "Partisan placement")]
		pUnit = pPlayer.initUnit(eUnit, pPlot.getX(), pPlot.getY(), UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)
		for ePromotion in listPromotions:
			pUnit.setHasPromotion(ePromotion, True)
	szText = CyTranslator().getText("TXT_KEY_MISC_PARTISANS_CREATED", (pCity.getNameKey(), iNum))
	CyInterface().addMessage(pPlayer.getID(), True, gc.getEVENT_MESSAGE_TIME(), szText, "AS2D_UNIT_BUILD_UNIT", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, gc.getUnitInfo(eUnit).getButton(), gc.getInfoTypeForString("COLOR_GREEN"), pCity.getX(), pCity.getY(), True, True)
