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
import heapq

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

def isUnitClassIn(pUnit, lszUnitClasses):
	"True if the unit is of one of the unit classes (['UNITCLASS_GREAT_GENERAL', 'UNITCLASS_ENGINEER']), unique units too"
	iUnitClass = pUnit.getUnitClassType()
	for szUnitClass in lszUnitClasses:
		if iUnitClass == gc.getInfoTypeForString(szUnitClass):
			return True
	return False

def isTechIn(eTech, lszTechs):
	"True if the tech is one of the techs (['TECH_WRITING', 'TECH_ALPHABET'])"
	for szTech in lszTechs:
		if eTech == gc.getInfoTypeForString(szTech):
			return True
	return False

def isCivicIn(eCivic, lszCivics):
	"True if the civic is one of the civics (['CIVIC_REPRESENTATION', 'CIVIC_UNIVERSAL_SUFFRAGE'])"
	for szCivic in lszCivics:
		if eCivic == gc.getInfoTypeForString(szCivic):
			return True
	return False

def isOnFeature(pUnit, szFeature):
	"True if the unit is on a plot with the feature ('FEATURE_FOREST')"
	return pUnit.plot().getFeatureType() == gc.getInfoTypeForString(szFeature)

def infoDescription(szInfo, eType):
	"The name of an info for messages: szInfo is the class without Cv and Info ('Tech', 'Civic', 'Era', 'Religion', 'Project'), '-' for -1"
	if eType < 0:
		return '-'
	return getattr(gc, 'get' + szInfo + 'Info')(eType).getDescription()

def cityName(pCity):
	"The name of the city for messages, '-' without a city"
	if pCity.isNone():
		return '-'
	return pCity.getName()

## Effects

def showMessage(pPlayer, szKey, tArgs = ()):
	"Shows the text (a key of the text XML with the arguments %s1, %s2... in the tuple tArgs) in the event log of the player"
	szText = CyTranslator().getText(szKey, tArgs)
	CyInterface().addMessage(pPlayer.getID(), True, gc.getEVENT_MESSAGE_TIME(), szText, "", InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, gc.getInfoTypeForString("COLOR_WHITE"), -1, -1, False, False)

def gainGold(pPlayer, iGold):
	"The player gains gold (loses it when iGold is negative)"
	pPlayer.changeGold(iGold)

def startGoldenAge(pPlayer, iPercent = 100):
	"The player starts a golden age of iPercent % of the usual length (at least 1 turn; like a free golden age: longer if one is going on, and the great people needed for the next one stay the same)"
	pPlayer.changeGoldenAgeTurns(max(1, pPlayer.getGoldenAgeLength() * iPercent / 100))

def gainCircumnavigationMoves(pPlayer):
	"The team of the player gets the bonus moves of the ships of the first circumnavigation (CIRCUMNAVIGATE_FREE_MOVES), with its message. For PyOnCircumnavigated when iOrder > 1 (the first team gets them anyway). Once per player: a team of two players with the trait gets them twice"
	iMoves = gc.getDefineINT("CIRCUMNAVIGATE_FREE_MOVES")
	gc.getTeam(pPlayer.getTeam()).changeExtraMoves(DomainTypes.DOMAIN_SEA, iMoves)
	showMessage(pPlayer, 'TXT_KEY_MISC_YOU_CIRC_GLOBE', (iMoves,))

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

ROUTE_COST_EXISTING = 1		# a plot with a route at least as good
ROUTE_COST_PER_MOVE = 10	# a plot without it, per movement point of its terrain, feature and hills
ROUTE_COST_RIVER = 30		# crossing a river (a unit without a bridge uses all its moves there)
ROUTE_COST_RIVER_DIAGONAL = 30	# more for crossing a river diagonally, at a corner of the river

def hasRouteAtLeast(pPlot, iRouteValue):
	"The plot has a route at least as good as the value (CvRouteInfo value: road 1, railroad 2)"
	eOldRoute = pPlot.getRouteType()
	return eOldRoute != -1 and gc.getRouteInfo(eOldRoute).getValue() >= iRouteValue

def routeStepCost(pFromPlot, pPlot, iDX, iDY, iRouteValue):
	"The cost of the step to the plot for buildRouteToCapital (see the ROUTE_COST_ constants)"
	if hasRouteAtLeast(pPlot, iRouteValue):
		iCost = ROUTE_COST_EXISTING
	else:
		eFeature = pPlot.getFeatureType()
		if eFeature != -1:
			iMoves = gc.getFeatureInfo(eFeature).getMovementCost()
		else:
			iMoves = gc.getTerrainInfo(pPlot.getTerrainType()).getMovementCost()
		if pPlot.isHills():
			iMoves += gc.getDefineINT("HILLS_EXTRA_MOVEMENT")
		iCost = ROUTE_COST_PER_MOVE * iMoves
	if pFromPlot.isRiverCrossing(directionXY(iDX, iDY)):
		iCost += ROUTE_COST_RIVER
		if iDX != 0 and iDY != 0:
			iCost += ROUTE_COST_RIVER_DIAGONAL
	return iCost

