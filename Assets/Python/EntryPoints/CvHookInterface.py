## Sid Meier's Civilization 4
## CvHookInterface.py (added by this mod)
##
## The DLL calls the functions below for the Python hooks of buildings, promotions, techs, civics and units, and for
## PyCondition of ConditionalFreeTraits (traits). A hook is a Python expression written in the XML and evaluated here.
## The common functions of CvHookUtils.py (gainGold, createUnits, addPromotion, ...) can be used in it; add new ones there.
## (The hooks of the traits are in CvTraitInterface.py, the spells in CvSpellInterface.py.)
##
## Hook (where)                      Called when                                            Names it can use
## PyOnBuilt (BuildingInfo)          a city completes the building by production            pCity, pPlayer, eBuilding
##                                   (not a free building, an event or Python)
## PyOnBeginTurn (BuildingInfo)      each turn, for each city with the building (not        pCity, pPlayer, eBuilding
##                                   obsolete), before its growth and production
## PyOnCombatWin (PromotionInfo)     the unit with the promotion won a normal combat        pUnit, pPlayer, pOpponent, ePromotion
## PyOnCombatLoss (PromotionInfo)    the unit with the promotion lost a normal combat       pUnit, pPlayer, pOpponent, ePromotion
##                                   (it dies right after)
## PyOnBeginTurn (PromotionInfo)     each turn, for each unit with the promotion, after     pUnit, pPlayer, ePromotion
##                                   it heals
## PyOnDiscovered (TechInfo)         the team gets the tech (research, trade, hut, ...):    pPlayer, eTech, bFirst
##                                   once for each player of the team. bFirst: 1 when the
##                                   team is the first to have it, else 0 (0 by trade)
## PyOnAdopt (CivicInfo)             the player adopts the civic                            pPlayer, eCivic
## PyOnRevoke (CivicInfo)            the player stops the civic                             pPlayer, eCivic
## PyOnCreated (UnitInfo)            a unit of the type is created (trained, from Python,   pUnit, pPlayer
##                                   great people, ...)
## PyOnDeath (UnitInfo)              a unit of the type dies (killed, disbanded, ...)       pUnit, pPlayer, iKiller
##                                   iKiller: the player who killed it, -1 if none
## PyCondition (ConditionalFreeTrait) the other conditions of the item hold: True gives    pPlayer, eTrait (the trait given)
##                                   the trait. PyConditionText: the text key of the help
##
## pPlayer is the owner of the city or the unit. A combat is the normal attack of a unit (not air strikes or bombing),
## and a withdrawal is neither a win nor a loss. None of the hooks runs in the world builder, for the techs and civics
## a game starts with, or for an upgrade, a gift or the ConvertUnit of a spell (the unit goes on as another unit).
## Promotions: the promotions the unit had in the combat count, also one the combat removes.
## PyCondition is evaluated when the conditions of ConditionalFreeTraits are (war and peace, civics, cities, ...) and at the
## start of each turn of the player, so a condition on gold or population follows the next turn.
##
## Samples in the test data of this mod:
##   <PyOnBuilt>gainGold(pPlayer, 30)</PyOnBuilt>                                    (BUILDING_TEST_GROWTH)
##   <PyOnBeginTurn>isEveryTurns(5) and addCulture(pCity, 10)</PyOnBeginTurn>         (BUILDING_TEST)
##   <PyOnCombatWin>gainGold(pPlayer, 10)</PyOnCombatWin>                             (PROMOTION_TEST_HIDDEN)
##   <PyOnCombatLoss>gainGold(pPlayer, 5)</PyOnCombatLoss>                            (PROMOTION_TEST_HIDDEN)
##   <PyOnBeginTurn>isOnFeature(pUnit, 'FEATURE_FOREST') and healUnit(pUnit, 10)</PyOnBeginTurn>  (PROMOTION_TEST_HIDDEN)
##   <PyOnDiscovered>gainGold(pPlayer, 20 + 30 * bFirst)</PyOnDiscovered>             (TECH_TEST)
##   <PyOnAdopt>createUnits(pPlayer, 'UNITCLASS_WORKER', 1, capitalPlot(pPlayer))</PyOnAdopt>      (CIVIC_TEST)
##   <PyOnRevoke>gainGold(pPlayer, 20)</PyOnRevoke>                                   (CIVIC_TEST)
##   <PyOnCreated>addPromotion(pUnit, 'PROMOTION_COMBAT1')</PyOnCreated>               (UNIT_TEST_B)
##   <PyOnDeath>createUnits(pPlayer, 'UNITCLASS_SCOUT', 1, capitalPlot(pPlayer))</PyOnDeath>       (UNIT_TEST_A)
##   <PyCondition>pPlayer.getGold() &gt;= 200</PyCondition>                           (TRAIT_TEST)
## More samples:
##   <PyOnBuilt>createUnits(pPlayer, 'UNITCLASS_WORKER', 1, pCity.plot())</PyOnBuilt>  (a worker in the city)
##   <PyOnCombatWin>CyGame().getSorenRandNum(100, 'Hook') &lt; 25 and addPromotion(pUnit, 'PROMOTION_COMBAT1')</PyOnCombatWin>
##                                                                                      (25% chance)
##   <PyOnDiscovered>bFirst and createUnits(pPlayer, 'UNITCLASS_SETTLER', 1, capitalPlot(pPlayer))</PyOnDiscovered>
##   <PyOnAdopt>pPlayer.changeGoldenAgeTurns(scaleBySpeed(2, 'GoldenAge'))</PyOnAdopt> (a short golden age)
##   <PyOnDeath>iKiller != -1 and gainGold(gc.getPlayer(iKiller), 10)</PyOnDeath>       (a bounty for the killer)
##   <PyCondition>pPlayer.getNumCities() &gt;= 2 and pPlayer.getTotalPopulation() &gt;= 10</PyCondition>
## Several effects: put them in a tuple, which evaluates all of them, e.g.
##   <PyOnAdopt>(gainGold(pPlayer, 20), createUnits(pPlayer, 'UNITCLASS_WORKER', 1, capitalPlot(pPlayer)))</PyOnAdopt>
## ("and" stops at the first function that returns None, as most do), or write a function in CvHookUtils.py.
## "condition and effect" runs the effect only when the condition is true, as in the samples.
##
## Rules:
## - The hooks (not PyCondition) change the game and run on every computer of a multiplayer game: use
##   CyGame().getSorenRandNum for random numbers, never the random module or CyGame().getASyncRand.
## - PyCondition must not change the game: it runs many times and only reads the state.
## - A combat hook must not kill or heal pUnit or pOpponent. To kill a unit in a hook, use the delayed kill:
##   pUnit.kill(True, -1). A hook of a building must not destroy its city.
## - In the XML, write < > & as &lt; &gt; &amp;
## - Describe the effects in the Help of the item: the help text does not show the hooks.

