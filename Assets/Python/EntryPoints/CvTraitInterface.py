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