def buildRouteToCapital(pCity, szRoute, iMaxDistance):
	"""Builds the route ('ROUTE_ROAD') from the city to the capital of its owner, e.g. from PyOnCityFounded, when a land
	path of at most iMaxDistance plots joins them. The techs do not matter. The path goes over land of the same area,
	not over impassable plots, and only over plots without owner or of the team of the owner. Of those paths it takes
	the cheapest one, like the path of a worker: rough terrain and river crossings (diagonal ones most) cost more,
	existing routes almost nothing (the ROUTE_COST_ constants). A plot keeps a route at least as good (CvRouteInfo
	value: a railroad stays a railroad). Returns the number of plots that got the route, -1 without a path"""
	pPlayer = gc.getPlayer(pCity.getOwner())
	pCapital = pPlayer.getCapitalCity()
	eRoute = gc.getInfoTypeForString(szRoute)
	if pCapital.isNone() or eRoute == -1:
		return -1
	iRouteValue = gc.getRouteInfo(eRoute).getValue()
	iTeam = pPlayer.getTeam()
	pStart = pCity.plot()
	iStartX = pStart.getX()
	iStartY = pStart.getY()
	iGoalX = pCapital.getX()
	iGoalY = pCapital.getY()
	iArea = pStart.getArea()
	if iStartX == iGoalX and iStartY == iGoalY:
		return 0
	if stepDistance(iStartX, iStartY, iGoalX, iGoalY) > iMaxDistance or pCapital.plot().getArea() != iArea:
		return -1
	# Dijkstra by cost over the states (x, y, number of plots), so that the cheapest path within iMaxDistance plots wins.
	# dCosts[(x, y)] is the list of (number of plots, cost) reached so far; a state is dropped if one with fewer or
	# as many plots costs as little
	dCosts = {(iStartX, iStartY): [(0, 0)]}
	dParent = {}
	lHeap = [(0, 0, iStartX, iStartY)]
	tGoal = None
	while lHeap:
		iCost, iSteps, iX, iY = heapq.heappop(lHeap)
		if iX == iGoalX and iY == iGoalY:
			tGoal = (iX, iY, iSteps)
			break
		pFromPlot = CyMap().plot(iX, iY)
		for iDX in (-1, 0, 1):
			for iDY in (-1, 0, 1):
				if iDX == 0 and iDY == 0:
					continue
				pPlot = plotXY(iX, iY, iDX, iDY)
				if pPlot.isNone() or pPlot.isWater() or pPlot.isImpassable() or pPlot.getArea() != iArea:
					continue
				if pPlot.getOwner() != -1 and pPlot.getTeam() != iTeam:
					continue
				iNewX = pPlot.getX()
				iNewY = pPlot.getY()
				iNewSteps = iSteps + 1
				if iNewSteps + stepDistance(iNewX, iNewY, iGoalX, iGoalY) > iMaxDistance:
					continue
				iNewCost = iCost + routeStepCost(pFromPlot, pPlot, iDX, iDY, iRouteValue)
				lReached = dCosts.setdefault((iNewX, iNewY), [])
				bDominated = False
				for (iOldSteps, iOldCost) in lReached:
					if iOldSteps <= iNewSteps and iOldCost <= iNewCost:
						bDominated = True
						break
				if bDominated:
					continue
				lReached.append((iNewSteps, iNewCost))
				dParent[(iNewX, iNewY, iNewSteps)] = (iX, iY, iSteps)
				heapq.heappush(lHeap, (iNewCost, iNewSteps, iNewX, iNewY))
	if tGoal is None:
		return -1
	iNum = 0
	tNode = tGoal
	while tNode[2] > 0:
		pPlot = CyMap().plot(tNode[0], tNode[1])
		if not pPlot.isCity() and not hasRouteAtLeast(pPlot, iRouteValue):
			pPlot.setRouteType(eRoute)
			iNum += 1
		tNode = dParent[tNode]
	return iNum

def gainTraitForTurns(pPlayer, szTrait, iTurns):
	"The player has the trait for iTurns turns (CyPlayer.setTraitTurns). If it is already temporary, the longer of the two counts (no stacking). Scale by game speed with scaleBySpeed(iTurns, 'GoldenAge') if wanted"
	eTrait = gc.getInfoTypeForString(szTrait)
	if eTrait == -1 or iTurns <= 0:
		return
	pPlayer.setTraitTurns(eTrait, max(pPlayer.getTraitTurns(eTrait), iTurns))

def gainTrait(pPlayer, szTrait):
	"The player gets the trait for good (CyPlayer.changeFreeTraitCount), unless the player already has it. Returns True when it was given now (also used by chooseGreatPeopleByCalendar)"
	eTrait = gc.getInfoTypeForString(szTrait)
	if eTrait == -1 or pPlayer.hasTrait(eTrait):
		return False
	pPlayer.changeFreeTraitCount(eTrait, 1)
	return True

def addTraitCounter(pPlayer, eTrait, iChange):
	"Adds iChange to the counter of the trait (CyPlayer.changeTraitCounter; eTrait of the hook, or the name of a trait as a string) and returns the new value"
	if isinstance(eTrait, str):
		eTrait = gc.getInfoTypeForString(eTrait)
	pPlayer.changeTraitCounter(eTrait, iChange)
	return pPlayer.getTraitCounter(eTrait)

def countEvery(pPlayer, eTrait, iEvery):
	"Adds 1 to the counter of the trait and returns True every iEvery-th time (the counter is then 0 again)"
	if isinstance(eTrait, str):
		eTrait = gc.getInfoTypeForString(eTrait)
	if addTraitCounter(pPlayer, eTrait, 1) >= iEvery:
		pPlayer.setTraitCounter(eTrait, 0)
		return True
	return False

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

