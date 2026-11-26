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
##   PyOnCorporationFounded  pPlayer, eTrait, pCity (the headquarters), eCorporation: after the player founds the
##                     corporation, e.g. createExecutives(pPlayer, eCorporation, 3, pCity)
##   PyOnCityLost      pPlayer, eTrait, pCity (now of the new owner), iNewOwner, bConquest, bTrade (1 or 0), iPopulation,
##                     iCultureLevel (the population and the culture level before the city was lost): after the player
##                     loses the city by conquest, trade or culture. A conquered city is razed, if it is, only afterwards,
##                     so this is also the hook for a razed city, e.g.
##                     bConquest and createPartisans(pPlayer, pCity, 2 + iPopulation, iCultureLevel)
##   PyOnBuildingBuilt pPlayer, eTrait, pCity, eBuilding: after a city of the player completes the building by production
##                     (the same buildings as PyOnBuilt of CIV4BuildingInfos.xml: not the free ones), e.g.
##                     isBuildingClassIn(eBuilding, ['BUILDINGCLASS_GRANARY']) and pCity.changePopulation(1)
##   PyOnReligionFounded  pPlayer, eTrait, pCity (the holy city), eReligion: after the player founds the religion (after
##                     the free missionary of the religion, if any), e.g. createMissionaries(pPlayer, eReligion, 2, pCity)
##   PyOnGained        pPlayer, eTrait: when the player gains this trait during the game (conditions of
##                     ConditionalFreeTraits, FreeTraits of civics, buildings and techs, temporary traits, Python).
##                     Not for the traits the player has when the game starts
##   PyOnLost          pPlayer, eTrait: when the player loses this trait (the player no longer has it)
##   PyOnTechAcquired  pPlayer, eTrait, eTech, bFirst (1 if the team of the player is the first to have it): after the team
##                     gets the tech, however it got it (research, trade, goody hut, espionage, Python). Not for the techs
##                     of the start of the game, e.g. bFirst and gainGold(pPlayer, 50)
##   PyOnGreatPersonBorn  pPlayer, eTrait, pUnit (CyUnit), pCity: after a great person of the player is born. A great
##                     general from combat has no city (pCity.isNone()), e.g.
##                     isUnitClassIn(pUnit, ['UNITCLASS_GREAT_GENERAL']) and gainTraitForTurns(pPlayer, 'TRAIT_X', 5)
##   PyOnGoldenAgeStart / PyOnGoldenAgeEnd  pPlayer, eTrait: when a golden age of the player starts / ends (not when a
##                     golden age only gets longer)
##   PyOnWarDeclared   pPlayer, eTrait, iOtherTeam, bAggressor (1 if the team of the player declared the war): after a war
##                     between the teams starts. A defensive pact or a vassal that joins a war declares it too
##   PyOnPeaceMade     pPlayer, eTrait, iOtherTeam: after the teams make peace
##   PyOnEraChanged    pPlayer, eTrait, eEra, iOldEra: after the player enters a new era (it can skip eras when it gets
##                     a tech of a later era). Not for the era of the start of the game
##   PyOnFirstContact  pPlayer, eTrait, iOtherTeam: after the team of the player meets another team (by units or a
##                     contact trade; not the barbarians)
##   PyOnProjectBuilt  pPlayer, eTrait, pCity, eProject: after a city of the player completes the project
##   PyOnVassalGained  pPlayer, eTrait, iVassalTeam, bCapitulated (1 if the vassal surrendered): after a team becomes a
##                     vassal of the team of the player
##   PyOnReligionSpread  pPlayer, eTrait, pCity, eReligion: after a religion spreads to a city of the player (natural
##                     spread, missionaries, the holy city, events). Not for the religions a city keeps when it changes hands
##   PyOnCivicChanged  pPlayer, eTrait, eCivic, iOldCivic: after the player changes the civic of a slot, once per slot
##                     (eCivic or iOldCivic is -1 for an empty extra slot of CivicOptionExtraSlots)
##   PyOnCircumnavigated  pPlayer, eTrait, iOrder (1 if the team of the player is the first to circumnavigate the globe,
##                     2 if the second...): after the team circumnavigates the globe (the first team has its bonus moves
##                     of the ships by then; a later team gets nothing else), e.g. iOrder == 1 and startGoldenAge(pPlayer),
##                     iOrder > 1 and gainCircumnavigationMoves(pPlayer)
##   PyOnOtherCityFounded  pPlayer, eTrait, pCity (CyCity of the founder): after another player (any player alive but this
##                     one, the barbarians too) founds a city, e.g. offerTreatyTerms(pPlayer, eTrait, pCity, 100,
##                     'BUILDINGCLASS_X')
##   PyOnPlayerEliminated  pPlayer, eTrait, iEliminatedPlayer: after a player whose last city this player conquered is
##                     eliminated (not when the last city was razed by someone else or lost by culture), e.g.
##                     startGoldenAge(pPlayer)
##   PyOnImprovementChanged  pPlayer, eTrait, pPlot (CyPlot), eOld, eNew (improvement types, -1 for none): after the
##                     improvement of a plot of the player changes (built by workers, upgraded like a cottage, pillaged,
##                     events), e.g. replacing a farm by a workshop:
##                     gainProduction(pPlot.getWorkingCity(), improvementReplacedValue(eOld, eNew, 'IMPROVEMENT_WORKSHOP',
##                     {'IMPROVEMENT_FARM': scaleBySpeed(30, 'Train')}))
## Counters: each player has a number per trait for the hooks (CyPlayer.getTraitCounter, saved with the game), e.g.
##   countEvery(pPlayer, eTrait, 3) and gainGold(pPlayer, 100)   (every third time)
## Temporary traits: gainTraitForTurns(pPlayer, 'TRAIT_X', iTurns) gives a trait for some turns, e.g. from a hook.
## To do two things in one hook, write a tuple: (addBuilding(pCity, 'BUILDINGCLASS_WALLS'), gainGold(pPlayer, 10))
## The common functions are in CvHookUtils.py (imported below), e.g.
##   <PyOnGameStart>gainGold(pPlayer, scaleBySpeed(100, 'Train'))</PyOnGameStart>
## The hooks change the game and are called on every computer of a multiplayer game: use CyGame().getSorenRandNum for
## random numbers. Describe the effects in the Help of the trait (the trait text shows it).
## The hooks of buildings, promotions, techs, civics and units are in CvHookInterface.py.

