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

def startGoldenAge(pPlayer):
	"The player starts a golden age of the usual length (like a free golden age: longer if one is going on), without great people"
	pPlayer.changeGoldenAgeTurns(pPlayer.getGoldenAgeLength())

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

# iData1 of CyMessageControl().sendModNetMessage for the choice (MOD_NET_MESSAGE_CHOOSE_GOODY of the DLL is -20261110)
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

def chooseGreatPeopleByCalendar(pPlayer, eTrait, szTech, iCycleYears, iCycleEndYear):
	"""For PyOnBeginTurn. Once the team has the tech szTech (None: from the start), each time a calendar cycle ends
	(calendarCyclesEnded: iCycleYears years, one of them ending in iCycleEndYear), the player chooses a great person who
	appears in the capital: a human player in a popup, the AI at random. A great person chosen once cannot be chosen
	again until all of them have been chosen (greatPersonChoices). Uses the counter of the trait. A choice not made yet
	(no capital, or the game was loaded before the popup was answered) is offered again at the start of the next turn.
	Returns the number of cycles that ended"""
	iCycles = 0
	if szTech is None or szTech == '' or gc.getTeam(pPlayer.getTeam()).isHasTech(gc.getInfoTypeForString(szTech)):
		iCycles = calendarCyclesEnded(iCycleYears, iCycleEndYear)
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
	return False