## Great people chosen at the end of calendar cycles (chooseGreatPeopleByCalendar, the Maya calendar of Civ5)

# iData1 of CyMessageControl().sendModNetMessage for the choice. The DLL handles some numbers itself and Python never sees
# them (MOD_NET_MESSAGE_* of CvDefines.h): -20261110 (bChooseGoody), -20261125 (bUnsettleGreatPeople), -20261126
# (bIgnoreEvents), -20261128 (LendLease). Do not use those here
MOD_NET_MESSAGE_CHOOSE_GREAT_PERSON = -20261123
# The trait counter of chooseGreatPeopleByCalendar: bits 0-23 are the great people already chosen (the bits of
# greatPeopleUnitClasses), the higher bits the number of choices still to make
GREAT_PERSON_CHOICE_BITS = 24
GREAT_PERSON_CHOICE_MAX_PENDING = 127

def calendarCyclesEnded(iCycleYears, iCycleEndYear):
	"""The number of calendar cycles that end between this game turn and the next one (0 on most turns). A cycle of
	iCycleYears years ends in the year iCycleEndYear and every iCycleYears years before and after it. The trait hooks
	of the turn (PyOnBeginTurn) run before the game turn advances, so the cycle ends with the year of the next turn"""
	if iCycleYears <= 0:
		return 0
	iTurn = CyGame().getGameTurn()
	iYear = CyGame().getTurnYear(iTurn + 1) - iCycleEndYear
	iOldYear = CyGame().getTurnYear(iTurn) - iCycleEndYear
	# The division of Python 2 rounds down negative numbers too
	return max(0, iYear / iCycleYears - iOldYear / iCycleYears)

def greatPeopleUnitClasses():
	"The unit classes whose default unit can join a city as a great person (GreatPeoples of CIV4UnitInfos.xml), in the order of CIV4UnitClassInfos.xml (at most GREAT_PERSON_CHOICE_BITS)"
	lUnitClasses = []
	for iUnitClass in range(gc.getNumUnitClassInfos()):
		eUnit = gc.getUnitClassInfo(iUnitClass).getDefaultUnitIndex()
		if eUnit == -1:
			continue
		kUnit = gc.getUnitInfo(eUnit)
		for iSpecialist in range(gc.getNumSpecialistInfos()):
			if kUnit.getGreatPeoples(iSpecialist):
				lUnitClasses.append(iUnitClass)
				break
		if len(lUnitClasses) >= GREAT_PERSON_CHOICE_BITS:
			break
	return lUnitClasses

def greatPersonChoiceState(pPlayer, eTrait):
	"(the bits of the great people already chosen, the number of choices still to make) from the trait counter"
	iCounter = pPlayer.getTraitCounter(eTrait)
	return (iCounter & ((1 << GREAT_PERSON_CHOICE_BITS) - 1), iCounter >> GREAT_PERSON_CHOICE_BITS)

def setGreatPersonChoiceState(pPlayer, eTrait, iChosen, iPending):
	pPlayer.setTraitCounter(eTrait, iChosen | (min(iPending, GREAT_PERSON_CHOICE_MAX_PENDING) << GREAT_PERSON_CHOICE_BITS))

def greatPeopleOfCivilization(pPlayer):
	"The bits of greatPeopleUnitClasses whose class has a unit in the civilization of the player"
	lUnitClasses = greatPeopleUnitClasses()
	iAll = 0
	for iBit in range(len(lUnitClasses)):
		if pPlayer.getCivilizationUnit(lUnitClasses[iBit]) != -1:
			iAll |= (1 << iBit)
	return iAll

def greatPersonChoices(pPlayer, eTrait):
	"The bits of the great people the player can choose now (greatPeopleOfCivilization not chosen yet). When all of them are chosen, all of them again"
	iAll = greatPeopleOfCivilization(pPlayer)
	iChosen = greatPersonChoiceState(pPlayer, eTrait)[0]
	if iAll & ~iChosen == 0:
		return iAll
	return iAll & ~iChosen

def chooseGreatPeopleByCalendar(pPlayer, eTrait, szTech, iCycleYears, iCycleEndYear, szTechTrait = None):
	"""For PyOnBeginTurn. Once the team has the tech szTech (None: from the start), each time a calendar cycle ends
	(calendarCyclesEnded: iCycleYears years, one of them ending in iCycleEndYear), the player chooses a great person who
	appears in the capital: a human player in a popup, the AI at random. A great person chosen once cannot be chosen
	again until all of them have been chosen (greatPersonChoices). Uses the counter of the trait. A choice not made yet
	(no capital, or the game was loaded before the popup was answered) is offered again at the start of the next turn.
	With szTechTrait, the player also gets that trait for good once the team has the tech (gainTrait; e.g. a
	trait with bLongCountCalendar, so the calendar changes with the same tech). Returns the number of cycles that ended"""
	iCycles = 0
	if szTech is None or szTech == '' or gc.getTeam(pPlayer.getTeam()).isHasTech(gc.getInfoTypeForString(szTech)):
		iCycles = calendarCyclesEnded(iCycleYears, iCycleEndYear)
		if szTechTrait is not None:
			gainTrait(pPlayer, szTechTrait)
	(iChosen, iPending) = greatPersonChoiceState(pPlayer, eTrait)
	if iCycles > 0:
		iPending += iCycles
		setGreatPersonChoiceState(pPlayer, eTrait, iChosen, iPending)
	if iPending > 0:
		offerGreatPersonChoice(pPlayer, eTrait)
	return iCycles

