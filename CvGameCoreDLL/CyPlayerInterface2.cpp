#include "CvGameCoreDLL.h"
#include "CyPlayer.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArea.h"
//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>
//# include <boost/python/scope.hpp>

//
// published python interface for CyPlayer
//

void CyPlayerPythonInterface2(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface2\n");

	// set the docstring of the current module scope 
	python::scope().attr("__doc__") = "Civilization IV Player Class"; 
	x
		.def("AI_updateFoundValues", &CyPlayer::AI_updateFoundValues, "void (bool bStartingLoc)")
		.def("AI_foundValue", &CyPlayer::AI_foundValue, "int (int, int, int, bool)")
		.def("AI_isFinancialTrouble", &CyPlayer::AI_isFinancialTrouble, "bool ()")
		.def("AI_demandRebukedWar", &CyPlayer::AI_demandRebukedWar, "bool (int /*PlayerTypes*/)")
		.def("AI_getAttitude", &CyPlayer::AI_getAttitude, "AttitudeTypes (int /*PlayerTypes*/) - Gets the attitude of the player towards the player passed in")
		.def("AI_unitValue", &CyPlayer::AI_unitValue, "int (int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea)")
		.def("AI_civicValue", &CyPlayer::AI_civicValue, "int (int /*CivicTypes*/ eCivic)")
		.def("AI_totalUnitAIs", &CyPlayer::AI_totalUnitAIs, "int (int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalAreaUnitAIs", &CyPlayer::AI_totalAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalWaterAreaUnitAIs", &CyPlayer::AI_totalWaterAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_getNumAIUnits", &CyPlayer::AI_getNumAIUnits, "int (UnitAIType) - Returns # of UnitAITypes the player current has of UnitAIType")
		.def("AI_getAttitudeExtra", &CyPlayer::AI_getAttitudeExtra, "int (int /*PlayerTypes*/ eIndex) - Returns the extra attitude for this player - usually scenario specific")
		.def("AI_setAttitudeExtra", &CyPlayer::AI_setAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iNewValue) - Sets the extra attitude for this player - usually scenario specific")
		.def("AI_changeAttitudeExtra", &CyPlayer::AI_changeAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iChange) - Changes the extra attitude for this player - usually scenario specific")
		.def("AI_getMemoryCount", &CyPlayer::AI_getMemoryCount, "int (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2)")
		.def("AI_changeMemoryCount", &CyPlayer::AI_changeMemoryCount, "void (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2, int iChange)")
		.def("AI_getExtraGoldTarget", &CyPlayer::AI_getExtraGoldTarget, "int ()")
		.def("AI_setExtraGoldTarget", &CyPlayer::AI_setExtraGoldTarget, "void (int)")

		.def("getScoreHistory", &CyPlayer::getScoreHistory, "int (int iTurn)")
		.def("getEconomyHistory", &CyPlayer::getEconomyHistory, "int (int iTurn)")
		.def("getIndustryHistory", &CyPlayer::getIndustryHistory, "int (int iTurn)")
		.def("getAgricultureHistory", &CyPlayer::getAgricultureHistory, "int (int iTurn)")
		.def("getPowerHistory", &CyPlayer::getPowerHistory, "int (int iTurn)")
		.def("getCultureHistory", &CyPlayer::getCultureHistory, "int (int iTurn)")
		.def("getEspionageHistory", &CyPlayer::getEspionageHistory, "int (int iTurn)")

		.def("getScriptData", &CyPlayer::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyPlayer::setScriptData, "void (str) - Set stored custom data (via pickle)")

		.def("chooseTech", &CyPlayer::chooseTech, "void (int iDiscover, wstring szText, bool bFront)")

		.def("AI_maxGoldTrade", &CyPlayer::AI_maxGoldTrade, "int (int)")
		.def("AI_maxGoldPerTurnTrade", &CyPlayer::AI_maxGoldPerTurnTrade, "int (int)")

		.def("splitEmpire", &CyPlayer::splitEmpire, "bool (int iAreaId)")
		.def("canSplitEmpire", &CyPlayer::canSplitEmpire, "bool ()")
		.def("canSplitArea", &CyPlayer::canSplitArea, "bool (int)")
		.def("canHaveTradeRoutesWith", &CyPlayer::canHaveTradeRoutesWith, "bool (int)")
		.def("forcePeace", &CyPlayer::forcePeace, "void (int)")

		// Added by this mod (here, as CyPlayerInterface1.cpp reached the debug information limit of MSVC 7.1: C1067)
		.def("getSummonDuration", &CyPlayer::getSummonDuration, "int () - extra turns of the summoned units from traits (added by this mod)")
		.def("getNewCityPopulation", &CyPlayer::getNewCityPopulation, "int () - extra population of new cities from traits and buildings (added by this mod)")
		.def("isNewCityStateReligion", &CyPlayer::isNewCityStateReligion, "bool () - the state religion spreads to new cities (added by this mod)")
		.def("isMaintainFeatures", &CyPlayer::isMaintainFeatures, "bool (int /*FeatureTypes*/ eFeature) - builds with an improvement keep the feature on our plots (added by this mod)")
		.def("getFeatureRouteType", &CyPlayer::getFeatureRouteType, "int (int /*FeatureTypes*/ eFeature) - the route a feature counts as in our territory (added by this mod)")
		.def("getGoldenAgeGreatPeopleRateModifier", &CyPlayer::getGoldenAgeGreatPeopleRateModifier, "int () - great people rate % in golden ages from traits (added by this mod)")
		.def("getCapitalBuildingProductionModifier", &CyPlayer::getCapitalBuildingProductionModifier, "int () - production % of the buildings the capital has from traits (added by this mod)")
		.def("getPillageGoldModifier", &CyPlayer::getPillageGoldModifier, "int () - pillage gold % from traits (added by this mod)")
		.def("getCityCaptureGoldModifier", &CyPlayer::getCityCaptureGoldModifier, "int () - city capture gold % from traits (added by this mod)")
		.def("getNewCityCultureLevel", &CyPlayer::getNewCityCultureLevel, "int () - the culture level new cities start with (added by this mod)")
		.def("getOccupationTimeModifier", &CyPlayer::getOccupationTimeModifier, "int () - revolt time % of conquered cities from traits and buildings (added by this mod)")
		.def("getUnitCostModifier", &CyPlayer::getUnitCostModifier, "int () - production cost % of units from traits (added by this mod)")
		.def("getBuildingCostModifier", &CyPlayer::getBuildingCostModifier, "int () - production cost % of buildings from traits (added by this mod)")
		.def("getNumWorldWonders", &CyPlayer::getNumWorldWonders, "int () - the world wonders in all the cities (added by this mod)")
		.def("getGoldInterest", &CyPlayer::getGoldInterest, "int () - gold of this turn from the treasury interest of the traits (added by this mod)")
		.def("getPeaceAngerPercentAnger", &CyPlayer::getPeaceAngerPercentAnger, "int () - anger from iPeaceAngerPerTurn of the traits, in 1/10 % of the population (added by this mod)")
		.def("getCultureRangeChange", &CyPlayer::getCultureRangeChange, "int () - iCultureRangeChange of the traits (added by this mod)")
		.def("isCivicReplaced", &CyPlayer::isCivicReplaced, "bool (int /*CivicTypes*/ eCivic) - the player can adopt a civic that replaces this one (added by this mod)")
		.def("isFavoriteCivicWithTraits", &CyPlayer::isFavoriteCivicWithTraits, "bool (int /*CivicTypes*/ eCivic) - this civic gives the player traits through bFavoriteCivic (added by this mod)")
		.def("canDoCivicByTraits", &CyPlayer::canDoCivicByTraits, "bool (int /*CivicTypes*/ eCivic) - PrereqOrTraits and ReplacedCivic, without the techs (added by this mod)")
		.def("getCivilizationUnit", &CyPlayer::getCivilizationUnit, "int (int /*UnitClassTypes*/ iUnitClass) - the unit of the class for this player, with UniqueUnits of the traits (added by this mod)")
		.def("getCivilizationBuilding", &CyPlayer::getCivilizationBuilding, "int (int /*BuildingClassTypes*/ iBuildingClass) - the building of the class for this player, with UniqueBuildings of the traits (added by this mod)")
		.def("isForbiddenCivic", &CyPlayer::isForbiddenCivic, "bool (int /*CivicTypes*/ eCivic) - ForbiddenCivics of the traits (added by this mod)")
		.def("isNoPillageInBorders", &CyPlayer::isNoPillageInBorders, "bool () - other teams cannot pillage in our territory (traits; added by this mod)")
		.def("getMinCityRange", &CyPlayer::getMinCityRange, "int () - MIN_CITY_RANGE with iMinCityRangeChange of the traits (added by this mod)")
		.def("isFoundInForeignTerritory", &CyPlayer::isFoundInForeignTerritory, "bool () - bFoundInForeignTerritory of the traits (added by this mod)")
		.def("isNoGlobalWarmingInBorders", &CyPlayer::isNoGlobalWarmingInBorders, "bool () - bNoGlobalWarmingInBorders of the traits (added by this mod)")
		.def("getMaxCities", &CyPlayer::getMaxCities, "int () - the most cities the player can found, -1 without a limit (traits; added by this mod)")
		.def("isMaxCitiesReached", &CyPlayer::isMaxCitiesReached, "bool () - the player cannot found more cities (added by this mod)")
		.def("getBuildingClassExtraCommerce", &CyPlayer::getBuildingClassExtraCommerce, "int (int /*BuildingClassTypes*/ eBuildingClass, int /*CommerceTypes*/ eCommerce)")
		.def("getConditionalTraitCount", &CyPlayer::getConditionalTraitCount, "int (int /*TraitTypes*/ eTrait) - the part of getFreeTraitCount given by ConditionalFreeTraits (added by this mod)")
		.def("updateConditionalTraits", &CyPlayer::updateConditionalTraits, "void () - recalculates ConditionalFreeTraits; game state, call it on every computer (added by this mod)")
		.def("getNumUnitsKilled", &CyPlayer::getNumUnitsKilled, "int () - enemy units killed in the whole game (added by this mod)")
		.def("getNumCitiesConquered", &CyPlayer::getNumCitiesConquered, "int () - cities conquered in the whole game (added by this mod)")
		.def("hasBaseTrait", &CyPlayer::hasBaseTrait, "bool (int /*TraitTypes*/ eTrait) - the trait of the leader or the civilization (added by this mod)")
		.def("getFreeTraitCount", &CyPlayer::getFreeTraitCount, "int (int /*TraitTypes*/ eTrait) - how many times the trait is given during the game (added by this mod)")
		.def("changeFreeTraitCount", &CyPlayer::changeFreeTraitCount, "void (int /*TraitTypes*/ eTrait, int iChange) - gives or takes away the trait during the game; game state, call it on every computer (added by this mod)")
		.def("isIgnoreObsoleteBuilding", &CyPlayer::isIgnoreObsoleteBuilding, "bool (int /*BuildingTypes*/ eBuilding) - a trait keeps the building from becoming obsolete (added by this mod)")
		.def("isObsoleteBuilding", &CyPlayer::isObsoleteBuilding, "bool (int /*BuildingTypes*/ eBuilding) - obsolete for the team and not kept by a trait (added by this mod)")
		.def("canBuildByTraits", &CyPlayer::canBuildByTraits, "bool (int /*BuildTypes*/ eBuild) - PrereqOrTraits and ReplacedBuild of the builds (added by this mod)")
		;
}