from CvPythonExtensions import *
from CvHookUtils import *

gc = CyGlobalContext()

## Buildings

def onBuildingBuilt(argsList):
	pCity, eBuilding = argsList
	pPlayer = gc.getPlayer(pCity.getOwner())
	eval(gc.getBuildingInfo(eBuilding).getPyOnBuilt())

def onBuildingBeginTurn(argsList):
	pCity, eBuilding = argsList
	pPlayer = gc.getPlayer(pCity.getOwner())
	eval(gc.getBuildingInfo(eBuilding).getPyOnBeginTurn())

## Promotions

def onPromotionCombatWin(argsList):
	pUnit, pOpponent, ePromotion = argsList
	pPlayer = gc.getPlayer(pUnit.getOwner())
	eval(gc.getPromotionInfo(ePromotion).getPyOnCombatWin())

def onPromotionCombatLoss(argsList):
	pUnit, pOpponent, ePromotion = argsList
	pPlayer = gc.getPlayer(pUnit.getOwner())
	eval(gc.getPromotionInfo(ePromotion).getPyOnCombatLoss())

def onPromotionBeginTurn(argsList):
	pUnit, ePromotion = argsList
	pPlayer = gc.getPlayer(pUnit.getOwner())
	eval(gc.getPromotionInfo(ePromotion).getPyOnBeginTurn())

## Techs

def onTechDiscovered(argsList):
	pPlayer, eTech, bFirst = argsList
	eval(gc.getTechInfo(eTech).getPyOnDiscovered())

## Civics

def onCivicAdopt(argsList):
	pPlayer, eCivic = argsList
	eval(gc.getCivicInfo(eCivic).getPyOnAdopt())

def onCivicRevoke(argsList):
	pPlayer, eCivic = argsList
	eval(gc.getCivicInfo(eCivic).getPyOnRevoke())

## Units

def onUnitCreated(argsList):
	pUnit = argsList[0]
	pPlayer = gc.getPlayer(pUnit.getOwner())
	eval(gc.getUnitInfo(pUnit.getUnitType()).getPyOnCreated())

def onUnitDeath(argsList):
	pUnit, iKiller = argsList
	pPlayer = gc.getPlayer(pUnit.getOwner())
	eval(gc.getUnitInfo(pUnit.getUnitType()).getPyOnDeath())

## ConditionalFreeTraits

def isConditionTrue(argsList):
	pPlayer, eTrait, szCondition = argsList
	if eval(szCondition):
		return 1
	return 0