def offerGreatPersonChoice(pPlayer, eTrait):
	"A human player gets the popup of chooseGreatPeopleByCalendar (one at a time), the AI chooses all pending great people at random. Nothing without a capital"
	if capitalPlot(pPlayer) is None:
		return
	if pPlayer.isHuman():
		iChoices = greatPersonChoices(pPlayer, eTrait)
		lUnitClasses = greatPeopleUnitClasses()
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
		popupInfo.setText(CyTranslator().getText("TXT_KEY_POPUP_CHOOSE_GREAT_PERSON", ()))
		popupInfo.setData1(eTrait)
		popupInfo.setFlags(iChoices)
		# The exe finds the callback only in a module of EntryPoints: CvTraitInterface imports everything here
		popupInfo.setPythonModule("CvTraitInterface")
		popupInfo.setOnClickedPythonCallback("chooseGreatPersonCallback")
		for iBit in range(len(lUnitClasses)):
			if iChoices & (1 << iBit):
				kUnit = gc.getUnitInfo(pPlayer.getCivilizationUnit(lUnitClasses[iBit]))
				popupInfo.addPythonButton(kUnit.getDescription(), kUnit.getButton())
		popupInfo.addPopup(pPlayer.getID())
		return
	(iChosen, iPending) = greatPersonChoiceState(pPlayer, eTrait)
	for i in range(iPending):
		iChoices = greatPersonChoices(pPlayer, eTrait)
		lBits = []
		for iBit in range(GREAT_PERSON_CHOICE_BITS):
			if iChoices & (1 << iBit):
				lBits.append(iBit)
		if len(lBits) == 0:
			return
		applyGreatPersonChoice(pPlayer, eTrait, lBits[CyGame().getSorenRandNum(len(lBits), "Great person of the calendar")])

def chooseGreatPersonCallback(argsList):
	"The button of the popup of offerGreatPersonChoice (only on the computer of the player): the choice goes to every computer (receiveModNetMessage)"
	iButton = argsList[0]
	eTrait = argsList[1]
	iChoices = argsList[4]
	for iBit in range(GREAT_PERSON_CHOICE_BITS):
		if iChoices & (1 << iBit):
			if iButton == 0:
				CyMessageControl().sendModNetMessage(MOD_NET_MESSAGE_CHOOSE_GREAT_PERSON, CyGame().getActivePlayer(), eTrait, iBit, 0)
				return
			iButton -= 1