from CvPythonExtensions import *
from CvHookUtils import *

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

def onCorporationFounded(argsList):
	pPlayer, eTrait, pCity, eCorporation = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnCorporationFounded())

def onCityLost(argsList):
	pPlayer, eTrait, pCity, iNewOwner, bConquest, bTrade, iPopulation, iCultureLevel = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnCityLost())

def onBuildingBuilt(argsList):
	pPlayer, eTrait, pCity, eBuilding = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnBuildingBuilt())

def onReligionFounded(argsList):
	pPlayer, eTrait, pCity, eReligion = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnReligionFounded())

def onGained(argsList):
	pPlayer, eTrait = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnGained())

def onLost(argsList):
	pPlayer, eTrait = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnLost())

def onTechAcquired(argsList):
	pPlayer, eTrait, eTech, bFirst = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnTechAcquired())

def onGreatPersonBorn(argsList):
	pPlayer, eTrait, pUnit, pCity = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnGreatPersonBorn())

def onGoldenAgeStart(argsList):
	pPlayer, eTrait = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnGoldenAgeStart())

def onGoldenAgeEnd(argsList):
	pPlayer, eTrait = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnGoldenAgeEnd())

def onWarDeclared(argsList):
	pPlayer, eTrait, iOtherTeam, bAggressor = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnWarDeclared())

def onPeaceMade(argsList):
	pPlayer, eTrait, iOtherTeam = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnPeaceMade())

def onEraChanged(argsList):
	pPlayer, eTrait, eEra, iOldEra = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnEraChanged())

def onFirstContact(argsList):
	pPlayer, eTrait, iOtherTeam = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnFirstContact())

def onProjectBuilt(argsList):
	pPlayer, eTrait, pCity, eProject = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnProjectBuilt())

def onVassalGained(argsList):
	pPlayer, eTrait, iVassalTeam, bCapitulated = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnVassalGained())

def onReligionSpread(argsList):
	pPlayer, eTrait, pCity, eReligion = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnReligionSpread())

def onCivicChanged(argsList):
	pPlayer, eTrait, eCivic, iOldCivic = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnCivicChanged())

def onCircumnavigated(argsList):
	pPlayer, eTrait, iOrder = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnCircumnavigated())

def onOtherCityFounded(argsList):
	pPlayer, eTrait, pCity = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnOtherCityFounded())

def onPlayerEliminated(argsList):
	pPlayer, eTrait, iEliminatedPlayer = argsList
	eval(gc.getTraitInfo(eTrait).getPyOnPlayerEliminated())

def onImprovementChanged(argsList):
	pPlayer, eTrait, iX, iY, eOld, eNew = argsList
	pPlot = CyMap().plot(iX, iY)
	eval(gc.getTraitInfo(eTrait).getPyOnImprovementChanged())