def applyGreatPersonChoice(pPlayer, eTrait, iBit):
	"The great person of the bit appears in the capital, if the player still has a choice to make and can choose it. Returns True if it appeared"
	if not pPlayer.isAlive() or not pPlayer.hasTrait(eTrait):
		return False
	(iChosen, iPending) = greatPersonChoiceState(pPlayer, eTrait)
	pPlot = capitalPlot(pPlayer)
	if iPending <= 0 or pPlot is None or iBit < 0 or not (greatPersonChoices(pPlayer, eTrait) & (1 << iBit)):
		return False
	eUnit = pPlayer.getCivilizationUnit(greatPeopleUnitClasses()[iBit])
	pPlayer.initUnit(eUnit, pPlot.getX(), pPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
	iChosen |= (1 << iBit)
	if greatPeopleOfCivilization(pPlayer) & ~iChosen == 0:
		# All great people have been chosen: all of them can be chosen again
		iChosen = 0
	setGreatPersonChoiceState(pPlayer, eTrait, iChosen, iPending - 1)
	kUnit = gc.getUnitInfo(eUnit)
	szText = CyTranslator().getText("TXT_KEY_MISC_CALENDAR_GREAT_PERSON", (kUnit.getDescription(), pPlayer.getCapitalCity().getNameKey()))
	CyInterface().addMessage(pPlayer.getID(), True, gc.getEVENT_MESSAGE_TIME(), szText, "AS2D_UNIT_GREATPEOPLE", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, kUnit.getButton(), gc.getInfoTypeForString("COLOR_UNIT_TEXT"), pPlot.getX(), pPlot.getY(), True, True)
	return True

## Treaty zones: the half of the map east or west of the capital (the Treaty of Tordesillas)

# iData1 of CyMessageControl().sendModNetMessage for the choices of the treaty zone and of its terms
MOD_NET_MESSAGE_CHOOSE_TREATY_ZONE = -20261124
MOD_NET_MESSAGE_CHOOSE_TREATY_TERMS = -20261127
# The trait counter of chooseTreatyZone: the side + TREATY_ZONE_SIDES * (the x of the line through the capital)
TREATY_ZONE_NONE = 0
TREATY_ZONE_WEST = 1
TREATY_ZONE_EAST = 2
TREATY_ZONE_PENDING = 3
TREATY_ZONE_SIDES = 4
# The terms of offerTreatyTerms (the bits of the flags of its popup)
TREATY_TERMS_GOLD = 0
TREATY_TERMS_TECH = 1
TREATY_TERMS_BUILDING = 2
NUM_TREATY_TERMS = 3
# The gold and the building in one int of the popup and of the net message of offerTreatyTerms
TREATY_TERMS_GOLD_LIMIT = 100000
# The icons of the buttons of offerTreatyTerms (the building uses its own)
TREATY_TERMS_GOLD_BUTTON = "Art/Interface/Buttons/Process/ProcessWealth.dds"
TREATY_TERMS_TECH_BUTTON = "Art/Interface/Buttons/Process/ProcessResearch.dds"
# The overlay of updateTreatyZonePlots
TREATY_ZONE_PLOT_ALPHA = 0.35

def treatyZone(pPlayer, eTrait):
	"(the side: TREATY_ZONE_NONE, _WEST, _EAST or _PENDING, the x of the line) from the trait counter"
	iCounter = pPlayer.getTraitCounter(eTrait)
	return (iCounter % TREATY_ZONE_SIDES, iCounter / TREATY_ZONE_SIDES)

def setTreatyZone(pPlayer, eTrait, iSide, iX):
	pPlayer.setTraitCounter(eTrait, iSide + TREATY_ZONE_SIDES * iX)

def isTreatyZoneSet(iSide):
	return iSide == TREATY_ZONE_WEST or iSide == TREATY_ZONE_EAST

def isInTreatyZone(iSide, iX, pPlot):
	"""The plot is in the half of the map on the side (TREATY_ZONE_WEST or _EAST) of the line at the x iX. On a map that
	wraps east and west, each half has the line and half of the other columns: the column of the line (and the one on the
	other side of the globe) belongs to both. On a map that does not wrap, the line splits the map where it is"""
	if not isTreatyZoneSet(iSide):
		return False
	iWidth = CyMap().getGridWidth()
	if CyMap().isWrapX():
		iDX = (pPlot.getX() - iX) % iWidth
		if iSide == TREATY_ZONE_EAST:
			return iDX <= iWidth / 2
		return iDX == 0 or iDX >= iWidth - iWidth / 2
	if iSide == TREATY_ZONE_EAST:
		return pPlot.getX() >= iX
	return pPlot.getX() <= iX

def countTreatyZoneLand(iSide, iX):
	"The number of land plots in the half of the map"
	iCount = 0
	for i in range(CyMap().numPlots()):
		pPlot = CyMap().plotByIndex(i)
		if not pPlot.isWater() and isInTreatyZone(iSide, iX, pPlot):
			iCount += 1
	return iCount

def treatyZoneSideName(iSide):
	if iSide == TREATY_ZONE_EAST:
		return CyTranslator().getText("TXT_KEY_TREATY_ZONE_EAST", ())
	return CyTranslator().getText("TXT_KEY_TREATY_ZONE_WEST", ())

def chooseTreatyZone(pPlayer, eTrait):
	"""For PyOnProjectBuilt (or any hook, once): the line through the capital of the player is fixed now, and the player
	chooses the west or the east half of the map as its zone: a human player in a popup, the AI the half with more land
	(countTreatyZoneLand). Uses the counter of the trait. Nothing without a capital or when the zone is already set.
	A choice not made yet (the game was loaded before the popup was answered) is offered again by treatyZoneTurn"""
	pPlot = capitalPlot(pPlayer)
	if pPlot is None or treatyZone(pPlayer, eTrait)[0] != TREATY_ZONE_NONE:
		return
	setTreatyZone(pPlayer, eTrait, TREATY_ZONE_PENDING, pPlot.getX())
	offerTreatyZoneChoice(pPlayer, eTrait)

def offerTreatyZoneChoice(pPlayer, eTrait):
	(iSide, iX) = treatyZone(pPlayer, eTrait)
	if iSide != TREATY_ZONE_PENDING:
		return
	if pPlayer.isHuman():
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
		popupInfo.setText(CyTranslator().getText("TXT_KEY_POPUP_CHOOSE_TREATY_ZONE", ()))
		popupInfo.setData1(eTrait)
		# The exe finds the callback only in a module of EntryPoints: CvTraitInterface imports everything here
		popupInfo.setPythonModule("CvTraitInterface")
		popupInfo.setOnClickedPythonCallback("chooseTreatyZoneCallback")
		popupInfo.addPythonButton(treatyZoneSideName(TREATY_ZONE_WEST), "")
		popupInfo.addPythonButton(treatyZoneSideName(TREATY_ZONE_EAST), "")
		popupInfo.addPopup(pPlayer.getID())
		return
	if countTreatyZoneLand(TREATY_ZONE_EAST, iX) > countTreatyZoneLand(TREATY_ZONE_WEST, iX):
		applyTreatyZoneChoice(pPlayer, eTrait, TREATY_ZONE_EAST)
	else:
		applyTreatyZoneChoice(pPlayer, eTrait, TREATY_ZONE_WEST)

def chooseTreatyZoneCallback(argsList):
	"The button of the popup of offerTreatyZoneChoice (only on the computer of the player): 0 west, 1 east"
	iButton = argsList[0]
	eTrait = argsList[1]
	iSide = TREATY_ZONE_WEST
	if iButton == 1:
		iSide = TREATY_ZONE_EAST
	CyMessageControl().sendModNetMessage(MOD_NET_MESSAGE_CHOOSE_TREATY_ZONE, CyGame().getActivePlayer(), eTrait, iSide, 0)

def applyTreatyZoneChoice(pPlayer, eTrait, iSide):
	"The zone of the player becomes the side, if the choice is still to make. Every player gets a message. Returns True if it was set"
	if not pPlayer.isAlive() or not pPlayer.hasTrait(eTrait) or not isTreatyZoneSet(iSide):
		return False
	(iOldSide, iX) = treatyZone(pPlayer, eTrait)
	if iOldSide != TREATY_ZONE_PENDING:
		return False
	setTreatyZone(pPlayer, eTrait, iSide, iX)
	szText = CyTranslator().getText("TXT_KEY_MISC_TREATY_ZONE_CHOSEN", (pPlayer.getCivilizationShortDescription(0), treatyZoneSideName(iSide)))
	for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
		if gc.getPlayer(iLoopPlayer).isAlive():
			CyInterface().addMessage(iLoopPlayer, True, gc.getEVENT_MESSAGE_TIME(), szText, "", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, gc.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)
	return True

def isTreatyZoneCity(pPlayer, eTrait, pCity):
	"The city is in the zone of the player and belongs to another player who is not a barbarian"
	(iSide, iX) = treatyZone(pPlayer, eTrait)
	if pCity is None or pCity.isNone() or not isTreatyZoneSet(iSide):
		return False
	if pCity.getOwner() == pPlayer.getID() or gc.getPlayer(pCity.getOwner()).isBarbarian():
		return False
	return isInTreatyZone(iSide, iX, pCity.plot())

def treatyZoneTurn(pPlayer, eTrait, iGoldPerCity, szSource = None):
	"""For PyOnBeginTurn: the player gains iGoldPerCity gold for each city of the other players in its zone (isTreatyZoneCity:
	whoever founded it and whenever), with a message "From <source>: +N gold" (TXT_KEY_MISC_TREATY_ZONE_INCOME). szSource is
	the type of the info named in the message, e.g. 'PROJECT_MANHATTAN_PROJECT' (None: the trait). A zone still to choose is
	offered again. Returns the number of cities"""
	(iSide, iX) = treatyZone(pPlayer, eTrait)
	if iSide == TREATY_ZONE_PENDING:
		offerTreatyZoneChoice(pPlayer, eTrait)
		return 0
	if not isTreatyZoneSet(iSide):
		return 0
	iCount = 0
	for iLoopPlayer in range(gc.getMAX_CIV_PLAYERS()):
		pLoopPlayer = gc.getPlayer(iLoopPlayer)
		if iLoopPlayer == pPlayer.getID() or not pLoopPlayer.isAlive():
			continue
		(pLoopCity, iIter) = pLoopPlayer.firstCity(False)
		while pLoopCity is not None and not pLoopCity.isNone():
			if isInTreatyZone(iSide, iX, pLoopCity.plot()):
				iCount += 1
			(pLoopCity, iIter) = pLoopPlayer.nextCity(iIter, False)
	if iCount > 0 and iGoldPerCity != 0:
		pPlayer.changeGold(iCount * iGoldPerCity)
		szName = gc.getTraitInfo(eTrait).getDescription()
		if szSource is not None and gc.getInfoTypeForString(szSource) != -1:
			szName = getInfoDescriptionByType(szSource)
		showMessage(pPlayer, 'TXT_KEY_MISC_TREATY_ZONE_INCOME', (szName, iCount * iGoldPerCity, iCount))
	return iCount

def getInfoDescriptionByType(szType):
	"The name of the info of the type (a project, building, trait, civic or tech: the class from the prefix PROJECT_ etc.)"
	iType = gc.getInfoTypeForString(szType)
	for (szPrefix, szInfo) in (('PROJECT_', 'Project'), ('BUILDING_', 'Building'), ('TRAIT_', 'Trait'), ('CIVIC_', 'Civic'), ('TECH_', 'Tech')):
		if szType.startswith(szPrefix):
			return infoDescription(szInfo, iType)
	return szType

def treatyTermsTechs(pPlayer, eFounder):
	"The techs of offerTreatyTerms: the team of the founder has them and the team of the player does not. The techs the player can research now if there are some"
	pTeam = gc.getTeam(pPlayer.getTeam())
	pFounderTeam = gc.getTeam(gc.getPlayer(eFounder).getTeam())
	lTechs = []
	lResearchable = []
	for eTech in range(gc.getNumTechInfos()):
		if pFounderTeam.isHasTech(eTech) and not pTeam.isHasTech(eTech):
			lTechs.append(eTech)
			if pPlayer.canResearch(eTech, False):
				lResearchable.append(eTech)
	if len(lResearchable) > 0:
		return lResearchable
	return lTechs

def treatyTermsBuilding(szBuildingClass):
	"The building of offerTreatyTerms: the default building of the class (-1 if none)"
	iBuildingClass = gc.getInfoTypeForString(szBuildingClass)
	if iBuildingClass == -1:
		return -1
	return gc.getBuildingClassInfo(iBuildingClass).getDefaultBuildingIndex()

def treatyTermsChoices(pPlayer, pCity, iGold, eBuilding):
	"The bits of the terms the founder can choose now"
	pFounder = gc.getPlayer(pCity.getOwner())
	iChoices = 0
	if iGold > 0 and pFounder.getGold() >= iGold:
		iChoices |= (1 << TREATY_TERMS_GOLD)
	if len(treatyTermsTechs(pPlayer, pCity.getOwner())) > 0:
		iChoices |= (1 << TREATY_TERMS_TECH)
	if eBuilding != -1:
		iChoices |= (1 << TREATY_TERMS_BUILDING)
	return iChoices

def offerTreatyTerms(pPlayer, eTrait, pCity, iGold, szBuildingClass):
	"""For PyOnOtherCityFounded: when another player (not a barbarian, not of the same team) founds the city in the zone of the
	player (chooseTreatyZone), the founder chooses one of the terms: pay iGold gold to the player (only with that much gold),
	give the player a random tech the founder's team has and the player's team does not (treatyTermsTechs), or the city gets
	the building of szBuildingClass. A human founder chooses in a popup, the AI at random. The popup is not offered again
	after loading a game. Returns True if the terms were offered"""
	if not isTreatyZoneCity(pPlayer, eTrait, pCity):
		return False
	pFounder = gc.getPlayer(pCity.getOwner())
	if pFounder.getTeam() == pPlayer.getTeam():
		return False
	iGold = max(0, min(iGold, TREATY_TERMS_GOLD_LIMIT - 1))
	eBuilding = treatyTermsBuilding(szBuildingClass)
	iChoices = treatyTermsChoices(pPlayer, pCity, iGold, eBuilding)
	if iChoices == 0:
		return False
	if pFounder.isHuman():
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
		popupInfo.setText(CyTranslator().getText("TXT_KEY_POPUP_TREATY_TERMS", (pCity.getNameKey(), pPlayer.getCivilizationShortDescription(0))))
		popupInfo.setData1(pPlayer.getID())
		popupInfo.setData2(pCity.getID())
		popupInfo.setData3(iGold + TREATY_TERMS_GOLD_LIMIT * (eBuilding + 1))
		popupInfo.setFlags(iChoices)
		popupInfo.setPythonModule("CvTraitInterface")
		popupInfo.setOnClickedPythonCallback("chooseTreatyTermsCallback")
		if iChoices & (1 << TREATY_TERMS_GOLD):
			popupInfo.addPythonButton(CyTranslator().getText("TXT_KEY_TREATY_TERMS_GOLD", (iGold, pPlayer.getCivilizationShortDescription(0))), TREATY_TERMS_GOLD_BUTTON)
		if iChoices & (1 << TREATY_TERMS_TECH):
			popupInfo.addPythonButton(CyTranslator().getText("TXT_KEY_TREATY_TERMS_TECH", (pPlayer.getCivilizationShortDescription(0),)), TREATY_TERMS_TECH_BUTTON)
		if iChoices & (1 << TREATY_TERMS_BUILDING):
			kBuilding = gc.getBuildingInfo(eBuilding)
			popupInfo.addPythonButton(CyTranslator().getText("TXT_KEY_TREATY_TERMS_BUILDING", (kBuilding.getDescription(),)), kBuilding.getButton())
		popupInfo.addPopup(pFounder.getID())
		return True
	lTerms = []
	for iTerms in range(NUM_TREATY_TERMS):
		if iChoices & (1 << iTerms):
			lTerms.append(iTerms)
	applyTreatyTerms(pPlayer, pCity, lTerms[CyGame().getSorenRandNum(len(lTerms), "Treaty terms")], iGold, eBuilding)
	return True

def chooseTreatyTermsCallback(argsList):
	"The button of the popup of offerTreatyTerms (only on the computer of the founder): the choice goes to every computer"
	iButton = argsList[0]
	ePlayer = argsList[1]
	iCityID = argsList[2]
	iGoldAndBuilding = argsList[3]
	iChoices = argsList[4]
	for iTerms in range(NUM_TREATY_TERMS):
		if iChoices & (1 << iTerms):
			if iButton == 0:
				# iData5: the terms + NUM_TREATY_TERMS * (the gold + TREATY_TERMS_GOLD_LIMIT * (the building + 1))
				CyMessageControl().sendModNetMessage(MOD_NET_MESSAGE_CHOOSE_TREATY_TERMS, CyGame().getActivePlayer(), ePlayer, iCityID, iTerms + NUM_TREATY_TERMS * iGoldAndBuilding)
				return
			iButton -= 1

def applyTreatyTerms(pPlayer, pCity, iTerms, iGold, eBuilding):
	"The founder of the city fulfils the terms of offerTreatyTerms, if it still can. Both players get a message. Returns True if it did"
	if pCity is None or pCity.isNone() or not pPlayer.isAlive() or eBuilding < -1 or eBuilding >= gc.getNumBuildingInfos():
		return False
	eFounder = pCity.getOwner()
	pFounder = gc.getPlayer(eFounder)
	if iTerms < 0 or iTerms >= NUM_TREATY_TERMS or not (treatyTermsChoices(pPlayer, pCity, iGold, eBuilding) & (1 << iTerms)):
		return False
	szCiv = pPlayer.getCivilizationShortDescription(0)
	szFounder = pFounder.getCivilizationShortDescription(0)
	if iTerms == TREATY_TERMS_GOLD:
		pFounder.changeGold(-iGold)
		pPlayer.changeGold(iGold)
		szText = CyTranslator().getText("TXT_KEY_MISC_TREATY_TERMS_GOLD", (szFounder, iGold, szCiv, pCity.getNameKey()))
	elif iTerms == TREATY_TERMS_TECH:
		lTechs = treatyTermsTechs(pPlayer, eFounder)
		eTech = lTechs[CyGame().getSorenRandNum(len(lTechs), "Treaty terms tech")]
		gc.getTeam(pPlayer.getTeam()).setHasTech(eTech, True, pPlayer.getID(), False, False)
		szText = CyTranslator().getText("TXT_KEY_MISC_TREATY_TERMS_TECH", (szFounder, gc.getTechInfo(eTech).getDescription(), szCiv, pCity.getNameKey()))
	else:
		pCity.setNumRealBuilding(eBuilding, 1)
		szText = CyTranslator().getText("TXT_KEY_MISC_TREATY_TERMS_BUILDING", (szFounder, pCity.getNameKey(), gc.getBuildingInfo(eBuilding).getDescription(), szCiv))
	for iLoopPlayer in (pPlayer.getID(), eFounder):
		CyInterface().addMessage(iLoopPlayer, True, gc.getEVENT_MESSAGE_TIME(), szText, "", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, gc.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), pCity.getX(), pCity.getY(), True, True)
	return True

def treatyZones():
	"[(pPlayer, iSide, iX)] of the players alive whose trait with treatyZoneTurn in PyOnBeginTurn has its zone set"
	lZones = []
	for eTrait in range(gc.getNumTraitInfos()):
		if gc.getTraitInfo(eTrait).getPyOnBeginTurn().find('treatyZoneTurn') < 0:
			continue
		for iPlayer in range(gc.getMAX_CIV_PLAYERS()):
			pPlayer = gc.getPlayer(iPlayer)
			if pPlayer.isAlive() and pPlayer.hasTrait(eTrait):
				(iSide, iX) = treatyZone(pPlayer, eTrait)
				if isTreatyZoneSet(iSide):
					lZones.append((pPlayer, iSide, iX))
	return lZones

def updateTreatyZonePlots():
	"""From CvGameUtils.updateColoredPlots (the DLL has just cleared the layer): while the active player has a unit that can
	found cities selected, the revealed land plots of the treaty zones are colored with the main color of the player of the
	zone. Returns False (the DLL draws its own plots after it)"""
	pUnit = CyInterface().getHeadSelectedUnit()
	if pUnit is None or pUnit.isNone() or not pUnit.isFound():
		return False
	lZones = treatyZones()
	if len(lZones) == 0:
		return False
	lColors = []
	for (pPlayer, iSide, iX) in lZones:
		lColors.append(gc.getColorInfo(gc.getPlayerColorInfo(pPlayer.getPlayerColor()).getColorTypePrimary()).getType())
	iActiveTeam = CyGame().getActiveTeam()
	for i in range(CyMap().numPlots()):
		pPlot = CyMap().plotByIndex(i)
		if pPlot.isWater() or not pPlot.isRevealed(iActiveTeam, False):
			continue
		for iZone in range(len(lZones)):
			(pPlayer, iSide, iX) = lZones[iZone]
			if isInTreatyZone(iSide, iX, pPlot):
				CyEngine().addColoredPlotAlt(pPlot.getX(), pPlot.getY(), PlotStyles.PLOT_STYLE_BOX_FILL, PlotLandscapeLayers.PLOT_LANDSCAPE_LAYER_BASE, lColors[iZone], TREATY_ZONE_PLOT_ALPHA)
				break
	return False

def receiveModNetMessage(iData1, iData2, iData3, iData4, iData5):
	"The messages of this file from CvEventManager.onModNetMessage (on every computer). Returns True if the message was one of them"
	if iData1 == MOD_NET_MESSAGE_CHOOSE_GREAT_PERSON:
		# iData2: player, iData3: trait, iData4: bit of greatPeopleUnitClasses
		if iData2 >= 0 and iData2 < gc.getMAX_PLAYERS() and iData3 >= 0 and iData3 < gc.getNumTraitInfos():
			pPlayer = gc.getPlayer(iData2)
			if applyGreatPersonChoice(pPlayer, iData3, iData4):
				if greatPersonChoiceState(pPlayer, iData3)[1] > 0:
					offerGreatPersonChoice(pPlayer, iData3)
		return True
	if iData1 == MOD_NET_MESSAGE_CHOOSE_TREATY_ZONE:
		# iData2: player, iData3: trait, iData4: side
		if iData2 >= 0 and iData2 < gc.getMAX_PLAYERS() and iData3 >= 0 and iData3 < gc.getNumTraitInfos():
			applyTreatyZoneChoice(gc.getPlayer(iData2), iData3, iData4)
		return True
	if iData1 == MOD_NET_MESSAGE_CHOOSE_TREATY_TERMS:
		# iData2: founder, iData3: player of the zone, iData4: city of the founder, iData5: the terms + NUM_TREATY_TERMS *
		# (the gold + TREATY_TERMS_GOLD_LIMIT * (the building + 1))
		if iData2 >= 0 and iData2 < gc.getMAX_PLAYERS() and iData3 >= 0 and iData3 < gc.getMAX_PLAYERS() and iData5 >= 0:
			pCity = gc.getPlayer(iData2).getCity(iData4)
			iGoldAndBuilding = iData5 / NUM_TREATY_TERMS
			applyTreatyTerms(gc.getPlayer(iData3), pCity, iData5 % NUM_TREATY_TERMS, iGoldAndBuilding % TREATY_TERMS_GOLD_LIMIT, iGoldAndBuilding / TREATY_TERMS_GOLD_LIMIT - 1)
		return True
	return False
