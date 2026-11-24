//------------------------------------------------------------------------------------------------
//
//  FILE:    CvTraitInfo.cpp
//
//  PURPOSE: CvTraitInfo (the traits of CIV4TraitInfos.xml), moved out of CvInfos.cpp by this mod because of
//           its size. The XML helpers shared with the other infos are declared in CvInfosReadUtils.h
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvInfos.h"
#include "CvGlobals.h"
#include "CvArtFileMgr.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"
#include "CvInfosReadUtils.h"

//======================================================================================================
//					CvTraitInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTraitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::CvTraitInfo() :
m_iHealth(0),													
m_iHappiness(0),													
m_iMaxAnarchy(-1),											
m_iUpkeepModifier(0),									
m_iLevelExperienceModifier(0),									
m_iGreatPeopleRateModifier(0),						
m_iGreatGeneralRateModifier(0),						
m_iDomesticGreatGeneralRateModifier(0),						
m_iMaxGlobalBuildingProductionModifier(0),	
m_iMaxTeamBuildingProductionModifier(0),		
m_iMaxPlayerBuildingProductionModifier(0),
m_iWorkerSpeedModifier(0),
m_iFreeExperience(0),
m_iMilitaryProductionModifier(0),
m_iImprovementUpgradeRateModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iWarWearinessModifier(0),
m_iFreeSpecialist(0),
m_iTradeRoutes(0),
m_iExpInBorderModifier(0),
m_iCityRadiusChange(0),
m_iCityRadiusCultureLevel(NO_CULTURELEVEL),
m_iMaxCities(0),
m_iMaxCitiesPercent(0),
m_iSummonDuration(0),
m_iGoldenAgeModifier(0),
m_iGoldenAgeGreatPeopleRateModifier(0),
m_iCapitalBuildingProductionModifier(0),
m_iPillageGoldModifier(0),
m_iCityCaptureGoldModifier(0),
m_iUnitCostModifier(0),
m_iBuildingCostModifier(0),
m_bNoPillageInBorders(false),
m_iMinCityRangeChange(0),
m_bFoundInForeignTerritory(false),
m_bNoGlobalWarmingInBorders(false),
m_iReligionSpreadChange(0),
m_iCorporationSpreadChange(0),
m_iGoldenAgeLandMovesChange(0),
m_iGoldenAgeSeaMovesChange(0),
m_bInlandCanals(false),
m_bBarbarianAlly(false),
m_bBarbarianVision(false),
m_iStartBiasCoastal(0),
m_iStartBiasRiver(0),
m_piStartBiasTerrains(NULL),
m_piStartBiasFeatures(NULL),
m_piStartBiasBonuses(NULL),
m_bSeaStart(false),
m_iSeaStartUnitClass(NO_UNITCLASS),
m_bFoundingVassal(false),
m_iFoundingVassalCivilization(NO_CIVILIZATION),
m_iFoundingVassalLeader(NO_LEADER),
m_iFoundingVassalHandicap(NO_HANDICAP),
m_iFoundingVassalRange(0),
m_iGoldInterestPercent(0),
m_iGoldInterestMax(0),
m_iPeaceAngerPerTurn(0),
m_iPeaceAngerDecayPerTurn(0),
m_iPeaceAngerMax(0),
m_iCultureRangeChange(0),
m_iNewCityCultureLevel(NO_CULTURELEVEL),
m_iNewCityPopulation(0),
m_iOccupationTimeModifier(0),
m_iConquestCulturePercent(0),
m_iConquestBuildingSurviveModifier(0),
m_bNoConquestPopulationLoss(false),
m_bNoAutoRaze(false),
m_bConquestLearnUniqueUnits(false),
m_bConquestLearnUniqueBuildings(false),
m_iStateReligionHappiness(0),
m_iStateReligionHealth(0),
m_iStateReligionGreatPeopleRateModifier(0),
m_iStateReligionUnitProductionModifier(0),
m_iStateReligionBuildingProductionModifier(0),
m_iStateReligionFreeExperience(0),
m_iEnemyMoveCostInBorders(0),
m_iRevengeCombatPercent(0),
m_iKnownTechCostModifier(0),
m_iKnownTechCostLimit(0),
m_bConquestHeal(false),
m_bStackCityDefense(false),
m_bRevealWonderProduction(false),
m_bRevealWarPlans(false),
m_bRevealHiddenAttitude(false),
m_bLongCountCalendar(false),
m_bImprovementUpgradeUnworked(false),
m_bUnsettleGreatPeople(false),
m_bIgnoreEvents(false),
m_iEventChanceChange(0),
m_bNoFirstEventDelay(false),
m_bBuyCities(false),
m_iDealCityValueModifier(0),
m_iDealGoldLimitModifier(0),
m_iDealGoldValueModifier(0),
m_bNoCultureFlip(false),
m_bNoRevoltAndNoCultureFlip(false),
m_bInstantCultureFlip(false),
m_iCultureRevoltChance(0),
m_iNewWorldCityPopulation(0),
m_iOverseasCityPopulation(0),
m_iOverseasMaintenanceModifier(0),
m_iDistantTradeRouteModifier(0),
m_iEnemyAttritionInBorders(0),
m_iEnemyAttritionLimit(100),
m_iAnarchyGoldenAgePercent(0),
m_iTechCostModifier(0),
m_iAttitudeChange(0),
m_iRouteMovementModifierInBorders(0),
m_iCombatPercent(0),
m_iHappinessPerReligion(0),
m_paiCommercePerReligion(NULL),
m_iDefenseCombatPercent(0),
m_paiWorldWonderCommerceChange(NULL),
m_paiGlobalWorldWonderCommerceChange(NULL),
m_iEnemyWarWearinessModifier(0),
m_bConquestStateReligion(false),
m_pabConquestReligionBuilding(NULL),
m_pabNoTechCivic(NULL),
m_iMaxNationalWondersPerCity(0),
m_iMaxNationalWondersPerCityForOCC(0),
m_bHumanCanBeVassal(false),
m_bBasicTrait(false),
m_bGainTraitOnCapitalCapture(false),
m_bGainTraitsOnElimination(false),
m_bRandomBasicTraits(false),
m_iRandomBasicTraitTurns(0),
m_iRandomBasicTraitCount(0),
m_bGainTraitsOnFriendly(false),
m_bGainTraitsOnFriendlyAll(false),
m_iMaxExperienceAfterUpgrade(0),
m_iExpOutsideBordersChange(0),
m_iWeLoveTheKingChance(0),
m_paiHappyYieldPercent(NULL),
m_paiHappyCommercePercent(NULL),
m_paiHealthyYieldPercent(NULL),
m_paiHealthyCommercePercent(NULL),
m_pabRevealBonus(NULL),
m_pabNoTechProcess(NULL),
m_bNoConscriptExperiencePenalty(false),
m_pabFreeTech(NULL),
m_paiYieldChange(NULL),
m_iHealthPerReligion(0),
m_paiYieldPerReligion(NULL),
m_paiWorldWonderYieldChange(NULL),
m_paiGlobalWorldWonderYieldChange(NULL),
m_paiSpecialistExtraYield(NULL),
m_iNonStateReligionHappiness(0),
m_pabTerrainTrade(NULL),
m_paiCommerceHappiness(NULL),
m_paiCommerceHealth(NULL),
m_iBaseFreeUnits(0),
m_iBaseFreeMilitaryUnits(0),
m_iFreeUnitsPopulationPercent(0),
m_iFreeMilitaryUnitsPopulationPercent(0),
m_iGoldPerUnit(0),
m_iGoldPerMilitaryUnit(0),
m_iCorporationMaintenanceModifier(0),
m_iMaintenanceModifier(0),
m_iMaxConscript(0),
m_iConscriptPopulationModifier(0),
m_iConscriptMinCityPopulationChange(0),
m_iConscriptMinCulturePercentChange(0),
m_iConscriptPopAngerChange(0),
m_iConscriptAngerLengthPercent(0),
m_bMilitaryFoodProduction(false),
m_iHealRateChange(0),
m_iLargestCityHappiness(0),
m_iHappyPerMilitaryUnit(0),
m_iFoodKept(0),
m_bNoUnhealthyPopulation(false),
m_bBuildingOnlyHealthy(false),
m_iCoastalTradeRoutes(0),
m_iTradeRouteModifier(0),
m_iForeignTradeRouteModifier(0),
m_iAllCityDefense(0),
m_iBombardDefense(0),
m_iEspionageDefense(0),
m_bNewCityStateReligion(false),
m_paiExtraYieldThreshold(NULL),
m_paiYieldModifier(NULL),
m_paiCapitalYieldModifier(NULL),
m_paiCapitalCommerceModifier(NULL),
m_paiSpecialistExtraCommerce(NULL),
m_paiUnitCombatFreeExperience(NULL),
m_paiUnitCombatProductionModifier(NULL),
m_paiUnitCombatUpgradeDiscount(NULL),
m_pabUpgradeAnywhereUnitCombat(NULL),
m_pabUpgradeAnywhereExcludeEnemy(NULL),
m_paiRiverPlotYieldChange(NULL),
m_paiRiverEdgeYieldPercent(NULL),
m_paiLakePlotYieldChange(NULL),
m_paiLakePlotYieldChangePerEra(NULL),
m_paiAngryYieldChange(NULL),
m_paiAngryCommerceChange(NULL),
m_paiUnhealthyYieldChange(NULL),
m_paiUnhealthyCommerceChange(NULL),
m_paiTradeYieldModifier(NULL),
m_paiCommerceChange(NULL),
m_paiCommerceModifier(NULL),
m_pabFreePromotionUnitCombat(NULL),
m_pabFreePromotion(NULL),
m_pabMaintainFeatures(NULL),
m_paiFeatureRoute(NULL),
m_bIgnoreBuildingObsolescence(false),
m_pabNoObsoleteBuildingClass(NULL),
m_pabNoObsoleteSpecialBuilding(NULL),
m_bIgnoreBonusObsolescence(false),
m_pabNoObsoleteBonus(NULL),
m_pabForbiddenCivic(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTraitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::~CvTraitInfo()
{
	SAFE_DELETE_ARRAY(m_paiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_paiTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_paiCommerceChange);
	SAFE_DELETE_ARRAY(m_paiCommerceModifier);
	SAFE_DELETE_ARRAY(m_paiYieldModifier);
	SAFE_DELETE_ARRAY(m_paiCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_paiCapitalCommerceModifier);
	SAFE_DELETE_ARRAY(m_paiSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatUpgradeDiscount);
	SAFE_DELETE_ARRAY(m_pabUpgradeAnywhereUnitCombat);
	SAFE_DELETE_ARRAY(m_pabUpgradeAnywhereExcludeEnemy);
	SAFE_DELETE_ARRAY(m_paiRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_paiRiverEdgeYieldPercent);
	SAFE_DELETE_ARRAY(m_paiLakePlotYieldChange);
	SAFE_DELETE_ARRAY(m_paiLakePlotYieldChangePerEra);
	SAFE_DELETE_ARRAY(m_paiAngryYieldChange);
	SAFE_DELETE_ARRAY(m_paiAngryCommerceChange);
	SAFE_DELETE_ARRAY(m_paiUnhealthyYieldChange);
	SAFE_DELETE_ARRAY(m_paiUnhealthyCommerceChange);
	SAFE_DELETE_ARRAY(m_pabFreePromotionUnitCombat);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
	SAFE_DELETE_ARRAY(m_pabMaintainFeatures);
	SAFE_DELETE_ARRAY(m_paiFeatureRoute);
	SAFE_DELETE_ARRAY(m_pabNoObsoleteBuildingClass);
	SAFE_DELETE_ARRAY(m_pabNoObsoleteSpecialBuilding);
	SAFE_DELETE_ARRAY(m_pabNoObsoleteBonus);
	SAFE_DELETE_ARRAY(m_piStartBiasTerrains);
	SAFE_DELETE_ARRAY(m_piStartBiasFeatures);
	SAFE_DELETE_ARRAY(m_piStartBiasBonuses);
	SAFE_DELETE_ARRAY(m_pabForbiddenCivic);
	SAFE_DELETE_ARRAY(m_pabNoTechCivic);
	SAFE_DELETE_ARRAY(m_pabConquestReligionBuilding);
	SAFE_DELETE_ARRAY(m_paiHappyYieldPercent);
	SAFE_DELETE_ARRAY(m_paiHappyCommercePercent);
	SAFE_DELETE_ARRAY(m_paiHealthyYieldPercent);
	SAFE_DELETE_ARRAY(m_paiHealthyCommercePercent);
	SAFE_DELETE_ARRAY(m_pabRevealBonus);
	SAFE_DELETE_ARRAY(m_pabNoTechProcess);
	SAFE_DELETE_ARRAY(m_pabFreeTech);
	SAFE_DELETE_ARRAY(m_paiYieldChange);
	SAFE_DELETE_ARRAY(m_paiCommerceHappiness);
	SAFE_DELETE_ARRAY(m_pabTerrainTrade);
	SAFE_DELETE_ARRAY(m_paiCommerceHealth);
	SAFE_DELETE_ARRAY(m_paiYieldPerReligion);
	SAFE_DELETE_ARRAY(m_paiWorldWonderYieldChange);
	SAFE_DELETE_ARRAY(m_paiGlobalWorldWonderYieldChange);
	SAFE_DELETE_ARRAY(m_paiSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_paiCommercePerReligion);
	SAFE_DELETE_ARRAY(m_paiWorldWonderCommerceChange);
	SAFE_DELETE_ARRAY(m_paiGlobalWorldWonderCommerceChange);
}

int CvTraitInfo::getHealth() const									
{
	return m_iHealth; 
}

int CvTraitInfo::getHappiness() const									
{
	return m_iHappiness; 
}

int CvTraitInfo::getMaxAnarchy() const							
{
	return m_iMaxAnarchy; 
}

int CvTraitInfo::getUpkeepModifier() const					
{
	return m_iUpkeepModifier; 
}

int CvTraitInfo::getLevelExperienceModifier() const					
{
	return m_iLevelExperienceModifier; 
}

int CvTraitInfo::getGreatPeopleRateModifier() const	
{
	return m_iGreatPeopleRateModifier; 
}

int CvTraitInfo::getGreatGeneralRateModifier() const	
{
	return m_iGreatGeneralRateModifier; 
}

int CvTraitInfo::getDomesticGreatGeneralRateModifier() const	
{
	return m_iDomesticGreatGeneralRateModifier; 
}

int CvTraitInfo::getMaxGlobalBuildingProductionModifier() const	
{
	return m_iMaxGlobalBuildingProductionModifier; 
}

int CvTraitInfo::getMaxTeamBuildingProductionModifier() const		
{
	return m_iMaxTeamBuildingProductionModifier; 
}

int CvTraitInfo::getMaxPlayerBuildingProductionModifier() const	
{
	return m_iMaxPlayerBuildingProductionModifier; 
}

int CvTraitInfo::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

int CvTraitInfo::getFreeExperience() const
{
	return m_iFreeExperience;
}

int CvTraitInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

int CvTraitInfo::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}

int CvTraitInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvTraitInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}

int CvTraitInfo::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}

int CvTraitInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

int CvTraitInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvTraitInfo::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}

int CvTraitInfo::getCityRadiusChange() const
{
	return m_iCityRadiusChange;
}

// The culture level a city needs for iCityRadiusChange (added by this mod; NO_CULTURELEVEL: any city)
int CvTraitInfo::getCityRadiusCultureLevel() const
{
	return m_iCityRadiusCultureLevel;
}

// The most cities the player can found (added by this mod). Both 0: no limit
int CvTraitInfo::getMaxCities() const
{
	return m_iMaxCities;
}

// Percent of iTargetNumCities of the map size (CIV4WorldInfo.xml), added to iMaxCities
int CvTraitInfo::getMaxCitiesPercent() const
{
	return m_iMaxCitiesPercent;
}

// The city limit of the trait on the map size: iMaxCities + iMaxCitiesPercent of iTargetNumCities, at least 1.
// -1: the trait has no limit. Without a map size (NO_WORLDSIZE), only iMaxCities.
int CvTraitInfo::getMaxCitiesLimit(WorldSizeTypes eWorldSize) const
{
	int iLimit;

	if (m_iMaxCities == 0 && m_iMaxCitiesPercent == 0)
	{
		return -1;
	}

	iLimit = m_iMaxCities;
	if (m_iMaxCitiesPercent != 0 && eWorldSize != NO_WORLDSIZE)
	{
		iLimit += (m_iMaxCitiesPercent * GC.getWorldInfo(eWorldSize).getTargetNumCities()) / 100;
	}

	return std::max(1, iLimit);
}

int CvTraitInfo::getSummonDuration() const
{
	return m_iSummonDuration;
}

// Golden age length % (the building tag of the same name; applied once in CvPlayer::init)
int CvTraitInfo::getGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}

// Great people rate % during golden ages
int CvTraitInfo::getGoldenAgeGreatPeopleRateModifier() const
{
	return m_iGoldenAgeGreatPeopleRateModifier;
}

// Yields of the plots with the bonus in the territory of the player (BonusYieldChanges)
int CvTraitInfo::getBonusYieldChange(int iBonus, int iYield) const
{
	FAssertMsg(iBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(iBonus > -1, "Index out of bounds");
	FAssertMsg(iYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(iYield > -1, "Index out of bounds");
	if ((iBonus < 0) || (iBonus >= (int)m_aaiBonusYieldChange.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiBonusYieldChange[iBonus][iYield];
}

// Production % of the buildings the capital has, in the other cities
int CvTraitInfo::getCapitalBuildingProductionModifier() const
{
	return m_iCapitalBuildingProductionModifier;
}

// Gold % from pillaging improvements
int CvTraitInfo::getPillageGoldModifier() const
{
	return m_iPillageGoldModifier;
}

// Conquest/iGoldModifier: gold % from conquering cities
int CvTraitInfo::getCityCaptureGoldModifier() const
{
	return m_iCityCaptureGoldModifier;
}

// Production cost % of all units (added by this mod, as in ex's CivGoldEmpires)
int CvTraitInfo::getUnitCostModifier() const
{
	return m_iUnitCostModifier;
}

// Production cost % of all buildings (added by this mod, as in ex's CivGoldEmpires)
int CvTraitInfo::getBuildingCostModifier() const
{
	return m_iBuildingCostModifier;
}

// The units of other teams cannot pillage in the territory of the player (added by this mod, as in ex's CivGoldEmpires)
// Added to MIN_CITY_RANGE for the cities of the player (added by this mod)
int CvTraitInfo::getMinCityRangeChange() const
{
	return m_iMinCityRangeChange;
}

// The player can found cities in the territory of other players (added by this mod)
bool CvTraitInfo::isFoundInForeignTerritory() const
{
	return m_bFoundInForeignTerritory;
}

// Global warming does nothing on the plots of the player (added by this mod)
bool CvTraitInfo::isNoGlobalWarmingInBorders() const
{
	return m_bNoGlobalWarmingInBorders;
}

// Added to the chance (%) of the missionaries and the executives of the player (added by this mod)
int CvTraitInfo::getReligionSpreadChange() const
{
	return m_iReligionSpreadChange;
}

int CvTraitInfo::getCorporationSpreadChange() const
{
	return m_iCorporationSpreadChange;
}

// Added to the moves of the land and sea units of the player during a golden age (added by this mod)
int CvTraitInfo::getGoldenAgeLandMovesChange() const
{
	return m_iGoldenAgeLandMovesChange;
}

int CvTraitInfo::getGoldenAgeSeaMovesChange() const
{
	return m_iGoldenAgeSeaMovesChange;
}

// The ships of the player enter forts and cities that do not touch water (added by this mod)
bool CvTraitInfo::isInlandCanals() const
{
	return m_bInlandCanals;
}

// StartBias of the trait (added by this mod), as the civilizations: iCoastal, iRiver, Terrains, Features, BonusBiases
int CvTraitInfo::getStartBiasCoastal() const
{
	return m_iStartBiasCoastal;
}

int CvTraitInfo::getStartBiasRiver() const
{
	return m_iStartBiasRiver;
}

int CvTraitInfo::getStartBiasTerrain(int i) const
{
	return (m_piStartBiasTerrains != NULL && i > -1 && i < GC.getNumTerrainInfos()) ? m_piStartBiasTerrains[i] : 0;
}

int CvTraitInfo::getStartBiasFeature(int i) const
{
	return (m_piStartBiasFeatures != NULL && i > -1 && i < GC.getNumFeatureInfos()) ? m_piStartBiasFeatures[i] : 0;
}

int CvTraitInfo::getStartBiasBonus(int i) const
{
	return (m_piStartBiasBonuses != NULL && i > -1 && i < GC.getNumBonusInfos()) ? m_piStartBiasBonuses[i] : 0;
}

// StartBias/SeaStart (added by this mod): the player starts at sea (CvPlayer::moveStartingPlotToSea) with its starting units on ships
// of the class ShipUnitClass (CvPlayer::loadSeaStartUnits)
bool CvTraitInfo::isSeaStart() const
{
	return m_bSeaStart;
}

// StartBias/SeaStart/ShipUnitClass: NO_UNITCLASS without SeaStart or for NONE (no ships)
int CvTraitInfo::getSeaStartUnitClass() const
{
	return m_iSeaStartUnitClass;
}

// CityFoundedUnits (added by this mod): units the player gets in each city it founds
int CvTraitInfo::getNumCityFoundedUnits() const
{
	return (int)m_aiCityFoundedUnitClasses.size();
}

int CvTraitInfo::getCityFoundedUnitClass(int i) const
{
	return (i > -1 && i < getNumCityFoundedUnits()) ? m_aiCityFoundedUnitClasses[i] : NO_UNITCLASS;
}

int CvTraitInfo::getCityFoundedUnitNum(int i) const
{
	return (i > -1 && i < getNumCityFoundedUnits()) ? m_aiCityFoundedUnitNums[i] : 0;
}

bool CvTraitInfo::isCityFoundedUnitCoastalOnly(int i) const
{
	return (i > -1 && i < getNumCityFoundedUnits()) ? m_abCityFoundedUnitCoastalOnly[i] : false;
}

// FoundingVassal (added by this mod): when the player founds its first city, an AI civilization joins the game near it,
// with settlers and defenders, and joins the team of the player (CvPlayer::doFoundingVassal)
bool CvTraitInfo::isFoundingVassal() const
{
	return m_bFoundingVassal;
}

int CvTraitInfo::getFoundingVassalCivilization() const
{
	return m_iFoundingVassalCivilization;
}

int CvTraitInfo::getFoundingVassalLeader() const
{
	return m_iFoundingVassalLeader;
}

int CvTraitInfo::getFoundingVassalHandicap() const
{
	return m_iFoundingVassalHandicap;
}

// FoundingVassal/iRange: the AI starts within this distance of the first city
int CvTraitInfo::getFoundingVassalRange() const
{
	return m_iFoundingVassalRange;
}

// The team of the player is at peace with the barbarians (added by this mod, as FFH2)
bool CvTraitInfo::isBarbarianAlly() const
{
	return m_bBarbarianAlly;
}

bool CvTraitInfo::isBarbarianVision() const
{
	return m_bBarbarianVision;
}

// PassableTerrainsInBorders / PassableFeaturesInBorders (added by this mod): the units of the player enter these impassable
// plots in its territory without damage. TERRAIN_PEAK stands for peak plots
bool CvTraitInfo::isPassableTerrainInBorders(int iTerrain) const
{
	return (std::find(m_aiPassableTerrainsInBorders.begin(), m_aiPassableTerrainsInBorders.end(), iTerrain) != m_aiPassableTerrainsInBorders.end());
}

bool CvTraitInfo::isPassableFeatureInBorders(int iFeature) const
{
	return (std::find(m_aiPassableFeaturesInBorders.begin(), m_aiPassableFeaturesInBorders.end(), iFeature) != m_aiPassableFeaturesInBorders.end());
}

// FeatureHealChanges (added by this mod): heal % of the units of the player on the feature
int CvTraitInfo::getFeatureHealChange(int iFeature) const
{
	return ((iFeature > -1) && (iFeature < (int)m_aiFeatureHealChange.size())) ? m_aiFeatureHealChange[iFeature] : 0;
}

// LakePlotYieldChangesPerEra (added by this mod): times the current era of the player
int CvTraitInfo::getLakePlotYieldChangePerEra(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiLakePlotYieldChangePerEra ? m_paiLakePlotYieldChangePerEra[i] : 0;
}

int* CvTraitInfo::getLakePlotYieldChangePerEraArray() const
{
	return m_paiLakePlotYieldChangePerEra;
}

bool CvTraitInfo::isNoPillageInBorders() const
{
	return m_bNoPillageInBorders;
}

// Gold every turn: this % of the treasury, at most GoldInterest/iMax (0: no limit; added by this mod, as in ex's
// CivGoldEmpires)
int CvTraitInfo::getGoldInterestPercent() const
{
	return m_iGoldInterestPercent;
}

int CvTraitInfo::getGoldInterestMax() const
{
	return m_iGoldInterestMax;
}

// Anger from peace, in 1/10 % of the population like iCivicPercentAnger (added by this mod): see CvPlayer::doPeaceAnger()
int CvTraitInfo::getPeaceAngerPerTurn() const
{
	return m_iPeaceAngerPerTurn;
}

int CvTraitInfo::getPeaceAngerDecayPerTurn() const
{
	return m_iPeaceAngerDecayPerTurn;
}

int CvTraitInfo::getPeaceAngerMax() const
{
	return m_iPeaceAngerMax;
}

// The borders of all cities reach this much further than the culture level says (added by this mod)
int CvTraitInfo::getCultureRangeChange() const
{
	return m_iCultureRangeChange;
}

// The player cannot adopt the civic (added by this mod)
bool CvTraitInfo::isForbiddenCivic(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabForbiddenCivic ? m_pabForbiddenCivic[i] : false;
}

// The culture level new cities start with (NO_CULTURELEVEL: none)
int CvTraitInfo::getNewCityCultureLevel() const
{
	return m_iNewCityCultureLevel;
}

// Extra population of the cities the player founds (added by this mod)
int CvTraitInfo::getNewCityPopulation() const
{
	return m_iNewCityPopulation;
}

// The state religion spreads to the cities the player founds (added by this mod)
bool CvTraitInfo::isNewCityStateReligion() const
{
	return m_bNewCityStateReligion;
}

// Revolt time % of the cities the player conquers (added by this mod)
int CvTraitInfo::getOccupationTimeModifier() const
{
	return m_iOccupationTimeModifier;
}

// Conquest/iCulturePercent, iBuildingSurviveModifier, bNoPopulationLoss, bNoAutoRaze (added by this mod)
int CvTraitInfo::getConquestCulturePercent() const
{
	return m_iConquestCulturePercent;
}

int CvTraitInfo::getConquestBuildingSurviveModifier() const
{
	return m_iConquestBuildingSurviveModifier;
}

bool CvTraitInfo::isNoConquestPopulationLoss() const
{
	return m_bNoConquestPopulationLoss;
}

bool CvTraitInfo::isNoAutoRaze() const
{
	return m_bNoAutoRaze;
}

// Conquest/bLearnUniqueUnits and bLearnUniqueBuildings (added by this mod): see CvPlayer::learnConquestUniques
bool CvTraitInfo::isConquestLearnUniqueUnits() const
{
	return m_bConquestLearnUniqueUnits;
}

bool CvTraitInfo::isConquestLearnUniqueBuildings() const
{
	return m_bConquestLearnUniqueBuildings;
}

// iStateReligionHappiness (added by this mod): as the civics, happiness of the cities with the state religion
int CvTraitInfo::getStateReligionHappiness() const
{
	return m_iStateReligionHappiness;
}

// iStateReligionHealth (added by this mod): health of the cities with the state religion
int CvTraitInfo::getStateReligionHealth() const
{
	return m_iStateReligionHealth;
}

// iStateReligionGreatPeopleRateModifier (added by this mod): as the civics, the great people rate of the cities with the state religion % (CvPlayer::processTrait)
int CvTraitInfo::getStateReligionGreatPeopleRateModifier() const
{
	return m_iStateReligionGreatPeopleRateModifier;
}

// iStateReligionUnitProductionModifier (added by this mod): as the civics, the production % of the units in the cities with the state religion (CvPlayer::processTrait)
int CvTraitInfo::getStateReligionUnitProductionModifier() const
{
	return m_iStateReligionUnitProductionModifier;
}

// iStateReligionBuildingProductionModifier (added by this mod): as the civics, the production % of the buildings in the cities with the state religion (CvPlayer::processTrait)
int CvTraitInfo::getStateReligionBuildingProductionModifier() const
{
	return m_iStateReligionBuildingProductionModifier;
}

// iStateReligionFreeExperience (added by this mod): as the civics, the experience of the units built in the cities with the state religion (CvPlayer::processTrait)
int CvTraitInfo::getStateReligionFreeExperience() const
{
	return m_iStateReligionFreeExperience;
}

// GreatPeopleClassRateModifiers (added by this mod): % of the great people points of a unit class (CvCity::getGreatPeopleUnitRateWithTraits)
int CvTraitInfo::getGreatPeopleClassRateModifier(int iUnitClass) const
{
	return (iUnitClass >= 0 && iUnitClass < (int)m_aiGreatPeopleClassRateModifier.size()) ? m_aiGreatPeopleClassRateModifier[iUnitClass] : 0;
}

bool CvTraitInfo::isAnyGreatPeopleClassRateModifier() const
{
	return !m_aiGreatPeopleClassRateModifier.empty();
}

// iEnemyMoveCostInBorders (added by this mod): extra moves of the land units at war in the territory (CvPlot::movementCost)
int CvTraitInfo::getEnemyMoveCostInBorders() const
{
	return m_iEnemyMoveCostInBorders;
}

// iRevengeCombatPercent (added by this mod): combat % against the players that have conquered a city of the player
int CvTraitInfo::getRevengeCombatPercent() const
{
	return m_iRevengeCombatPercent;
}

// SpecialBuildingProductionModifiers (added by this mod): production % of the buildings of a special building type
int CvTraitInfo::getSpecialBuildingProductionModifier(int iSpecialBuilding) const
{
	return (iSpecialBuilding >= 0 && iSpecialBuilding < (int)m_aiSpecialBuildingProductionModifier.size()) ? m_aiSpecialBuildingProductionModifier[iSpecialBuilding] : 0;
}

// KnownTechCost/iModifier and KnownTechCost/iLimit (added by this mod): see CvTeam::getResearchCost
int CvTraitInfo::getKnownTechCostModifier() const
{
	return m_iKnownTechCostModifier;
}

int CvTraitInfo::getKnownTechCostLimit() const
{
	return m_iKnownTechCostLimit;
}

// Conquest/bHeal (added by this mod): the units that conquer a city are healed (CvUnit::setXY)
bool CvTraitInfo::isConquestHeal() const
{
	return m_bConquestHeal;
}

// bStackCityDefense (added by this mod): the culture defense and the building defense of the cities are added
// instead of using the larger one (CvCity::getTotalDefense)
bool CvTraitInfo::isStackCityDefense() const
{
	return m_bStackCityDefense;
}

// bRevealWonderProduction (added by this mod): the player sees the world wonders the other players are building
// (CvGameTextMgr::getRevealedPlansString)
bool CvTraitInfo::isRevealWonderProduction() const
{
	return m_bRevealWonderProduction;
}

// bRevealWarPlans (added by this mod): the player sees the war plans of the other players (CvGameTextMgr::getRevealedPlansString)
bool CvTraitInfo::isRevealWarPlans() const
{
	return m_bRevealWarPlans;
}

// bRevealHiddenAttitude (added by this mod): the player sees the parts of the attitude of the AI players that BtS does
// not show (CvPlayerAI::AI_getHiddenAttitude), also between two AI players
bool CvTraitInfo::isRevealHiddenAttitude() const
{
	return m_bRevealHiddenAttitude;
}

// bLongCountCalendar (added by this mod): the date at the top right of the screen of the player is in the Maya Long Count
// (CvGameTextMgr::setInterfaceTime)
bool CvTraitInfo::isLongCountCalendar() const
{
	return m_bLongCountCalendar;
}

// bImprovementUpgradeUnworked (added by this mod): the improvements in the borders of the player upgrade (cottage to hamlet
// and so on) also when no citizen works the plot (CvPlot::doTurn)
bool CvTraitInfo::isImprovementUpgradeUnworked() const
{
	return m_bImprovementUpgradeUnworked;
}

// bUnsettleGreatPeople (added by this mod): a great person settled in a city of the player can become a unit again by
// clicking its icon on the city screen (CvCity::unsettleGreatPerson)
bool CvTraitInfo::isUnsettleGreatPeople() const
{
	return m_bUnsettleGreatPeople;
}

// bIgnoreEvents (added by this mod): the popup of a random event of the player has one more choice that ignores the event
// (CvDLLButtonPopup::launchEventPopup, CvPlayer::ignoreEvent). The AI does not use it
bool CvTraitInfo::isIgnoreEvents() const
{
	return m_bIgnoreEvents;
}

// iEventChanceChange (added by this mod): added to iEventChancePerTurn of the era (out of EVENT_PROBABILITY_ROLL_SIDES,
// 100) in the check for a new random event each turn (CvPlayer::doEvents)
int CvTraitInfo::getEventChanceChange() const
{
	return m_iEventChanceChange;
}

// bNoFirstEventDelay (added by this mod): new random events can come from the first turn, without waiting for
// FIRST_EVENT_DELAY_TURNS (20) turns (CvPlayer::doEvents)
bool CvTraitInfo::isNoFirstEventDelay() const
{
	return m_bNoFirstEventDelay;
}

// Deals/bBuyCities (added by this mod): the AI sells its cities other than the capital to the player in peace, too
// (CvPlayerAI::AI_cityTrade). Without it, the AI gives cities only in the deals of the end of a war
bool CvTraitInfo::isBuyCities() const
{
	return m_bBuyCities;
}

// Deals/iCityValueModifier (added by this mod): % to the value of the cities that the player receives in deals, that is the
// price that the AI asks for them (CvPlayerAI::AI_cityTradeVal). -50: half price
int CvTraitInfo::getDealCityValueModifier() const
{
	return m_iDealCityValueModifier;
}

// Deals/iGoldLimitModifier (added by this mod): % to the most gold (lump sum and per turn) that an AI gives to the player in a
// deal (CvPlayerAI::AI_maxGoldTrade, AI_maxGoldPerTurnTrade). The gold that the AI has and earns still caps it
int CvTraitInfo::getDealGoldLimitModifier() const
{
	return m_iDealGoldLimitModifier;
}

// Deals/iGoldValueModifier (added by this mod): % to the value that the AI puts on the gold (lump sum and per turn) that the
// player gives in deals (CvPlayerAI::AI_goldTradeValuePercent). 50: the AI asks for 2/3 of the gold
int CvTraitInfo::getDealGoldValueModifier() const
{
	return m_iDealGoldValueModifier;
}

// CultureFlip/bNoFlip (added by this mod): the cities of the player do not flip to another player by culture. The revolts still
// occur (occupation, damage to the defenders) (CvPlot::doCulture)
bool CvTraitInfo::isNoCultureFlip() const
{
	return m_bNoCultureFlip;
}

// CultureFlip/bNoRevoltAndNoFlip (added by this mod): no revolt by foreign culture occurs in the cities of the player, so they
// do not flip either (CvCity::getRevoltTestProbability)
bool CvTraitInfo::isNoRevoltAndNoCultureFlip() const
{
	return m_bNoRevoltAndNoCultureFlip;
}

// CultureFlip/bInstantFlip (added by this mod): foreign cities flip to the player at the first revolt by the culture of the
// player, without the warning revolts (NUM_WARNING_REVOLTS) (CvPlot::doCulture)
bool CvTraitInfo::isInstantCultureFlip() const
{
	return m_bInstantCultureFlip;
}

// CultureFlip/iRevoltChance (added by this mod): percent change of the chance of revolt in foreign cities by the culture of the
// player (CvCity::getRevoltTestProbability)
int CvTraitInfo::getCultureRevoltChance() const
{
	return m_iCultureRevoltChance;
}

// iNewWorldCityPopulation (added by this mod): population of the cities the player founds in the New World
int CvTraitInfo::getNewWorldCityPopulation() const
{
	return m_iNewWorldCityPopulation;
}

// iOverseasCityPopulation (added by this mod): population of the cities the player founds overseas
int CvTraitInfo::getOverseasCityPopulation() const
{
	return m_iOverseasCityPopulation;
}

// iOverseasMaintenanceModifier (added by this mod): maintenance % of the overseas cities
int CvTraitInfo::getOverseasMaintenanceModifier() const
{
	return m_iOverseasMaintenanceModifier;
}

// iDistantTradeRouteModifier (added by this mod): trade route yield % with cities far from the capital
int CvTraitInfo::getDistantTradeRouteModifier() const
{
	return m_iDistantTradeRouteModifier;
}

// EnemyAttrition/iPercent (added by this mod): damage (% of the max HP) at the start of each turn of the player to the units
// of the players at war with it in its territory (CvPlayer::doEnemyAttrition)
int CvTraitInfo::getEnemyAttritionInBorders() const
{
	return m_iEnemyAttritionInBorders;
}

// EnemyAttrition/iLimit (added by this mod): the attrition does not damage a unit beyond this % of its max HP (100, the
// default: it can kill)
int CvTraitInfo::getEnemyAttritionLimit() const
{
	return m_iEnemyAttritionLimit;
}

// iAnarchyGoldenAgePercent (added by this mod): when the anarchy of a change of civics or religion ends, a golden age of
// this % of the turns spent in that anarchy (CvPlayer::changeAnarchyTurns)
int CvTraitInfo::getAnarchyGoldenAgePercent() const
{
	return m_iAnarchyGoldenAgePercent;
}

// iTechCostModifier (added by this mod): research cost % of the techs (the average of the members of the team,
// CvTeam::getResearchCost)
int CvTraitInfo::getTechCostModifier() const
{
	return m_iTechCostModifier;
}

// iAttitudeChange (added by this mod): attitude of the other players (the AI) toward the player
// (CvPlayerAI::AI_getAttitudeVal)
int CvTraitInfo::getAttitudeChange() const
{
	return m_iAttitudeChange;
}

// iRouteMovementModifierInBorders (added by this mod): % to the movement cost of routes between plots of the team of the
// player (CvPlot::movementCost). -50: the roads of the player are twice as fast there
int CvTraitInfo::getRouteMovementModifierInBorders() const
{
	return m_iRouteMovementModifierInBorders;
}

// iCombatPercent (added by this mod): combat % of all the units of the player while it has the trait, in attack and defense
// (CvUnit::getExtraCombatPercent). Summed when asked, so a trait given and taken by conditions changes the existing units
int CvTraitInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

// iHappinessPerReligion and CommercePerReligion (added by this mod): happiness and commerce of each city of the player for each
// religion in the city (CvCity::getTraitReligionHappiness, getTraitReligionCommerce)
int CvTraitInfo::getHappinessPerReligion() const
{
	return m_iHappinessPerReligion;
}

int CvTraitInfo::getCommercePerReligion(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCommercePerReligion ? m_paiCommercePerReligion[i] : 0;
}

// iDefenseCombatPercent (added by this mod): combat % of the units of the player when they defend (CvUnit::maxCombatStr)
int CvTraitInfo::getDefenseCombatPercent() const
{
	return m_iDefenseCombatPercent;
}

// WorldWonderCommerceChanges and GlobalWorldWonderCommerceChanges (added by this mod, as the tags of the buildings): commerce
// of each city of the player for each world wonder in the city, and for each world wonder of the player
// (CvCity::getTraitWorldWonderCommerce)
int CvTraitInfo::getWorldWonderCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiWorldWonderCommerceChange ? m_paiWorldWonderCommerceChange[i] : 0;
}

int CvTraitInfo::getGlobalWorldWonderCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiGlobalWorldWonderCommerceChange ? m_paiGlobalWorldWonderCommerceChange[i] : 0;
}

int* CvTraitInfo::getWorldWonderCommerceChangeArray() const
{
	return m_paiWorldWonderCommerceChange;
}

int* CvTraitInfo::getGlobalWorldWonderCommerceChangeArray() const
{
	return m_paiGlobalWorldWonderCommerceChange;
}

// iEnemyWarWearinessModifier (added by this mod, as the tag of the buildings): war weariness % of the teams at war with the team
// of the player, against that team (CvTeam::changeEnemyWarWearinessModifier in CvPlayer::processTrait)
int CvTraitInfo::getEnemyWarWearinessModifier() const
{
	return m_iEnemyWarWearinessModifier;
}

// Conquest/bStateReligion (added by this mod): the state religion spreads to the cities the player conquers
bool CvTraitInfo::isConquestStateReligion() const
{
	return m_bConquestStateReligion;
}

// Conquest/ReligionBuildings (added by this mod): the special buildings (temple, monastery, cathedral) of the state religion
// a conquered city gets, when it could build them (CvPlayer::acquireCity)
bool CvTraitInfo::isConquestReligionBuilding(int iSpecialBuilding) const
{
	FAssertMsg(iSpecialBuilding < GC.getNumSpecialBuildingInfos(), "Index out of bounds");
	FAssertMsg(iSpecialBuilding > -1, "Index out of bounds");
	return m_pabConquestReligionBuilding ? m_pabConquestReligionBuilding[iSpecialBuilding] : false;
}

// NoTechCivics (added by this mod): the player can adopt the civic without its tech (CvPlayer::canDoCivics)
bool CvTraitInfo::isNoTechCivic(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabNoTechCivic ? m_pabNoTechCivic[i] : false;
}

// CivicOptionExtraSlots (added by this mod): more civics of the civic option the player can adopt at the same time
int CvTraitInfo::getCivicOptionExtraSlots(int iCivicOption) const
{
	return (iCivicOption >= 0 && iCivicOption < (int)m_aiCivicOptionExtraSlots.size()) ? m_aiCivicOptionExtraSlots[iCivicOption] : 0;
}

// ExplorationRewards (added by this mod): each entry gives iPercent/100 of the commerce for each plot of the kind the player
// reveals during its turn (CvPlayer::doExplorationRewards)
int CvTraitInfo::getNumExplorationRewards() const
{
	return (int)m_aiExplorationRewardPercent.size();
}

int CvTraitInfo::getExplorationRewardPlotType(int iIndex) const
{
	return (iIndex >= 0 && iIndex < getNumExplorationRewards()) ? m_aiExplorationRewardPlotType[iIndex] : NO_EXPLORE_PLOT;
}

int CvTraitInfo::getExplorationRewardCommerceType(int iIndex) const
{
	return (iIndex >= 0 && iIndex < getNumExplorationRewards()) ? m_aiExplorationRewardCommerceType[iIndex] : NO_COMMERCE;
}

int CvTraitInfo::getExplorationRewardPercent(int iIndex) const
{
	return (iIndex >= 0 && iIndex < getNumExplorationRewards()) ? m_aiExplorationRewardPercent[iIndex] : 0;
}

// The sum of iPercent of the entries with this kind of plot and commerce
int CvTraitInfo::getExplorationRewardPercentFor(int iExplorePlot, int iCommerce) const
{
	int iPercent = 0;
	int iI;

	for (iI = 0; iI < getNumExplorationRewards(); iI++)
	{
		if (m_aiExplorationRewardPlotType[iI] == iExplorePlot && m_aiExplorationRewardCommerceType[iI] == iCommerce)
		{
			iPercent += m_aiExplorationRewardPercent[iI];
		}
	}

	return iPercent;
}

// iMaxNationalWondersPerCity (added by this mod): replaces MAX_NATIONAL_WONDERS_PER_CITY of GlobalDefines for the player
// (CvPlayer::getMaxNationalWondersPerCity). 0: not set, -1: no limit
int CvTraitInfo::getMaxNationalWondersPerCity() const
{
	return m_iMaxNationalWondersPerCity;
}

// iMaxNationalWondersPerCityForOCC (added by this mod): the same for MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC (a human player
// in a One-City Challenge game)
int CvTraitInfo::getMaxNationalWondersPerCityForOCC() const
{
	return m_iMaxNationalWondersPerCityForOCC;
}

// bHumanCanBeVassal (added by this mod): a human player with the trait can become the vassal of an AI player (BtS allows
// human vassals only of humans: CvPlayer::canTradeItem)
bool CvTraitInfo::isHumanCanBeVassal() const
{
	return m_bHumanCanBeVassal;
}

// bBasicTrait (added by this mod): one of the basic traits (the 11 leader traits of BtS). bGainTraitOnCapitalCapture and
// bGainTraitsOnElimination take these from the leader of the conquered player (CvPlayer::gainBasicTraitsOf)
bool CvTraitInfo::isBasicTrait() const
{
	return m_bBasicTrait;
}

// bGainTraitOnCapitalCapture (added by this mod): conquering the capital of a player gives one random basic trait of that
// player that we do not have (CvPlayer::acquireCity)
bool CvTraitInfo::isGainTraitOnCapitalCapture() const
{
	return m_bGainTraitOnCapitalCapture;
}

// bGainTraitsOnElimination (added by this mod): eliminating a player gives all basic traits of that player that we do not
// have (CvPlayer::setAlive; the player who conquered its last city)
bool CvTraitInfo::isGainTraitsOnElimination() const
{
	return m_bGainTraitsOnElimination;
}

// RandomBasicTraits (iTurns, iCount; added by this mod; the Insane trait of FFH2): every iTurns turns (scaled by the game
// speed), the player gets iCount random basic traits in place of the ones it got last time (CvPlayer::doRandomBasicTraits).
// isRandomBasicTraits: the element is there
bool CvTraitInfo::isRandomBasicTraits() const
{
	return m_bRandomBasicTraits;
}

int CvTraitInfo::getRandomBasicTraitTurns() const
{
	return m_iRandomBasicTraitTurns;
}

int CvTraitInfo::getRandomBasicTraitCount() const
{
	return m_iRandomBasicTraitCount;
}

// GainTraitsOnFriendly (bAll; added by this mod): when an AI player first becomes friendly to the player, the player gets the
// basic traits of that player it does not have yet (CvPlayer::doGainTraitsOnFriendly). bAll 0: one of them at random, 1: all.
// isGainTraitsOnFriendly: the element is there
bool CvTraitInfo::isGainTraitsOnFriendly() const
{
	return m_bGainTraitsOnFriendly;
}

bool CvTraitInfo::isGainTraitsOnFriendlyAll() const
{
	return m_bGainTraitsOnFriendlyAll;
}

// iMaxExperienceAfterUpgrade (added by this mod): replaces MAX_EXPERIENCE_AFTER_UPGRADE, see
// CvPlayer::getMaxExperienceAfterUpgrade
int CvTraitInfo::getMaxExperienceAfterUpgrade() const
{
	return m_iMaxExperienceAfterUpgrade;
}

// iExpOutsideBordersChange (added by this mod): experience added to the combat experience outside our borders
int CvTraitInfo::getExpOutsideBordersChange() const
{
	return m_iExpOutsideBordersChange;
}

// iWeLoveTheKingChance (added by this mod): % chance each turn that a city able to celebrate celebrates
int CvTraitInfo::getWeLoveTheKingChance() const
{
	return m_iWeLoveTheKingChance;
}

// SpecialistSlotChanges (added by this mod): slots of the specialist in every city, -1 (any negative value) for no limit
int CvTraitInfo::getSpecialistSlotChange(int iSpecialist) const
{
	if (iSpecialist < 0 || iSpecialist >= (int)m_aiSpecialistSlotChange.size())
	{
		return 0;
	}
	return m_aiSpecialistSlotChange[iSpecialist];
}

// RevealBonuses (added by this mod): the team sees the bonus without its TechReveal, as the random events that reveal a
// bonus. Using it (TechCityTrade) still needs the tech (CvTeam::isTraitRevealedBonus)
bool CvTraitInfo::isRevealBonus(int iBonus) const
{
	FAssertMsg(iBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(iBonus > -1, "Index out of bounds");
	return m_pabRevealBonus ? m_pabRevealBonus[iBonus] : false;
}

// NoTechProcesses (added by this mod): the cities can maintain the process without its tech (CvPlayer::canMaintain)
bool CvTraitInfo::isNoTechProcess(int iProcess) const
{
	FAssertMsg(iProcess > -1, "Index out of bounds");
	return (m_pabNoTechProcess != NULL && iProcess < GC.getNumProcessInfos()) ? m_pabNoTechProcess[iProcess] : false;
}

// Conscription/bNoExperiencePenalty (added by this mod): drafted units get all the experience of the city, not half
bool CvTraitInfo::isNoConscriptExperiencePenalty() const
{
	return m_bNoConscriptExperiencePenalty;
}

// FreeTechs (added by this mod): the team gets the tech at the start of the game (CvPlayer::initTraitFreeTechs)
bool CvTraitInfo::isFreeTech(int iTech) const
{
	FAssertMsg(iTech < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(iTech > -1, "Index out of bounds");
	return m_pabFreeTech ? m_pabFreeTech[iTech] : false;
}

// Element of a vector of values per info, 0 when the vector is empty (not set) or the index is out of it (added by this mod)
static int getVectorValue(const std::vector<int>& aiValues, int i)
{
	return ((i >= 0 && i < (int)aiValues.size()) ? aiValues[i] : 0);
}

// YieldChanges (added by this mod): yield of every city, as CommerceChanges for the commerce (CvCity::getTraitYield)
int CvTraitInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldChange ? m_paiYieldChange[i] : 0;
}

int* CvTraitInfo::getYieldChangeArray() const
{
	return m_paiYieldChange;
}

// iHealthPerReligion (added by this mod): health of a city for each religion in it, as iHappinessPerReligion
int CvTraitInfo::getHealthPerReligion() const
{
	return m_iHealthPerReligion;
}

// YieldPerReligion (added by this mod): yield of a city for each religion in it, as CommercePerReligion
int CvTraitInfo::getYieldPerReligion(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldPerReligion ? m_paiYieldPerReligion[i] : 0;
}

// FeatureHappinessChanges (added by this mod): as the civics, happiness for each such feature in the city radius
int CvTraitInfo::getFeatureHappinessChange(int iFeature) const
{
	return getVectorValue(m_aiFeatureHappinessChange, iFeature);
}

// WorldWonderYieldChanges / GlobalWorldWonderYieldChanges (added by this mod): as the commerce versions
int CvTraitInfo::getWorldWonderYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiWorldWonderYieldChange ? m_paiWorldWonderYieldChange[i] : 0;
}

int CvTraitInfo::getGlobalWorldWonderYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiGlobalWorldWonderYieldChange ? m_paiGlobalWorldWonderYieldChange[i] : 0;
}

// SpecialistExtraYields (added by this mod): yield of every specialist, as SpecialistExtraCommerces
int CvTraitInfo::getSpecialistExtraYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiSpecialistExtraYield ? m_paiSpecialistExtraYield[i] : 0;
}

int* CvTraitInfo::getSpecialistExtraYieldArray() const
{
	return m_paiSpecialistExtraYield;
}

// iNonStateReligionHappiness (added by this mod): as the civics, happiness for each religion other than the state religion
int CvTraitInfo::getNonStateReligionHappiness() const
{
	return m_iNonStateReligionHappiness;
}

// BuildingHappinessChanges / BuildingHealthChanges (added by this mod): as the civics, for the buildings of the class
int CvTraitInfo::getBuildingHappinessChange(int iBuildingClass) const
{
	return getVectorValue(m_aiBuildingHappinessChange, iBuildingClass);
}

int CvTraitInfo::getBuildingHealthChange(int iBuildingClass) const
{
	return getVectorValue(m_aiBuildingHealthChange, iBuildingClass);
}

// BonusHappinessChanges / BonusHealthChanges (added by this mod): as the buildings, for each city with the bonus
int CvTraitInfo::getBonusHappinessChange(int iBonus) const
{
	return getVectorValue(m_aiBonusHappinessChange, iBonus);
}

int CvTraitInfo::getBonusHealthChange(int iBonus) const
{
	return getVectorValue(m_aiBonusHealthChange, iBonus);
}

bool CvTraitInfo::isAnyBonusHappinessOrHealthChange() const
{
	return (!m_aiBonusHappinessChange.empty() || !m_aiBonusHealthChange.empty());
}

// TerrainTrades (added by this mod): as the techs, the terrain connects the trade network of the team (CvPlayer::processTrait)
bool CvTraitInfo::isTerrainTrade(int iTerrain) const
{
	FAssertMsg(iTerrain < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(iTerrain > -1, "Index out of bounds");
	return m_pabTerrainTrade ? m_pabTerrainTrade[iTerrain] : false;
}

// CapitalTerrainYieldPercents (added by this mod): yield of the capital, in 1/100, for each plot of the terrain in its city
// radius (CvCity::getTraitCapitalTerrainYield). Not a change of the plots of the terrain
int CvTraitInfo::getCapitalTerrainYieldPercent(int iTerrain, int iYield) const
{
	if (iTerrain < 0 || iTerrain >= (int)m_aaiCapitalTerrainYieldPercent.size() || iYield < 0 || iYield >= NUM_YIELD_TYPES)
	{
		return 0;
	}
	return m_aaiCapitalTerrainYieldPercent[iTerrain][iYield];
}

bool CvTraitInfo::isAnyCapitalTerrainYieldPercent() const
{
	return !m_aaiCapitalTerrainYieldPercent.empty();
}

// The item applies to the unit or building (added by this mod). No flag: any unit or building
bool TraitHurry::isTarget(UnitTypes eUnit, BuildingTypes eBuilding) const
{
	if (!bUnitsOnly && !bBuildingsOnly && !bWondersOnly && !bWorldWondersOnly && !bNationalWondersOnly)
	{
		return (eUnit != NO_UNIT || eBuilding != NO_BUILDING);
	}

	if (eUnit != NO_UNIT)
	{
		return bUnitsOnly;
	}

	if (eBuilding != NO_BUILDING)
	{
		BuildingClassTypes eClass = (BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType();

		if (bBuildingsOnly || (bWondersOnly && isLimitedWonderClass(eClass)) || (bWorldWondersOnly && isWorldWonderClass(eClass)) || (bNationalWondersOnly && isNationalWonderClass(eClass)))
		{
			return true;
		}
	}

	return false;
}

// Hurrys (added by this mod): see TraitHurry and CvPlayer::isTraitHurry
int CvTraitInfo::getNumHurrys() const
{
	return (int)m_aHurrys.size();
}

const TraitHurry& CvTraitInfo::getHurry(int i) const
{
	FAssertMsg(i >= 0 && i < getNumHurrys(), "Index out of bounds");
	return m_aHurrys[i];
}

int CvTraitInfo::getHurryType(int i) const
{
	return getHurry(i).iHurry;
}

int CvTraitInfo::getHurryCostModifier(int i) const
{
	return getHurry(i).iCostModifier;
}

int CvTraitInfo::getHurryExperience(int i) const
{
	return getHurry(i).iExperience;
}

// FreeBonuses (added by this mod): as FreeBonus of the buildings, the player has the bonuses in the trade network of the
// capital, as the imported ones (CvPlot::updatePlotGroupBonus), so they follow the capital
int CvTraitInfo::getFreeBonus(int iBonus) const
{
	return getVectorValue(m_aiFreeBonus, iBonus);
}

bool CvTraitInfo::isAnyFreeBonus() const
{
	return !m_aiFreeBonus.empty();
}

// BonusProductionModifiers (added by this mod): production % in the cities with the bonus, of everything, or of the world
// wonders only (bWorldWondersOnly: getBonusWonderProductionModifier)
int CvTraitInfo::getBonusProductionModifier(int iBonus) const
{
	return getVectorValue(m_aiBonusProductionModifier, iBonus);
}

int CvTraitInfo::getBonusWonderProductionModifier(int iBonus) const
{
	return getVectorValue(m_aiBonusWonderProductionModifier, iBonus);
}

// CommerceHappinesses (as the buildings) and CommerceHealths (added by this mod): happiness / health of every city per 100%
// of the commerce rate (10 = +1 per 10%)
int CvTraitInfo::getCommerceHappiness(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCommerceHappiness ? m_paiCommerceHappiness[i] : 0;
}

int CvTraitInfo::getCommerceHealth(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCommerceHealth ? m_paiCommerceHealth[i] : 0;
}

// iBaseFreeUnits (added by this mod): as the civics, units the player supports for free
int CvTraitInfo::getBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}

// iBaseFreeMilitaryUnits (added by this mod): as the civics, military units the player supports for free
int CvTraitInfo::getBaseFreeMilitaryUnits() const
{
	return m_iBaseFreeMilitaryUnits;
}

// iFreeUnitsPopulationPercent (added by this mod): as the civics, free units: % of the total population
int CvTraitInfo::getFreeUnitsPopulationPercent() const
{
	return m_iFreeUnitsPopulationPercent;
}

// iFreeMilitaryUnitsPopulationPercent (added by this mod): as the civics, free military units: % of the total population
int CvTraitInfo::getFreeMilitaryUnitsPopulationPercent() const
{
	return m_iFreeMilitaryUnitsPopulationPercent;
}

// iGoldPerUnit (added by this mod): as the civics, gold for each unit beyond the free ones
int CvTraitInfo::getGoldPerUnit() const
{
	return m_iGoldPerUnit;
}

// iGoldPerMilitaryUnit (added by this mod): as the civics, gold for each military unit beyond the free ones
int CvTraitInfo::getGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}

// iCorporationMaintenanceModifier (added by this mod): as the civics, maintenance % of the corporations
int CvTraitInfo::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}

// iMaintenanceModifier (added by this mod): as the buildings, maintenance % of every city (CvCity::getMaintenanceModifier)
int CvTraitInfo::getMaintenanceModifier() const
{
	return m_iMaintenanceModifier;
}

// iMaxConscript (added by this mod): as the civics, units the player can draft per turn, before the world size modifier
int CvTraitInfo::getMaxConscript() const
{
	return m_iMaxConscript;
}

// Conscription/iPopulationModifier (added by this mod): % to the unit cost when the population lost by a draft is counted. See CvCity::getConscriptPopulation
int CvTraitInfo::getConscriptPopulationModifier() const
{
	return m_iConscriptPopulationModifier;
}

// Conscription/iMinCityPopulationChange (added by this mod): added to CONSCRIPT_MIN_CITY_POPULATION. See CvCity::conscriptMinCityPopulation
int CvTraitInfo::getConscriptMinCityPopulationChange() const
{
	return m_iConscriptMinCityPopulationChange;
}

// Conscription/iMinCulturePercentChange (added by this mod): added to CONSCRIPT_MIN_CULTURE_PERCENT. See CvCity::conscriptMinCulturePercent
int CvTraitInfo::getConscriptMinCulturePercentChange() const
{
	return m_iConscriptMinCulturePercentChange;
}

// Conscription/iPopAngerChange (added by this mod): added to CONSCRIPT_POP_ANGER. See CvCity::conscriptPopAnger
int CvTraitInfo::getConscriptPopAngerChange() const
{
	return m_iConscriptPopAngerChange;
}

// Conscription/iAngerLengthPercent (added by this mod): % to the turns of the anger of a draft. See CvCity::flatConscriptAngerLength
int CvTraitInfo::getConscriptAngerLengthPercent() const
{
	return m_iConscriptAngerLengthPercent;
}

// bMilitaryFoodProduction (added by this mod): as the civics, the food surplus goes into military units
bool CvTraitInfo::isMilitaryFoodProduction() const
{
	return m_bMilitaryFoodProduction;
}

// iHealRateChange (added by this mod): as the buildings, extra heal % of the units in every city (CvCity::getHealRate)
int CvTraitInfo::getHealRateChange() const
{
	return m_iHealRateChange;
}

// DomainFreeExperiences / DomainProductionModifiers (added by this mod): as the buildings, for every city
int CvTraitInfo::getDomainFreeExperience(int iDomain) const
{
	return getVectorValue(m_aiDomainFreeExperience, iDomain);
}

int CvTraitInfo::getDomainProductionModifier(int iDomain) const
{
	return getVectorValue(m_aiDomainProductionModifier, iDomain);
}

// iLargestCityHappiness (added by this mod): as the civics, happiness of the largest cities (CvPlayer::processTrait)
int CvTraitInfo::getLargestCityHappiness() const
{
	return m_iLargestCityHappiness;
}

// iHappyPerMilitaryUnit (added by this mod): as the civics, happiness for each military unit in a city (CvPlayer::processTrait)
int CvTraitInfo::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}

// iFoodKept (added by this mod): as the buildings, % of the food kept after growth in every city (CvCity::getMaxFoodKeptPercent)
int CvTraitInfo::getFoodKept() const
{
	return m_iFoodKept;
}

// bNoUnhealthyPopulation (added by this mod): as the civics, no unhealthiness from the population (CvPlayer::processTrait)
bool CvTraitInfo::isNoUnhealthyPopulation() const
{
	return m_bNoUnhealthyPopulation;
}

// bBuildingOnlyHealthy (added by this mod): as the civics, no unhealthiness from the buildings (CvPlayer::processTrait)
bool CvTraitInfo::isBuildingOnlyHealthy() const
{
	return m_bBuildingOnlyHealthy;
}

// iCoastalTradeRoutes (added by this mod): as the buildings, trade routes of every coastal city (CvPlayer::processTrait)
int CvTraitInfo::getCoastalTradeRoutes() const
{
	return m_iCoastalTradeRoutes;
}

// iTradeRouteModifier (added by this mod): as the buildings, trade route yield % of every city (CvCity::totalTradeModifier)
int CvTraitInfo::getTradeRouteModifier() const
{
	return m_iTradeRouteModifier;
}

// iForeignTradeRouteModifier (added by this mod): as the buildings, foreign trade route yield % of every city (CvCity::getForeignTradeRouteModifier)
int CvTraitInfo::getForeignTradeRouteModifier() const
{
	return m_iForeignTradeRouteModifier;
}

// iAllCityDefense (added by this mod): as the buildings, defense % of every city (CvPlayer::processTrait)
int CvTraitInfo::getAllCityDefense() const
{
	return m_iAllCityDefense;
}

// iBombardDefense (added by this mod): as the buildings, % less damage to the defense of every city from bombardment (CvCity::getBuildingBombardDefense)
int CvTraitInfo::getBombardDefense() const
{
	return m_iBombardDefense;
}

// iEspionageDefense (added by this mod): as the buildings, defense % of every city against espionage (CvCity::getEspionageDefenseModifier)
int CvTraitInfo::getEspionageDefense() const
{
	return m_iEspionageDefense;
}

// ImprovedBonusYieldChanges (added by this mod): yields of a plot with the bonus and an improvement that trades it
int CvTraitInfo::getImprovedBonusYieldChange(int iBonus, int iYield) const
{
	if ((iBonus < 0) || (iBonus >= (int)m_aaiImprovedBonusYieldChange.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiImprovedBonusYieldChange[iBonus][iYield];
}

// HappyYieldPercents, HappyCommercePercents, HealthyYieldPercents and HealthyCommercePercents (added by this mod): the
// yield or commerce per extra happiness (health) in 1/100, see CvCity::updateMiseryChanges
int CvTraitInfo::getHappyYieldPercent(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiHappyYieldPercent ? m_paiHappyYieldPercent[i] : 0;
}

int* CvTraitInfo::getHappyYieldPercentArray() const
{
	return m_paiHappyYieldPercent;
}

int CvTraitInfo::getHappyCommercePercent(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiHappyCommercePercent ? m_paiHappyCommercePercent[i] : 0;
}

int* CvTraitInfo::getHappyCommercePercentArray() const
{
	return m_paiHappyCommercePercent;
}

int CvTraitInfo::getHealthyYieldPercent(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiHealthyYieldPercent ? m_paiHealthyYieldPercent[i] : 0;
}

int* CvTraitInfo::getHealthyYieldPercentArray() const
{
	return m_paiHealthyYieldPercent;
}

int CvTraitInfo::getHealthyCommercePercent(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiHealthyCommercePercent ? m_paiHealthyCommercePercent[i] : 0;
}

int* CvTraitInfo::getHealthyCommercePercentArray() const
{
	return m_paiHealthyCommercePercent;
}

const TCHAR* CvTraitInfo::getShortDescription() const
{
	return m_szShortDescription; 
}

void CvTraitInfo::setShortDescription(const TCHAR* szVal)
{
	m_szShortDescription = szVal; 
}

// Arrays

int CvTraitInfo::getExtraYieldThreshold(int i) const
{
	return m_paiExtraYieldThreshold ? m_paiExtraYieldThreshold[i] : -1; 
}

int CvTraitInfo::getTradeYieldModifier(int i) const
{
	return m_paiTradeYieldModifier ? m_paiTradeYieldModifier[i] : -1; 
}

int CvTraitInfo::getCommerceChange(int i) const			
{
	return m_paiCommerceChange ? m_paiCommerceChange[i] : -1; 
}

int CvTraitInfo::getCommerceModifier(int i) const		
{
	return m_paiCommerceModifier ? m_paiCommerceModifier[i] : -1; 
}

int CvTraitInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldModifier ? m_paiYieldModifier[i] : 0;
}

int* CvTraitInfo::getYieldModifierArray() const
{
	return m_paiYieldModifier;
}

int CvTraitInfo::getCapitalYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCapitalYieldModifier ? m_paiCapitalYieldModifier[i] : 0;
}

int* CvTraitInfo::getCapitalYieldModifierArray() const
{
	return m_paiCapitalYieldModifier;
}

int CvTraitInfo::getCapitalCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCapitalCommerceModifier ? m_paiCapitalCommerceModifier[i] : 0;
}

int* CvTraitInfo::getCapitalCommerceModifierArray() const
{
	return m_paiCapitalCommerceModifier;
}

int CvTraitInfo::getSpecialistExtraCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiSpecialistExtraCommerce ? m_paiSpecialistExtraCommerce[i] : 0;
}

int* CvTraitInfo::getSpecialistExtraCommerceArray() const
{
	return m_paiSpecialistExtraCommerce;
}

int CvTraitInfo::getUnitCombatFreeExperience(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatFreeExperience ? m_paiUnitCombatFreeExperience[i] : 0;
}

int CvTraitInfo::getUnitCombatProductionModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatProductionModifier ? m_paiUnitCombatProductionModifier[i] : 0;
}

int CvTraitInfo::getUnitCombatUpgradeDiscount(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatUpgradeDiscount ? m_paiUnitCombatUpgradeDiscount[i] : 0;
}

bool CvTraitInfo::isUpgradeAnywhereUnitCombat(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabUpgradeAnywhereUnitCombat ? m_pabUpgradeAnywhereUnitCombat[i] : false;
}

bool CvTraitInfo::isUpgradeAnywhereExcludeEnemy(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabUpgradeAnywhereExcludeEnemy ? m_pabUpgradeAnywhereExcludeEnemy[i] : false;
}

// Builds with an improvement keep the feature on the plots of the player (added by this mod, as FFH2's civilizations)
bool CvTraitInfo::isMaintainFeatures(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabMaintainFeatures ? m_pabMaintainFeatures[i] : false;
}

// The route type the feature counts as in the territory of the player (added by this mod; NO_ROUTE: none)
int CvTraitInfo::getFeatureRouteType(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiFeatureRoute ? m_paiFeatureRoute[i] : NO_ROUTE;
}

// NoObsolescence/bAllBuildings: no building of the player becomes obsolete (added by this mod)
bool CvTraitInfo::isIgnoreBuildingObsolescence() const
{
	return m_bIgnoreBuildingObsolescence;
}

// NoObsolescence/BuildingClasses: the buildings of the building class do not become obsolete for the player (added by this mod)
bool CvTraitInfo::isNoObsoleteBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabNoObsoleteBuildingClass ? m_pabNoObsoleteBuildingClass[i] : false;
}

// The buildings of the special building type (monasteries) do not become obsolete for the player (added by this mod)
// NoObsolescence/bAllBonuses and Bonuses (added by this mod): the bonuses of the team do not become obsolete
bool CvTraitInfo::isIgnoreBonusObsolescence() const
{
	return m_bIgnoreBonusObsolescence;
}

bool CvTraitInfo::isNoObsoleteBonus(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabNoObsoleteBonus ? m_pabNoObsoleteBonus[i] : false;
}

bool CvTraitInfo::isBonusObsolescenceIgnored(int iBonus) const
{
	return (m_bIgnoreBonusObsolescence || isNoObsoleteBonus(iBonus));
}

bool CvTraitInfo::isNoObsoleteSpecialBuilding(int i) const
{
	FAssertMsg(i < GC.getNumSpecialBuildingInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabNoObsoleteSpecialBuilding ? m_pabNoObsoleteSpecialBuilding[i] : false;
}

// Python hooks (added by this mod): the expression evaluated in CvTraitInterface.py, or an empty string
const TCHAR* CvTraitInfo::getPythonHook(TraitPythonHookTypes eHook) const
{
	FAssertMsg(eHook >= 0 && eHook < NUM_TRAIT_PYHOOK_TYPES, "Index out of bounds");
	return m_aszPythonHook[eHook];
}

const TCHAR* CvTraitInfo::getPyOnGameStart() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_GAME_START];
}

const TCHAR* CvTraitInfo::getPyOnBeginTurn() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_BEGIN_TURN];
}

const TCHAR* CvTraitInfo::getPyOnCityFounded() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_CITY_FOUNDED];
}

const TCHAR* CvTraitInfo::getPyOnCityAcquired() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_CITY_ACQUIRED];
}

const TCHAR* CvTraitInfo::getPyOnCorporationFounded() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_CORPORATION_FOUNDED];
}

const TCHAR* CvTraitInfo::getPyOnCityLost() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_CITY_LOST];
}

const TCHAR* CvTraitInfo::getPyOnBuildingBuilt() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_BUILDING_BUILT];
}

const TCHAR* CvTraitInfo::getPyOnReligionFounded() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_RELIGION_FOUNDED];
}

const TCHAR* CvTraitInfo::getPyOnGained() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_GAINED];
}

const TCHAR* CvTraitInfo::getPyOnLost() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_LOST];
}

const TCHAR* CvTraitInfo::getPyOnTechAcquired() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_TECH_ACQUIRED];
}

const TCHAR* CvTraitInfo::getPyOnGreatPersonBorn() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_GREAT_PERSON_BORN];
}

const TCHAR* CvTraitInfo::getPyOnGoldenAgeStart() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_GOLDEN_AGE_START];
}

const TCHAR* CvTraitInfo::getPyOnGoldenAgeEnd() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_GOLDEN_AGE_END];
}

const TCHAR* CvTraitInfo::getPyOnWarDeclared() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_WAR_DECLARED];
}

const TCHAR* CvTraitInfo::getPyOnPeaceMade() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_PEACE_MADE];
}

const TCHAR* CvTraitInfo::getPyOnEraChanged() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_ERA_CHANGED];
}

const TCHAR* CvTraitInfo::getPyOnFirstContact() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_FIRST_CONTACT];
}

const TCHAR* CvTraitInfo::getPyOnProjectBuilt() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_PROJECT_BUILT];
}

const TCHAR* CvTraitInfo::getPyOnVassalGained() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_VASSAL_GAINED];
}

const TCHAR* CvTraitInfo::getPyOnReligionSpread() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_RELIGION_SPREAD];
}

const TCHAR* CvTraitInfo::getPyOnCivicChanged() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_CIVIC_CHANGED];
}

const TCHAR* CvTraitInfo::getPyOnCircumnavigated() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_CIRCUMNAVIGATED];
}

// PyOnOtherCityFounded (added by this mod): another player founded a city (CvPlayer::found)
const TCHAR* CvTraitInfo::getPyOnOtherCityFounded() const
{
	return m_aszPythonHook[TRAIT_PYHOOK_OTHER_CITY_FOUNDED];
}

// ConditionalFreeTraits (added by this mod): the traits the player has while the conditions hold
int CvTraitInfo::getNumConditionalFreeTraits() const
{
	return (int)m_aConditionalFreeTraits.size();
}

int CvTraitInfo::getConditionalFreeTraitType(int i) const
{
	FAssertMsg(i >= 0 && i < getNumConditionalFreeTraits(), "Index out of bounds");
	return m_aConditionalFreeTraits[i].iTrait;
}

const ConditionalFreeTrait& CvTraitInfo::getConditionalFreeTrait(int i) const
{
	FAssertMsg(i >= 0 && i < getNumConditionalFreeTraits(), "Index out of bounds");
	return m_aConditionalFreeTraits[i];
}

int CvTraitInfo::isFreePromotion(int i) const
{
	return m_pabFreePromotion ? m_pabFreePromotion[i] : -1; 
}

int CvTraitInfo::isFreePromotionUnitCombat(int i) const
{
	return m_pabFreePromotionUnitCombat ? m_pabFreePromotionUnitCombat[i] : -1; 
}

int CvTraitInfo::getNumFreePromotionSets() const
{
	return (int)m_aabFreePromotionSetPromotion.size();
}

bool CvTraitInfo::isFreePromotionSetPromotion(int iSet, int iPromotion) const
{
	FAssertMsg(iSet < getNumFreePromotionSets(), "Index out of bounds");
	FAssertMsg(iSet > -1, "Index out of bounds");
	FAssertMsg(iPromotion < GC.getNumPromotionInfos(), "Index out of bounds");
	FAssertMsg(iPromotion > -1, "Index out of bounds");
	if ((iSet < 0) || (iSet >= getNumFreePromotionSets()) || (iPromotion < 0) || (iPromotion >= GC.getNumPromotionInfos()))
	{
		return false;
	}
	return m_aabFreePromotionSetPromotion[iSet][iPromotion];
}

bool CvTraitInfo::isFreePromotionSetUnitCombat(int iSet, int iUnitCombat) const
{
	FAssertMsg(iSet < getNumFreePromotionSets(), "Index out of bounds");
	FAssertMsg(iSet > -1, "Index out of bounds");
	FAssertMsg(iUnitCombat < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	if ((iSet < 0) || (iSet >= getNumFreePromotionSets()) || (iUnitCombat < 0) || (iUnitCombat >= GC.getNumUnitCombatInfos()))
	{
		return false;
	}
	return m_aabFreePromotionSetUnitCombat[iSet][iUnitCombat];
}

// FreePromotionUnitClasses, bFreePromotionNoUnitCombat and FreePromotionDomains of a FreePromotionSet (added by this mod)
bool CvTraitInfo::isFreePromotionSetUnitClass(int iSet, int iUnitClass) const
{
	if (iSet < 0 || iSet >= getNumFreePromotionSets())
	{
		return false;
	}
	return (std::find(m_aaiFreePromotionSetUnitClasses[iSet].begin(), m_aaiFreePromotionSetUnitClasses[iSet].end(), iUnitClass) != m_aaiFreePromotionSetUnitClasses[iSet].end());
}

bool CvTraitInfo::isFreePromotionSetNoUnitCombat(int iSet) const
{
	return (iSet > -1 && iSet < getNumFreePromotionSets()) ? m_abFreePromotionSetNoUnitCombat[iSet] : false;
}

bool CvTraitInfo::isFreePromotionSetDomain(int iSet, int iDomain) const
{
	if (iSet < 0 || iSet >= getNumFreePromotionSets())
	{
		return false;
	}
	return (m_aaiFreePromotionSetDomains[iSet].empty() || std::find(m_aaiFreePromotionSetDomains[iSet].begin(), m_aaiFreePromotionSetDomains[iSet].end(), iDomain) != m_aaiFreePromotionSetDomains[iSet].end());
}

// The set gives its promotions to units of iUnit (added by this mod): its unit combat, its unit class, or any unit without a
// unit combat with bFreePromotionNoUnitCombat; and its domain, when the set has FreePromotionDomains
bool CvTraitInfo::isFreePromotionSetUnit(int iSet, int iUnit) const
{
	if (iSet < 0 || iSet >= getNumFreePromotionSets() || iUnit < 0 || iUnit >= GC.getNumUnitInfos())
	{
		return false;
	}

	CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iUnit);
	bool bTarget = ((kUnit.getUnitCombatType() != NO_UNITCOMBAT && isFreePromotionSetUnitCombat(iSet, kUnit.getUnitCombatType()))
		|| isFreePromotionSetUnitClass(iSet, kUnit.getUnitClassType())
		|| (kUnit.getUnitCombatType() == NO_UNITCOMBAT && isFreePromotionSetNoUnitCombat(iSet)));

	return (bTarget && isFreePromotionSetDomain(iSet, kUnit.getDomainType()));
}

bool CvTraitInfo::isUnitFreePromotion(int iUnit, int iPromotion) const
{
	for (int iSet = 0; iSet < getNumFreePromotionSets(); iSet++)
	{
		if (isFreePromotionSetPromotion(iSet, iPromotion) && isFreePromotionSetUnit(iSet, iUnit))
		{
			return true;
		}
	}
	return false;
}

// True if any FreePromotionSet gives iPromotion to units of iUnitCombat
bool CvTraitInfo::isUnitCombatFreePromotion(int iUnitCombat, int iPromotion) const
{
	for (int iSet = 0; iSet < getNumFreePromotionSets(); iSet++)
	{
		if (isFreePromotionSetUnitCombat(iSet, iUnitCombat) && isFreePromotionSetPromotion(iSet, iPromotion))
		{
			return true;
		}
	}
	return false;
}

int CvTraitInfo::getTerrainYieldChange(int iTerrain, int iYield) const
{
	FAssertMsg(iTerrain < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(iTerrain > -1, "Index out of bounds");
	FAssertMsg(iYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(iYield > -1, "Index out of bounds");
	if ((iTerrain < 0) || (iTerrain >= (int)m_aaiTerrainYieldChange.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiTerrainYieldChange[iTerrain][iYield];
}

// TerrainYieldChangesPerEra (added by this mod): times the current era of the player (0 in the ancient era)
int CvTraitInfo::getTerrainYieldChangePerEra(int iTerrain, int iYield) const
{
	if ((iTerrain < 0) || (iTerrain >= (int)m_aaiTerrainYieldChangePerEra.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiTerrainYieldChangePerEra[iTerrain][iYield];
}

int CvTraitInfo::getFeatureYieldChange(int iFeature, int iYield) const
{
	FAssertMsg(iFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	FAssertMsg(iYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(iYield > -1, "Index out of bounds");
	if ((iFeature < 0) || (iFeature >= (int)m_aaiFeatureYieldChange.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiFeatureYieldChange[iFeature][iYield];
}

int CvTraitInfo::getFeatureProductionChange(int iFeature) const
{
	FAssertMsg(iFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	return ((iFeature > -1) && (iFeature < (int)m_aiFeatureProductionChange.size())) ? m_aiFeatureProductionChange[iFeature] : 0;
}

int CvTraitInfo::getFeatureGrowthChange(int iFeature) const
{
	FAssertMsg(iFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	return ((iFeature > -1) && (iFeature < (int)m_aiFeatureGrowthChange.size())) ? m_aiFeatureGrowthChange[iFeature] : 0;
}

int CvTraitInfo::getFeatureHealthChange(int iFeature) const
{
	FAssertMsg(iFeature < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	return ((iFeature > -1) && (iFeature < (int)m_aiFeatureHealthChange.size())) ? m_aiFeatureHealthChange[iFeature] : 0;
}

int CvTraitInfo::getSpecialistYieldChange(int iSpecialist, int iYield) const
{
	if ((iSpecialist < 0) || (iSpecialist >= (int)m_aaiSpecialistYieldChange.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiSpecialistYieldChange[iSpecialist][iYield];
}

int CvTraitInfo::getSpecialistCommerceChange(int iSpecialist, int iCommerce) const
{
	if ((iSpecialist < 0) || (iSpecialist >= (int)m_aaiSpecialistCommerceChange.size()) || (iCommerce < 0) || (iCommerce >= NUM_COMMERCE_TYPES))
	{
		return 0;
	}
	return m_aaiSpecialistCommerceChange[iSpecialist][iCommerce];
}

// The yields of one specialist type, or NULL when the trait has none (for the help text)
int* CvTraitInfo::getSpecialistYieldChangeArray(int iSpecialist) const
{
	if ((iSpecialist < 0) || (iSpecialist >= (int)m_aaiSpecialistYieldChange.size()))
	{
		return NULL;
	}
	return const_cast<int*>(&m_aaiSpecialistYieldChange[iSpecialist][0]);
}

int* CvTraitInfo::getSpecialistCommerceChangeArray(int iSpecialist) const
{
	if ((iSpecialist < 0) || (iSpecialist >= (int)m_aaiSpecialistCommerceChange.size()))
	{
		return NULL;
	}
	return const_cast<int*>(&m_aaiSpecialistCommerceChange[iSpecialist][0]);
}

int CvTraitInfo::getImprovementYieldChanges(int iImprovement, int iYield) const
{
	FAssertMsg(iImprovement < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(iImprovement > -1, "Index out of bounds");
	FAssertMsg(iYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(iYield > -1, "Index out of bounds");
	if ((iImprovement < 0) || (iImprovement >= (int)m_aaiImprovementYieldChange.size()) || (iYield < 0) || (iYield >= NUM_YIELD_TYPES))
	{
		return 0;
	}
	return m_aaiImprovementYieldChange[iImprovement][iYield];
}

int CvTraitInfo::getRiverPlotYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiRiverPlotYieldChange ? m_paiRiverPlotYieldChange[i] : 0;
}

int* CvTraitInfo::getRiverPlotYieldChangeArray() const
{
	return m_paiRiverPlotYieldChange;
}

// RiverEdgeYieldPercents (added by this mod): % of a yield for each edge of a plot with a river along it (0 to 4 edges). The
// sum is rounded down: 100 gives +1 for each edge, 34 gives +1 for 3 edges (CvPlot::calculateTraitYieldChange)
int CvTraitInfo::getRiverEdgeYieldPercent(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiRiverEdgeYieldPercent ? m_paiRiverEdgeYieldPercent[i] : 0;
}

// Yields of the lake plots in the territory of the player (added by this mod, from the Touhou Epic mod)
int CvTraitInfo::getLakePlotYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiLakePlotYieldChange ? m_paiLakePlotYieldChange[i] : 0;
}

int* CvTraitInfo::getLakePlotYieldChangeArray() const
{
	return m_paiLakePlotYieldChange;
}

// UniqueUnits and UniqueBuildings: for the players with this trait, instead of what the civilization has (added by this mod).
// See CvPlayer::getCivilizationUnit()
int CvTraitInfo::getNumUniqueUnits() const
{
	return (int)m_aiUniqueUnits.size();
}

int CvTraitInfo::getUniqueUnitClass(int i) const
{
	return m_aiUniqueUnitClasses[i];
}

int CvTraitInfo::getUniqueUnit(int i) const
{
	return m_aiUniqueUnits[i];
}

int CvTraitInfo::getUniqueUnitForClass(int iUnitClass) const
{
	int iI;

	for (iI = 0; iI < (int)m_aiUniqueUnits.size(); iI++)
	{
		if (m_aiUniqueUnitClasses[iI] == iUnitClass)
		{
			return m_aiUniqueUnits[iI];
		}
	}

	return NO_UNIT;
}

int CvTraitInfo::getNumUniqueBuildings() const
{
	return (int)m_aiUniqueBuildings.size();
}

int CvTraitInfo::getUniqueBuildingClass(int i) const
{
	return m_aiUniqueBuildingClasses[i];
}

int CvTraitInfo::getUniqueBuilding(int i) const
{
	return m_aiUniqueBuildings[i];
}

int CvTraitInfo::getUniqueBuildingForClass(int iBuildingClass) const
{
	int iI;

	for (iI = 0; iI < (int)m_aiUniqueBuildings.size(); iI++)
	{
		if (m_aiUniqueBuildingClasses[iI] == iBuildingClass)
		{
			return m_aiUniqueBuildings[iI];
		}
	}

	return NO_BUILDING;
}

int CvTraitInfo::getAngryYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiAngryYieldChange ? m_paiAngryYieldChange[i] : 0;
}

int* CvTraitInfo::getAngryYieldChangeArray() const
{
	return m_paiAngryYieldChange;
}

int CvTraitInfo::getAngryCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiAngryCommerceChange ? m_paiAngryCommerceChange[i] : 0;
}

int* CvTraitInfo::getAngryCommerceChangeArray() const
{
	return m_paiAngryCommerceChange;
}

int CvTraitInfo::getUnhealthyYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnhealthyYieldChange ? m_paiUnhealthyYieldChange[i] : 0;
}

int* CvTraitInfo::getUnhealthyYieldChangeArray() const
{
	return m_paiUnhealthyYieldChange;
}

int CvTraitInfo::getUnhealthyCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnhealthyCommerceChange ? m_paiUnhealthyCommerceChange[i] : 0;
}

int* CvTraitInfo::getUnhealthyCommerceChangeArray() const
{
	return m_paiUnhealthyCommerceChange;
}

// Turns the types from readTypeList() into a flag per info (iNumInfos), or NULL when nothing is set. NONE is skipped,
// and an unknown type (or a type of another info class) shows the same message as FindInInfoClass.
static void resolveTypeList(const TCHAR* szListTag, const std::vector<CvString>& aszTypes, int iNumInfos, bool** ppabFlags)
{
	char szMessage[1024];
	int iIndex;
	int i, j;

	SAFE_DELETE_ARRAY(*ppabFlags);

	for (i = 0; i < (int)aszTypes.size(); i++)
	{
		if (aszTypes[i].empty() || aszTypes[i] == "NONE")
		{
			continue;
		}

		iIndex = GC.getInfoTypeForString(aszTypes[i].GetCString(), true);
		if (iIndex < 0 || iIndex >= iNumInfos)
		{
			sprintf(szMessage, "Tag: %s in %s of CIV4TraitInfos.xml was incorrect", aszTypes[i].GetCString(), szListTag);
			gDLL->MessageBox(szMessage, "XML Error");
			continue;
		}

		if (*ppabFlags == NULL)
		{
			*ppabFlags = new bool[iNumInfos];
			for (j = 0; j < iNumInfos; j++)
			{
				(*ppabFlags)[j] = false;
			}
		}
		(*ppabFlags)[iIndex] = true;
	}
}

// UniqueUnits and UniqueBuildings of the traits: pairs of a class and what the trait has for it (added by this mod)
static void readUniqueTypes(CvXMLLoadUtility* pXML, const TCHAR* szListTag, const TCHAR* szItemTag, const TCHAR* szClassTag, const TCHAR* szTypeTag, std::vector<CvString>& aszClasses, std::vector<CvString>& aszTypes)
{
	CvString szClass;
	CvString szType;

	aszClasses.clear();
	aszTypes.clear();

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), szListTag))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), szItemTag))
		{
			do
			{
				pXML->GetChildXmlValByName(szClass, szClassTag);
				pXML->GetChildXmlValByName(szType, szTypeTag);
				aszClasses.push_back(szClass);
				aszTypes.push_back(szType);
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), const_cast<TCHAR*>(szItemTag)));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for the simple values of the mod (iWorkerSpeedModifier ... bBarbarianAlly) (added by this mod)
void CvTraitInfo::readModValues(CvXMLLoadUtility* pXML)
{
	pXML->GetChildXmlValByName(&m_iWorkerSpeedModifier, "iWorkerSpeedModifier");
	pXML->GetChildXmlValByName(&m_iFreeExperience, "iFreeExperience");
	pXML->GetChildXmlValByName(&m_iMilitaryProductionModifier, "iMilitaryProductionModifier");
	pXML->GetChildXmlValByName(&m_iImprovementUpgradeRateModifier, "iImprovementUpgradeRateModifier");
	pXML->GetChildXmlValByName(&m_iDistanceMaintenanceModifier, "iDistanceMaintenanceModifier");
	pXML->GetChildXmlValByName(&m_iNumCitiesMaintenanceModifier, "iNumCitiesMaintenanceModifier");
	pXML->GetChildXmlValByName(&m_iWarWearinessModifier, "iWarWearinessModifier");
	pXML->GetChildXmlValByName(&m_iFreeSpecialist, "iFreeSpecialist");
	pXML->GetChildXmlValByName(&m_iTradeRoutes, "iTradeRoutes");
	pXML->GetChildXmlValByName(&m_iExpInBorderModifier, "iExpInBorderModifier");
	pXML->GetChildXmlValByName(&m_iCityRadiusChange, "iCityRadiusChange");
	// The culture levels are loaded after the traits: readPass3() finds the type
	pXML->GetChildXmlValByName(m_szCityRadiusCultureLevel, "CityRadiusCultureLevel");
	pXML->GetChildXmlValByName(&m_iMaxCities, "iMaxCities");
	pXML->GetChildXmlValByName(&m_iMaxCitiesPercent, "iMaxCitiesPercent");
	pXML->GetChildXmlValByName(&m_iSummonDuration, "iSummonDuration");
	pXML->GetChildXmlValByName(&m_iGoldenAgeModifier, "iGoldenAgeModifier");
	pXML->GetChildXmlValByName(&m_iGoldenAgeGreatPeopleRateModifier, "iGoldenAgeGreatPeopleRateModifier");
	pXML->GetChildXmlValByName(&m_iCapitalBuildingProductionModifier, "iCapitalBuildingProductionModifier");
	pXML->GetChildXmlValByName(&m_iPillageGoldModifier, "iPillageGoldModifier");
	pXML->GetChildXmlValByName(&m_iUnitCostModifier, "iUnitCostModifier");
	pXML->GetChildXmlValByName(&m_iBuildingCostModifier, "iBuildingCostModifier");
	pXML->GetChildXmlValByName(&m_bNoPillageInBorders, "bNoPillageInBorders");
	pXML->GetChildXmlValByName(&m_iMinCityRangeChange, "iMinCityRangeChange");
	pXML->GetChildXmlValByName(&m_bFoundInForeignTerritory, "bFoundInForeignTerritory");
	pXML->GetChildXmlValByName(&m_bNoGlobalWarmingInBorders, "bNoGlobalWarmingInBorders");
	pXML->GetChildXmlValByName(&m_iReligionSpreadChange, "iReligionSpreadChange");
	pXML->GetChildXmlValByName(&m_iCorporationSpreadChange, "iCorporationSpreadChange");
	pXML->GetChildXmlValByName(&m_iGoldenAgeLandMovesChange, "iGoldenAgeLandMovesChange");
	pXML->GetChildXmlValByName(&m_iGoldenAgeSeaMovesChange, "iGoldenAgeSeaMovesChange");
	pXML->GetChildXmlValByName(&m_bInlandCanals, "bInlandCanals");
	pXML->GetChildXmlValByName(&m_bBarbarianAlly, "bBarbarianAlly");
	pXML->GetChildXmlValByName(&m_bBarbarianVision, "bBarbarianVision");
	// The terrains and features are loaded before the traits
	readTypeList(pXML, "PassableTerrainsInBorders", m_aiPassableTerrainsInBorders);
	readTypeList(pXML, "PassableFeaturesInBorders", m_aiPassableFeaturesInBorders);
	readTypeValueList(pXML, "FeatureHealChanges", "FeatureHealChange", "FeatureType", "iHealChange", GC.getNumFeatureInfos(), m_aiFeatureHealChange);
}

// The part of read() for CityFoundedUnits (added by this mod)
void CvTraitInfo::readCityFoundedUnits(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	// CityFoundedUnits (added by this mod): the unit classes are loaded before the traits
	m_aiCityFoundedUnitClasses.clear();
	m_aiCityFoundedUnitNums.clear();
	m_abCityFoundedUnitCoastalOnly.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "CityFoundedUnits"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "CityFoundedUnit"))
		{
			do
			{
				int iFoundedUnitClass;
				int iFoundedUnitNum = 1;
				bool bFoundedUnitCoastalOnly = false;

				pXML->GetChildXmlValByName(szTextVal, "UnitClassType");
				iFoundedUnitClass = pXML->FindInInfoClass(szTextVal);
				pXML->GetChildXmlValByName(&iFoundedUnitNum, "iNumUnits", 1);
				pXML->GetChildXmlValByName(&bFoundedUnitCoastalOnly, "bCoastalOnly");

				if (iFoundedUnitClass > -1 && iFoundedUnitNum > 0)
				{
					m_aiCityFoundedUnitClasses.push_back(iFoundedUnitClass);
					m_aiCityFoundedUnitNums.push_back(iFoundedUnitNum);
					m_abCityFoundedUnitCoastalOnly.push_back(bFoundedUnitCoastalOnly);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "CityFoundedUnit"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for SpecialBuildingProductionModifiers (added by this mod): the special buildings are loaded after the
// traits, so the types are kept as strings and resolved in readPass3
void CvTraitInfo::readSpecialBuildingProductionModifiers(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	int iValue;

	m_aszSpecialBuildingProductionTypes.clear();
	m_aiSpecialBuildingProductionValues.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "SpecialBuildingProductionModifiers"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "SpecialBuildingProductionModifier"))
		{
			do
			{
				iValue = 0;
				pXML->GetChildXmlValByName(szTextVal, "SpecialBuildingType");
				pXML->GetChildXmlValByName(&iValue, "iProductionModifier", 0);
				if (!szTextVal.empty() && szTextVal != "NONE" && iValue != 0)
				{
					m_aszSpecialBuildingProductionTypes.push_back(szTextVal);
					m_aiSpecialBuildingProductionValues.push_back(iValue);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "SpecialBuildingProductionModifier"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for CivicOptionExtraSlots (added by this mod): the civic options are loaded after the traits, so the
// types are kept as strings and resolved in readPass3
void CvTraitInfo::readCivicOptionExtraSlots(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	int iValue;

	m_aszCivicOptionExtraSlotTypes.clear();
	m_aiCivicOptionExtraSlotValues.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "CivicOptionExtraSlots"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "CivicOptionExtraSlot"))
		{
			do
			{
				iValue = 0;
				pXML->GetChildXmlValByName(szTextVal, "CivicOptionType");
				pXML->GetChildXmlValByName(&iValue, "iExtraSlots", 0);
				if (!szTextVal.empty() && szTextVal != "NONE" && iValue != 0)
				{
					m_aszCivicOptionExtraSlotTypes.push_back(szTextVal);
					m_aiCivicOptionExtraSlotValues.push_back(iValue);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "CivicOptionExtraSlot"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for ExplorationRewards (added by this mod). The commerce infos are loaded after the traits, so the
// names are matched here (CommerceTypes is a fixed enum, in the order of CIV4CommerceInfo.xml)
void CvTraitInfo::readExplorationRewards(CvXMLLoadUtility* pXML)
{
	static const char* aszExplorePlotNames[NUM_EXPLORE_PLOT_TYPES] =
	{
		"EXPLORE_PLOT_WATER",
		"EXPLORE_PLOT_HOME",
		"EXPLORE_PLOT_OVERSEAS_OLD_WORLD",
		"EXPLORE_PLOT_OVERSEAS_NEW_WORLD",
	};
	static const char* aszCommerceNames[NUM_COMMERCE_TYPES] =
	{
		"COMMERCE_GOLD",
		"COMMERCE_RESEARCH",
		"COMMERCE_CULTURE",
		"COMMERCE_ESPIONAGE",
	};
	CvString szTextVal;
	int iPlotType;
	int iCommerce;
	int iPercent;
	int iI;

	m_aiExplorationRewardPlotType.clear();
	m_aiExplorationRewardCommerceType.clear();
	m_aiExplorationRewardPercent.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "ExplorationRewards"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "ExplorationReward"))
		{
			do
			{
				iPlotType = NO_EXPLORE_PLOT;
				pXML->GetChildXmlValByName(szTextVal, "ExplorePlotType");
				for (iI = 0; iI < NUM_EXPLORE_PLOT_TYPES; iI++)
				{
					if (szTextVal == aszExplorePlotNames[iI])
					{
						iPlotType = iI;
					}
				}
				FAssertMsg(iPlotType != NO_EXPLORE_PLOT, CvString::format("ExplorationRewards: unknown ExplorePlotType %s", szTextVal.c_str()).c_str());

				pXML->GetChildXmlValByName(szTextVal, "CommerceType");
				iCommerce = NO_COMMERCE;
				for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
				{
					if (szTextVal == aszCommerceNames[iI])
					{
						iCommerce = iI;
					}
				}
				FAssertMsg(iCommerce != NO_COMMERCE, CvString::format("ExplorationRewards: unknown CommerceType %s", szTextVal.c_str()).c_str());

				iPercent = 0;
				pXML->GetChildXmlValByName(&iPercent, "iPercent", 0);

				// Negative rewards are not supported (the remainder carried to the next turn is never negative)
				if (iPlotType != NO_EXPLORE_PLOT && iCommerce >= 0 && iCommerce < NUM_COMMERCE_TYPES && iPercent > 0)
				{
					m_aiExplorationRewardPlotType.push_back(iPlotType);
					m_aiExplorationRewardCommerceType.push_back(iCommerce);
					m_aiExplorationRewardPercent.push_back(iPercent);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "ExplorationReward"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for the start bias (added by this mod)
void CvTraitInfo::readStartBias(CvXMLLoadUtility* pXML)
{
	// StartBias (added by this mod): iCoastal, iRiver, Terrains, Features, BonusBiases, SeaStart.
	// The terrains and features are loaded before the traits, the bonuses after them
	m_aszStartBiasBonusTypes.clear();
	m_aiStartBiasBonusValues.clear();
	m_bSeaStart = false;
	m_iSeaStartUnitClass = NO_UNITCLASS;
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "StartBias"))
	{
		pXML->GetChildXmlValByName(&m_iStartBiasCoastal, "iCoastal");
		pXML->GetChildXmlValByName(&m_iStartBiasRiver, "iRiver");
		pXML->SetVariableListTagPair(&m_piStartBiasTerrains, "Terrains", sizeof(GC.getTerrainInfo((TerrainTypes)0)), GC.getNumTerrainInfos());
		pXML->SetVariableListTagPair(&m_piStartBiasFeatures, "Features", sizeof(GC.getFeatureInfo((FeatureTypes)0)), GC.getNumFeatureInfos());
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "BonusBiases"))
		{
			if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "StartBiasBonus"))
			{
				do
				{
					CvString szBiasBonus;
					int iBiasBonus = 0;
					pXML->GetChildXmlValByName(szBiasBonus, "BonusType");
					pXML->GetChildXmlValByName(&iBiasBonus, "iStartBias");
					m_aszStartBiasBonusTypes.push_back(szBiasBonus);
					m_aiStartBiasBonusValues.push_back(iBiasBonus);
				}
				while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "StartBiasBonus"));

				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		// SeaStart: ShipUnitClass (the unit classes are loaded before the traits). NONE: no ships
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "SeaStart"))
		{
			CvString szShipClass;
			m_bSeaStart = true;
			pXML->GetChildXmlValByName(szShipClass, "ShipUnitClass");
			m_iSeaStartUnitClass = pXML->FindInInfoClass(szShipClass);
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	// FoundingVassal: the types stay strings until readPass3(). iRange: 5 when not set
	m_bFoundingVassal = false;
	m_iFoundingVassalRange = 0;
	m_szFoundingVassalCivilization.clear();
	m_szFoundingVassalLeader.clear();
	m_szFoundingVassalHandicap.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "FoundingVassal"))
	{
		m_bFoundingVassal = true;
		pXML->GetChildXmlValByName(m_szFoundingVassalCivilization, "CivilizationType");
		pXML->GetChildXmlValByName(m_szFoundingVassalLeader, "LeaderType");
		pXML->GetChildXmlValByName(m_szFoundingVassalHandicap, "HandicapType");
		pXML->GetChildXmlValByName(&m_iFoundingVassalRange, "iRange", 5);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for gold interest, peace anger, culture range, ForbiddenCivics and the new cities (added by this mod)
void CvTraitInfo::readCityAndEconomyValues(CvXMLLoadUtility* pXML)
{
	// GoldInterest: iPercent, iMax (0 = no limit)
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "GoldInterest"))
	{
		pXML->GetChildXmlValByName(&m_iGoldInterestPercent, "iPercent");
		pXML->GetChildXmlValByName(&m_iGoldInterestMax, "iMax");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	// PeaceAnger: iPerTurn, iDecayPerTurn, iMax
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "PeaceAnger"))
	{
		pXML->GetChildXmlValByName(&m_iPeaceAngerPerTurn, "iPerTurn");
		pXML->GetChildXmlValByName(&m_iPeaceAngerDecayPerTurn, "iDecayPerTurn");
		pXML->GetChildXmlValByName(&m_iPeaceAngerMax, "iMax");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_iCultureRangeChange, "iCultureRangeChange");
	// Civics are loaded after the traits: readPass3() finds the types
	readTypeList(pXML, "ForbiddenCivics", m_aszForbiddenCivicTypes);
	// The culture levels are loaded after the traits: readPass3() finds the type
	pXML->GetChildXmlValByName(m_szNewCityCultureLevel, "NewCityCultureLevel");
	pXML->GetChildXmlValByName(&m_iNewCityPopulation, "iNewCityPopulation");
	pXML->GetChildXmlValByName(&m_bNewCityStateReligion, "bNewCityStateReligion");
	pXML->GetChildXmlValByName(&m_iOccupationTimeModifier, "iOccupationTimeModifier");
	// Conquest: iCulturePercent, iBuildingSurviveModifier, bNoPopulationLoss, bNoAutoRaze, bLearnUniqueUnits, bLearnUniqueBuildings,
	// bHeal, bStateReligion, ReligionBuildings (special buildings: readPass3()), iGoldModifier
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Conquest"))
	{
		pXML->GetChildXmlValByName(&m_iConquestCulturePercent, "iCulturePercent");
		pXML->GetChildXmlValByName(&m_iConquestBuildingSurviveModifier, "iBuildingSurviveModifier");
		pXML->GetChildXmlValByName(&m_bNoConquestPopulationLoss, "bNoPopulationLoss");
		pXML->GetChildXmlValByName(&m_bNoAutoRaze, "bNoAutoRaze");
		pXML->GetChildXmlValByName(&m_bConquestLearnUniqueUnits, "bLearnUniqueUnits");
		pXML->GetChildXmlValByName(&m_bConquestLearnUniqueBuildings, "bLearnUniqueBuildings");
		pXML->GetChildXmlValByName(&m_bConquestHeal, "bHeal");
		pXML->GetChildXmlValByName(&m_bConquestStateReligion, "bStateReligion");
		readTypeList(pXML, "ReligionBuildings", m_aszConquestReligionBuildingTypes);
		pXML->GetChildXmlValByName(&m_iCityCaptureGoldModifier, "iGoldModifier");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_iStateReligionHappiness, "iStateReligionHappiness");
	pXML->GetChildXmlValByName(&m_iStateReligionHealth, "iStateReligionHealth");
	pXML->GetChildXmlValByName(&m_iStateReligionGreatPeopleRateModifier, "iStateReligionGreatPeopleRateModifier");
	pXML->GetChildXmlValByName(&m_iStateReligionUnitProductionModifier, "iStateReligionUnitProductionModifier");
	pXML->GetChildXmlValByName(&m_iStateReligionBuildingProductionModifier, "iStateReligionBuildingProductionModifier");
	pXML->GetChildXmlValByName(&m_iStateReligionFreeExperience, "iStateReligionFreeExperience");
	pXML->GetChildXmlValByName(&m_iEnemyMoveCostInBorders, "iEnemyMoveCostInBorders");
	pXML->GetChildXmlValByName(&m_iRevengeCombatPercent, "iRevengeCombatPercent");
	// KnownTechCost: iModifier, iLimit (0 = no limit)
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "KnownTechCost"))
	{
		pXML->GetChildXmlValByName(&m_iKnownTechCostModifier, "iModifier");
		pXML->GetChildXmlValByName(&m_iKnownTechCostLimit, "iLimit");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_bStackCityDefense, "bStackCityDefense");
	pXML->GetChildXmlValByName(&m_bRevealWonderProduction, "bRevealWonderProduction");
	pXML->GetChildXmlValByName(&m_bRevealWarPlans, "bRevealWarPlans");
	pXML->GetChildXmlValByName(&m_bRevealHiddenAttitude, "bRevealHiddenAttitude");
	pXML->GetChildXmlValByName(&m_bLongCountCalendar, "bLongCountCalendar");
	pXML->GetChildXmlValByName(&m_bImprovementUpgradeUnworked, "bImprovementUpgradeUnworked");
	pXML->GetChildXmlValByName(&m_bUnsettleGreatPeople, "bUnsettleGreatPeople");
	pXML->GetChildXmlValByName(&m_bIgnoreEvents, "bIgnoreEvents");
	pXML->GetChildXmlValByName(&m_iEventChanceChange, "iEventChanceChange");
	pXML->GetChildXmlValByName(&m_bNoFirstEventDelay, "bNoFirstEventDelay");
	// CultureFlip: bNoFlip, bNoRevoltAndNoFlip, bInstantFlip, iRevoltChance
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "CultureFlip"))
	{
		pXML->GetChildXmlValByName(&m_bNoCultureFlip, "bNoFlip");
		pXML->GetChildXmlValByName(&m_bNoRevoltAndNoCultureFlip, "bNoRevoltAndNoFlip");
		pXML->GetChildXmlValByName(&m_bInstantCultureFlip, "bInstantFlip");
		pXML->GetChildXmlValByName(&m_iCultureRevoltChance, "iRevoltChance");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// GreatPeopleClassRateModifiers: the unit classes are loaded before the traits
	readTypeValueList(pXML, "GreatPeopleClassRateModifiers", "GreatPeopleClassRateModifier", "UnitClassType", "iGreatPeopleRateModifier", GC.getNumUnitClassInfos(), m_aiGreatPeopleClassRateModifier);
	readSpecialBuildingProductionModifiers(pXML);
	pXML->GetChildXmlValByName(&m_iNewWorldCityPopulation, "iNewWorldCityPopulation");
	pXML->GetChildXmlValByName(&m_iOverseasCityPopulation, "iOverseasCityPopulation");
	pXML->GetChildXmlValByName(&m_iOverseasMaintenanceModifier, "iOverseasMaintenanceModifier");
	pXML->GetChildXmlValByName(&m_iDistantTradeRouteModifier, "iDistantTradeRouteModifier");
	// EnemyAttrition: iPercent, iLimit (100 when left out)
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "EnemyAttrition"))
	{
		pXML->GetChildXmlValByName(&m_iEnemyAttritionInBorders, "iPercent");
		pXML->GetChildXmlValByName(&m_iEnemyAttritionLimit, "iLimit", 100);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_iAnarchyGoldenAgePercent, "iAnarchyGoldenAgePercent");
	pXML->GetChildXmlValByName(&m_iTechCostModifier, "iTechCostModifier");
	pXML->GetChildXmlValByName(&m_iAttitudeChange, "iAttitudeChange");
	// Deals: bBuyCities, iCityValueModifier, iGoldLimitModifier, iGoldValueModifier
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Deals"))
	{
		pXML->GetChildXmlValByName(&m_bBuyCities, "bBuyCities");
		pXML->GetChildXmlValByName(&m_iDealCityValueModifier, "iCityValueModifier");
		pXML->GetChildXmlValByName(&m_iDealGoldLimitModifier, "iGoldLimitModifier");
		pXML->GetChildXmlValByName(&m_iDealGoldValueModifier, "iGoldValueModifier");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_iRouteMovementModifierInBorders, "iRouteMovementModifierInBorders");
	pXML->GetChildXmlValByName(&m_iCombatPercent, "iCombatPercent");
	pXML->GetChildXmlValByName(&m_iHappinessPerReligion, "iHappinessPerReligion");
	readCommerceChanges(pXML, "CommercePerReligion", &m_paiCommercePerReligion);
	pXML->GetChildXmlValByName(&m_iDefenseCombatPercent, "iDefenseCombatPercent");
	readCommerceChanges(pXML, "WorldWonderCommerceChanges", &m_paiWorldWonderCommerceChange);
	readCommerceChanges(pXML, "GlobalWorldWonderCommerceChanges", &m_paiGlobalWorldWonderCommerceChange);
	pXML->GetChildXmlValByName(&m_iEnemyWarWearinessModifier, "iEnemyWarWearinessModifier");
	readTypeList(pXML, "NoTechCivics", m_aszNoTechCivicTypes);	// civics are loaded later: readPass3()
	readCivicOptionExtraSlots(pXML);
	readExplorationRewards(pXML);
	pXML->GetChildXmlValByName(&m_iMaxNationalWondersPerCity, "iMaxNationalWondersPerCity");
	pXML->GetChildXmlValByName(&m_iMaxNationalWondersPerCityForOCC, "iMaxNationalWondersPerCityForOCC");
	pXML->GetChildXmlValByName(&m_bHumanCanBeVassal, "bHumanCanBeVassal");
	pXML->GetChildXmlValByName(&m_bBasicTrait, "bBasicTrait");
	pXML->GetChildXmlValByName(&m_bGainTraitOnCapitalCapture, "bGainTraitOnCapitalCapture");
	pXML->GetChildXmlValByName(&m_bGainTraitsOnElimination, "bGainTraitsOnElimination");
	// RandomBasicTraits: iTurns and iCount; on when the element is there
	m_bRandomBasicTraits = false;
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "RandomBasicTraits"))
	{
		m_bRandomBasicTraits = true;
		pXML->GetChildXmlValByName(&m_iRandomBasicTraitTurns, "iTurns");
		pXML->GetChildXmlValByName(&m_iRandomBasicTraitCount, "iCount");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// GainTraitsOnFriendly: bAll; on when the element is there
	m_bGainTraitsOnFriendly = false;
	m_bGainTraitsOnFriendlyAll = false;
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "GainTraitsOnFriendly"))
	{
		m_bGainTraitsOnFriendly = true;
		pXML->GetChildXmlValByName(&m_bGainTraitsOnFriendlyAll, "bAll");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for the yield and commerce arrays, the unit combat arrays, the free promotions and MaintainFeatures (added by this mod)
void CvTraitInfo::readYieldAndUnitArrays(CvXMLLoadUtility* pXML)
{
	readYieldChanges(pXML, "ExtraYieldThresholds", &m_paiExtraYieldThreshold);
	readYieldChanges(pXML, "TradeYieldModifiers", &m_paiTradeYieldModifier);
	readCommerceChanges(pXML, "CommerceChanges", &m_paiCommerceChange);
	readCommerceChanges(pXML, "CommerceModifiers", &m_paiCommerceModifier);
	readYieldChanges(pXML, "YieldModifiers", &m_paiYieldModifier);
	readYieldChanges(pXML, "CapitalYieldModifiers", &m_paiCapitalYieldModifier);
	readCommerceChanges(pXML, "CapitalCommerceModifiers", &m_paiCapitalCommerceModifier);
	readCommerceChanges(pXML, "SpecialistExtraCommerces", &m_paiSpecialistExtraCommerce);

	pXML->SetVariableListTagPair(&m_paiUnitCombatFreeExperience, "UnitCombatFreeExperiences", sizeof(GC.getUnitCombatInfo((UnitCombatTypes)0)), GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_paiUnitCombatProductionModifier, "UnitCombatProductionModifiers", sizeof(GC.getUnitCombatInfo((UnitCombatTypes)0)), GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_paiUnitCombatUpgradeDiscount, "UnitCombatUpgradeDiscounts", sizeof(GC.getUnitCombatInfo((UnitCombatTypes)0)), GC.getNumUnitCombatInfos());
	readUpgradeAnywhereUnitCombats(pXML, &m_pabUpgradeAnywhereUnitCombat, &m_pabUpgradeAnywhereExcludeEnemy);

	pXML->SetVariableListTagPair(&m_pabFreePromotion, "FreePromotions", sizeof(GC.getPromotionInfo((PromotionTypes)0)), GC.getNumPromotionInfos());

	pXML->SetVariableListTagPair(&m_pabFreePromotionUnitCombat, "FreePromotionUnitCombats", sizeof(GC.getUnitCombatInfo((UnitCombatTypes)0)), GC.getNumUnitCombatInfos());

	pXML->SetVariableListTagPair(&m_pabMaintainFeatures, "MaintainFeatures", sizeof(GC.getFeatureInfo((FeatureTypes)0)), GC.getNumFeatureInfos());
}

// The part of read() for FeatureRoutes (added by this mod)
void CvTraitInfo::readFeatureRoutes(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	// FeatureRoutes: <FeatureRoute> with a FeatureType and a RouteType. The route types are loaded after the traits, so
	// readPass3() finds them
	m_aiFeatureRouteFeatures.clear();
	m_aszFeatureRouteTypes.clear();
	readUniqueTypes(pXML, "UniqueUnits", "UniqueUnit", "UnitClassType", "UnitType", m_aszUniqueUnitClassTypes, m_aszUniqueUnitTypes);
	readUniqueTypes(pXML, "UniqueBuildings", "UniqueBuilding", "BuildingClassType", "BuildingType", m_aszUniqueBuildingClassTypes, m_aszUniqueBuildingTypes);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "FeatureRoutes"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "FeatureRoute"))
		{
			do
			{
				pXML->GetChildXmlValByName(szTextVal, "FeatureType");
				int iFeature = pXML->FindInInfoClass(szTextVal);
				pXML->GetChildXmlValByName(szTextVal, "RouteType");
				if (iFeature > -1)
				{
					m_aiFeatureRouteFeatures.push_back(iFeature);
					m_aszFeatureRouteTypes.push_back(szTextVal);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "FeatureRoute"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for the obsolescence lists and the Python hooks (added by this mod)
void CvTraitInfo::readObsolescenceAndHooks(CvXMLLoadUtility* pXML)
{
	// NoObsolescence: bAllBuildings, BuildingClasses, SpecialBuildings, bAllBonuses, Bonuses.
	// Building classes and bonuses are loaded after the traits, so readPass3() finds the types
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "NoObsolescence"))
	{
		pXML->GetChildXmlValByName(&m_bIgnoreBuildingObsolescence, "bAllBuildings");
		readTypeList(pXML, "BuildingClasses", m_aszNoObsoleteBuildingClassTypes);
		readTypeList(pXML, "SpecialBuildings", m_aszNoObsoleteSpecialBuildingTypes);
		pXML->GetChildXmlValByName(&m_bIgnoreBonusObsolescence, "bAllBonuses");
		readTypeList(pXML, "Bonuses", m_aszNoObsoleteBonusTypes);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	// Python hooks: expressions evaluated in CvTraitInterface.py
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_GAME_START], "PyOnGameStart");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_BEGIN_TURN], "PyOnBeginTurn");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_CITY_FOUNDED], "PyOnCityFounded");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_CITY_ACQUIRED], "PyOnCityAcquired");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_CORPORATION_FOUNDED], "PyOnCorporationFounded");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_CITY_LOST], "PyOnCityLost");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_BUILDING_BUILT], "PyOnBuildingBuilt");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_RELIGION_FOUNDED], "PyOnReligionFounded");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_GAINED], "PyOnGained");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_LOST], "PyOnLost");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_TECH_ACQUIRED], "PyOnTechAcquired");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_GREAT_PERSON_BORN], "PyOnGreatPersonBorn");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_GOLDEN_AGE_START], "PyOnGoldenAgeStart");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_GOLDEN_AGE_END], "PyOnGoldenAgeEnd");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_WAR_DECLARED], "PyOnWarDeclared");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_PEACE_MADE], "PyOnPeaceMade");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_ERA_CHANGED], "PyOnEraChanged");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_FIRST_CONTACT], "PyOnFirstContact");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_PROJECT_BUILT], "PyOnProjectBuilt");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_VASSAL_GAINED], "PyOnVassalGained");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_RELIGION_SPREAD], "PyOnReligionSpread");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_CIVIC_CHANGED], "PyOnCivicChanged");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_CIRCUMNAVIGATED], "PyOnCircumnavigated");
	pXML->GetChildXmlValByName(m_aszPythonHook[TRAIT_PYHOOK_OTHER_CITY_FOUNDED], "PyOnOtherCityFounded");
}

// The part of read() for ConditionalFreeTraits (added by this mod)
void CvTraitInfo::readConditionalFreeTraits(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	// ConditionalFreeTraits: <ConditionalFreeTrait> with a TraitType and the conditions. The traits after this one and the
	// civics are loaded later, so readPass3() finds the types
	m_aConditionalFreeTraits.clear();
	m_aszConditionalFreeTraitTypes.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "ConditionalFreeTraits"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "ConditionalFreeTrait"))
		{
			do
			{
				ConditionalFreeTrait kCondition;
				kCondition.iTrait = NO_TRAIT;
				kCondition.bAtWar = false;
				kCondition.bAtPeace = false;
				kCondition.iStateReligion = NO_RELIGION;
				kCondition.bNoStateReligion = false;
				kCondition.iEraMin = NO_ERA;
				kCondition.iEraMax = NO_ERA;
				kCondition.iPrereqCivic = NO_CIVIC;
				kCondition.bFavoriteCivic = false;
				kCondition.iMinUnitsKilled = 0;
				kCondition.iMinCitiesConquered = 0;
				kCondition.bGoldenAge = false;
				kCondition.bNoGoldenAge = false;
				kCondition.bAnarchy = false;
				kCondition.bNoAnarchy = false;
				kCondition.iMinCities = 0;
				kCondition.iMaxCities = 0;
				kCondition.iPrereqBonus = NO_BONUS;
				kCondition.iPrereqBuildingClass = NO_BUILDINGCLASS;
				kCondition.bVassal = false;
				kCondition.bHasVassal = false;
				kCondition.iMinScoreRank = 0;
				// PrereqTech (added by this mod): the techs are loaded before the traits
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "PrereqTech");
				kCondition.iPrereqTech = pXML->FindInInfoClass(szTextVal);
				pXML->GetChildXmlValByName(&kCondition.bAtWar, "bAtWar");
				pXML->GetChildXmlValByName(&kCondition.bAtPeace, "bAtPeace");
				pXML->GetChildXmlValByName(&kCondition.bNoStateReligion, "bNoStateReligion");
				pXML->GetChildXmlValByName(&kCondition.bFavoriteCivic, "bFavoriteCivic");
				pXML->GetChildXmlValByName(&kCondition.iMinUnitsKilled, "iMinUnitsKilled");
				pXML->GetChildXmlValByName(&kCondition.iMinCitiesConquered, "iMinCitiesConquered");
				pXML->GetChildXmlValByName(&kCondition.bGoldenAge, "bGoldenAge");
				pXML->GetChildXmlValByName(&kCondition.bNoGoldenAge, "bNoGoldenAge");
				pXML->GetChildXmlValByName(&kCondition.bAnarchy, "bAnarchy");
				pXML->GetChildXmlValByName(&kCondition.bNoAnarchy, "bNoAnarchy");
				pXML->GetChildXmlValByName(&kCondition.iMinCities, "iMinCities");
				pXML->GetChildXmlValByName(&kCondition.iMaxCities, "iMaxCities");
				pXML->GetChildXmlValByName(&kCondition.bVassal, "bVassal");
				pXML->GetChildXmlValByName(&kCondition.bHasVassal, "bHasVassal");
				pXML->GetChildXmlValByName(&kCondition.iMinScoreRank, "iMinScoreRank");
				pXML->GetChildXmlValByName(kCondition.szPyCondition, "PyCondition");
				pXML->GetChildXmlValByName(kCondition.szPyConditionText, "PyConditionText");
				m_aConditionalFreeTraits.push_back(kCondition);

				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "TraitType");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "StateReligion");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "EraMin");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "EraMax");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "PrereqCivic");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "PrereqBonus");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "PrereqBuildingClass");
				m_aszConditionalFreeTraitTypes.push_back(szTextVal);
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "ConditionalFreeTrait"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for FreePromotionSets (added by this mod)
void CvTraitInfo::readFreePromotionSets(CvXMLLoadUtility* pXML)
{
	// FreePromotionSets: any number of <FreePromotions> x <FreePromotionUnitCombats> pairs
	m_aabFreePromotionSetPromotion.clear();
	m_aabFreePromotionSetUnitCombat.clear();
	m_aaiFreePromotionSetUnitClasses.clear();
	m_abFreePromotionSetNoUnitCombat.clear();
	m_aaiFreePromotionSetDomains.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "FreePromotionSets"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "FreePromotionSet"))
		{
			do
			{
				bool* pabPromotions = NULL;
				bool* pabUnitCombats = NULL;
				pXML->SetVariableListTagPair(&pabPromotions, "FreePromotions", sizeof(GC.getPromotionInfo((PromotionTypes)0)), GC.getNumPromotionInfos());
				pXML->SetVariableListTagPair(&pabUnitCombats, "FreePromotionUnitCombats", sizeof(GC.getUnitCombatInfo((UnitCombatTypes)0)), GC.getNumUnitCombatInfos());

				m_aabFreePromotionSetPromotion.push_back(std::vector<bool>(pabPromotions, pabPromotions + GC.getNumPromotionInfos()));
				m_aabFreePromotionSetUnitCombat.push_back(std::vector<bool>(pabUnitCombats, pabUnitCombats + GC.getNumUnitCombatInfos()));

				SAFE_DELETE_ARRAY(pabPromotions);
				SAFE_DELETE_ARRAY(pabUnitCombats);

				// The units without a unit combat, by unit class or all of them, and the domains (added by this mod). Unit classes and
				// domains are loaded before the traits
				std::vector<int> aiSetUnitClasses;
				std::vector<int> aiSetDomains;
				bool bSetNoUnitCombat = false;
				readTypeList(pXML, "FreePromotionUnitClasses", aiSetUnitClasses);
				pXML->GetChildXmlValByName(&bSetNoUnitCombat, "bFreePromotionNoUnitCombat");
				readTypeList(pXML, "FreePromotionDomains", aiSetDomains);
				m_aaiFreePromotionSetUnitClasses.push_back(aiSetUnitClasses);
				m_abFreePromotionSetNoUnitCombat.push_back(bSetNoUnitCombat);
				m_aaiFreePromotionSetDomains.push_back(aiSetDomains);
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "FreePromotionSet"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for the plot yield changes and the changes per angry citizen and unhealthiness (added by this mod)
void CvTraitInfo::readPlotYieldChanges(CvXMLLoadUtility* pXML)
{
	std::vector<CvString> aszTypes;
	std::vector< std::vector<int> > aaiYields;
	readYieldChanges(pXML, "TerrainYieldChanges", "TerrainYieldChange", "TerrainType", "Yields", aszTypes, aaiYields);
	resolveYieldChanges("TerrainYieldChanges", aszTypes, aaiYields, GC.getNumTerrainInfos(), m_aaiTerrainYieldChange);
	readYieldChanges(pXML, "TerrainYieldChangesPerEra", "TerrainYieldChangePerEra", "TerrainType", "Yields", aszTypes, aaiYields);
	resolveYieldChanges("TerrainYieldChangesPerEra", aszTypes, aaiYields, GC.getNumTerrainInfos(), m_aaiTerrainYieldChangePerEra);
	readYieldChanges(pXML, "FeatureYieldChanges", "FeatureYieldChange", "FeatureType", "Yields", aszTypes, aaiYields);
	resolveYieldChanges("FeatureYieldChanges", aszTypes, aaiYields, GC.getNumFeatureInfos(), m_aaiFeatureYieldChange);
	// The features are loaded before the traits
	readTypeValueList(pXML, "FeatureProductionChanges", "FeatureProductionChange", "FeatureType", "iProductionChange", GC.getNumFeatureInfos(), m_aiFeatureProductionChange);
	readTypeValueList(pXML, "FeatureGrowthChanges", "FeatureGrowthChange", "FeatureType", "iGrowthChange", GC.getNumFeatureInfos(), m_aiFeatureGrowthChange);
	readTypeValueList(pXML, "FeatureHealthChanges", "FeatureHealthChange", "FeatureType", "iHealthPercent", GC.getNumFeatureInfos(), m_aiFeatureHealthChange);
	// The same format as the building SpecialistYieldChanges. The specialists are loaded before the traits
	readYieldChanges(pXML, "SpecialistYieldChanges", "SpecialistYieldChange", "SpecialistType", "YieldChanges", aszTypes, aaiYields);
	resolveYieldChanges("SpecialistYieldChanges", aszTypes, aaiYields, GC.getNumSpecialistInfos(), m_aaiSpecialistYieldChange);
	readYieldChanges(pXML, "SpecialistCommerceChanges", "SpecialistCommerceChange", "SpecialistType", "CommerceChanges", aszTypes, aaiYields, true);
	resolveYieldChanges("SpecialistCommerceChanges", aszTypes, aaiYields, GC.getNumSpecialistInfos(), m_aaiSpecialistCommerceChange, NUM_COMMERCE_TYPES);
	// Same format as the civic ImprovementYieldChanges. Resolved in readPass3()
	readYieldChanges(pXML, "ImprovementYieldChanges", "ImprovementYieldChange", "ImprovementType", "ImprovementYields", m_aszImprovementYieldChangeTypes, m_aaiImprovementYieldChangeValues);
	// Same format as TerrainYieldChanges. Resolved in readPass3()
	readYieldChanges(pXML, "BonusYieldChanges", "BonusYieldChange", "BonusType", "Yields", m_aszBonusYieldChangeTypes, m_aaiBonusYieldChangeValues);
	readYieldChanges(pXML, "RiverPlotYieldChanges", &m_paiRiverPlotYieldChange);
	readYieldChanges(pXML, "RiverEdgeYieldPercents", &m_paiRiverEdgeYieldPercent);
	readYieldChanges(pXML, "LakePlotYieldChanges", &m_paiLakePlotYieldChange);
	readYieldChanges(pXML, "LakePlotYieldChangesPerEra", &m_paiLakePlotYieldChangePerEra);
	readYieldChanges(pXML, "AngryYieldChanges", &m_paiAngryYieldChange);
	readCommerceChanges(pXML, "AngryCommerceChanges", &m_paiAngryCommerceChange);
	readYieldChanges(pXML, "UnhealthyYieldChanges", &m_paiUnhealthyYieldChange);
	readCommerceChanges(pXML, "UnhealthyCommerceChanges", &m_paiUnhealthyCommerceChange);
}

// The part of read() for the tags of 2026-11-18: upgrades, combat experience, celebration, specialist slots, hurry, improved
// bonuses and the changes per extra happiness and health (added by this mod)
void CvTraitInfo::readUnitAndCityValues(CvXMLLoadUtility* pXML)
{
	pXML->GetChildXmlValByName(&m_iMaxExperienceAfterUpgrade, "iMaxExperienceAfterUpgrade");
	pXML->GetChildXmlValByName(&m_iExpOutsideBordersChange, "iExpOutsideBordersChange");
	pXML->GetChildXmlValByName(&m_iWeLoveTheKingChance, "iWeLoveTheKingChance");
	// The specialists are loaded before the traits
	readTypeValueList(pXML, "SpecialistSlotChanges", "SpecialistSlotChange", "SpecialistType", "iSlots", GC.getNumSpecialistInfos(), m_aiSpecialistSlotChange);
	// The hurries are loaded after the traits: readPass3() finds the types
	// Same format as BonusYieldChanges. Resolved in readPass3()
	readYieldChanges(pXML, "ImprovedBonusYieldChanges", "ImprovedBonusYieldChange", "BonusType", "Yields", m_aszImprovedBonusYieldChangeTypes, m_aaiImprovedBonusYieldChangeValues);
	readYieldChanges(pXML, "HappyYieldPercents", &m_paiHappyYieldPercent);
	readCommerceChanges(pXML, "HappyCommercePercents", &m_paiHappyCommercePercent);
	readYieldChanges(pXML, "HealthyYieldPercents", &m_paiHealthyYieldPercent);
	readCommerceChanges(pXML, "HealthyCommercePercents", &m_paiHealthyCommercePercent);
}

// The part of read() for the tags of 2026-11-19: revealed bonuses, processes without tech, drafted units and the techs at
// the start of the game (added by this mod)
void CvTraitInfo::readRevealAndStartValues(CvXMLLoadUtility* pXML)
{
	// Bonuses: readPass3(). Processes are loaded with LoadPostMenuGlobals: readPass4()
	readTypeList(pXML, "RevealBonuses", m_aszRevealBonusTypes);
	readTypeList(pXML, "NoTechProcesses", m_aszNoTechProcessTypes);
	// Same format as FreeTechs of the civilizations. The techs are loaded before the traits
	pXML->SetVariableListTagPair(&m_pabFreeTech, "FreeTechs", sizeof(GC.getTechInfo((TechTypes)0)), GC.getNumTechInfos());
}

// BonusProductionModifiers of the traits (added by this mod): <BonusProductionModifier> with BonusType, iProductionModifier
// and the optional bWorldWondersOnly
static void readBonusProductionModifiers(CvXMLLoadUtility* pXML, std::vector<CvString>& aszTypes, std::vector<int>& aiValues, std::vector<int>& aiWonders)
{
	CvString szTextVal;
	int iValue;
	bool bWonders;

	aszTypes.clear();
	aiValues.clear();
	aiWonders.clear();

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "BonusProductionModifiers"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "BonusProductionModifier"))
		{
			do
			{
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "BonusType");
				pXML->GetChildXmlValByName(&iValue, "iProductionModifier", 0);
				pXML->GetChildXmlValByName(&bWonders, "bWorldWondersOnly", false);
				if (!szTextVal.empty() && iValue != 0)
				{
					aszTypes.push_back(szTextVal);
					aiValues.push_back(iValue);
					aiWonders.push_back(bWonders ? 1 : 0);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "BonusProductionModifier"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// Hurrys of the traits (added by this mod): <Hurry> with HurryType and the optional flags and values of TraitHurry
static void readTraitHurrys(CvXMLLoadUtility* pXML, std::vector<TraitHurry>& aHurrys, std::vector<CvString>& aszTypes)
{
	CvString szTextVal;

	aHurrys.clear();
	aszTypes.clear();

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Hurrys"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Hurry"))
		{
			do
			{
				TraitHurry kHurry;
				szTextVal = "";
				pXML->GetChildXmlValByName(szTextVal, "HurryType");
				kHurry.iHurry = NO_HURRY;
				pXML->GetChildXmlValByName(&kHurry.bUnitsOnly, "bUnitsOnly", false);
				pXML->GetChildXmlValByName(&kHurry.bBuildingsOnly, "bBuildingsOnly", false);
				pXML->GetChildXmlValByName(&kHurry.bWondersOnly, "bWondersOnly", false);
				pXML->GetChildXmlValByName(&kHurry.bWorldWondersOnly, "bWorldWondersOnly", false);
				pXML->GetChildXmlValByName(&kHurry.bNationalWondersOnly, "bNationalWondersOnly", false);
				pXML->GetChildXmlValByName(&kHurry.iCostModifier, "iCostModifier", 0);
				pXML->GetChildXmlValByName(&kHurry.bIgnoreCostModifier, "bIgnoreCostModifier", false);
				pXML->GetChildXmlValByName(&kHurry.iExperience, "iExperience", 0);
				if (!szTextVal.empty())
				{
					aHurrys.push_back(kHurry);
					aszTypes.push_back(szTextVal);
				}
			}
			while (gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "Hurry"));

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

// The part of read() for the counterparts of the tags that existed for only one of happiness/health or yield/commerce
// (added by this mod)
void CvTraitInfo::readCounterpartValues(CvXMLLoadUtility* pXML)
{
	readYieldChanges(pXML, "YieldChanges", &m_paiYieldChange);
	pXML->GetChildXmlValByName(&m_iHealthPerReligion, "iHealthPerReligion");
	readYieldChanges(pXML, "YieldPerReligion", &m_paiYieldPerReligion);
	// The features are loaded before the traits. Same format as the civics
	readTypeValueList(pXML, "FeatureHappinessChanges", "FeatureHappinessChange", "FeatureType", "iHappinessChange", GC.getNumFeatureInfos(), m_aiFeatureHappinessChange);
	readYieldChanges(pXML, "WorldWonderYieldChanges", &m_paiWorldWonderYieldChange);
	readYieldChanges(pXML, "GlobalWorldWonderYieldChanges", &m_paiGlobalWorldWonderYieldChange);
	readYieldChanges(pXML, "SpecialistExtraYields", &m_paiSpecialistExtraYield);
	pXML->GetChildXmlValByName(&m_iNonStateReligionHappiness, "iNonStateReligionHappiness");
	// Building classes and bonuses are loaded after the traits: readPass3()
	readTypeStringValueList(pXML, "BuildingHappinessChanges", "BuildingHappinessChange", "BuildingClassType", "iHappinessChange", m_aszBuildingHappinessChangeTypes, m_aiBuildingHappinessChangeValues);
	readTypeStringValueList(pXML, "BuildingHealthChanges", "BuildingHealthChange", "BuildingClassType", "iHealthChange", m_aszBuildingHealthChangeTypes, m_aiBuildingHealthChangeValues);
	readTypeStringValueList(pXML, "BonusHappinessChanges", "BonusHappinessChange", "BonusType", "iHappinessChange", m_aszBonusHappinessChangeTypes, m_aiBonusHappinessChangeValues);
	readTypeStringValueList(pXML, "BonusHealthChanges", "BonusHealthChange", "BonusType", "iHealthChange", m_aszBonusHealthChangeTypes, m_aiBonusHealthChangeValues);
	// TerrainTrades (the format of the techs) and CapitalTerrainYieldPercents (the format of TerrainYieldChanges). The
	// terrains are loaded before the traits
	pXML->SetVariableListTagPair(&m_pabTerrainTrade, "TerrainTrades", sizeof(GC.getTerrainInfo((TerrainTypes)0)), GC.getNumTerrainInfos(), false);
	{
		std::vector<CvString> aszTypes;
		std::vector< std::vector<int> > aaiYields;
		readYieldChanges(pXML, "CapitalTerrainYieldPercents", "CapitalTerrainYieldPercent", "TerrainType", "Yields", aszTypes, aaiYields);
		resolveYieldChanges("CapitalTerrainYieldPercents", aszTypes, aaiYields, GC.getNumTerrainInfos(), m_aaiCapitalTerrainYieldPercent);
	}
	// Hurrys. The hurries are loaded after the traits: readPass3()
	readTraitHurrys(pXML, m_aHurrys, m_aszHurryTypes);
	// Bonuses and commerce rates. The bonuses are loaded after the traits: readPass3()
	readTypeStringValueList(pXML, "FreeBonuses", "FreeBonus", "BonusType", "iNumBonuses", m_aszFreeBonusTypes, m_aiFreeBonusValues);
	readBonusProductionModifiers(pXML, m_aszBonusProductionModifierTypes, m_aiBonusProductionModifierValues, m_aiBonusProductionModifierWonders);
	readCommerceChanges(pXML, "CommerceHappinesses", &m_paiCommerceHappiness);
	readCommerceChanges(pXML, "CommerceHealths", &m_paiCommerceHealth);
	// Unit support and maintenance, as the civics and buildings
	pXML->GetChildXmlValByName(&m_iBaseFreeUnits, "iBaseFreeUnits");
	pXML->GetChildXmlValByName(&m_iBaseFreeMilitaryUnits, "iBaseFreeMilitaryUnits");
	pXML->GetChildXmlValByName(&m_iFreeUnitsPopulationPercent, "iFreeUnitsPopulationPercent");
	pXML->GetChildXmlValByName(&m_iFreeMilitaryUnitsPopulationPercent, "iFreeMilitaryUnitsPopulationPercent");
	pXML->GetChildXmlValByName(&m_iGoldPerUnit, "iGoldPerUnit");
	pXML->GetChildXmlValByName(&m_iGoldPerMilitaryUnit, "iGoldPerMilitaryUnit");
	pXML->GetChildXmlValByName(&m_iCorporationMaintenanceModifier, "iCorporationMaintenanceModifier");
	pXML->GetChildXmlValByName(&m_iMaintenanceModifier, "iMaintenanceModifier");
	// Military, as the civics and buildings. The domains are loaded before the traits
	// Conscription: iMaxConscript, iPopulationModifier, iMinCityPopulationChange, iMinCulturePercentChange, iPopAngerChange, iAngerLengthPercent,
	// bNoExperiencePenalty
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Conscription"))
	{
		pXML->GetChildXmlValByName(&m_iMaxConscript, "iMaxConscript");
		pXML->GetChildXmlValByName(&m_iConscriptPopulationModifier, "iPopulationModifier");
		pXML->GetChildXmlValByName(&m_iConscriptMinCityPopulationChange, "iMinCityPopulationChange");
		pXML->GetChildXmlValByName(&m_iConscriptMinCulturePercentChange, "iMinCulturePercentChange");
		pXML->GetChildXmlValByName(&m_iConscriptPopAngerChange, "iPopAngerChange");
		pXML->GetChildXmlValByName(&m_iConscriptAngerLengthPercent, "iAngerLengthPercent");
		pXML->GetChildXmlValByName(&m_bNoConscriptExperiencePenalty, "bNoExperiencePenalty");
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_bMilitaryFoodProduction, "bMilitaryFoodProduction");
	pXML->GetChildXmlValByName(&m_iHealRateChange, "iHealRateChange");
	readTypeValueList(pXML, "DomainFreeExperiences", "DomainFreeExperience", "DomainType", "iExperience", NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	readTypeValueList(pXML, "DomainProductionModifiers", "DomainProductionModifier", "DomainType", "iProductionModifier", NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	// Cities and population, as the civics and buildings
	pXML->GetChildXmlValByName(&m_iLargestCityHappiness, "iLargestCityHappiness");
	pXML->GetChildXmlValByName(&m_iHappyPerMilitaryUnit, "iHappyPerMilitaryUnit");
	pXML->GetChildXmlValByName(&m_iFoodKept, "iFoodKept");
	pXML->GetChildXmlValByName(&m_bNoUnhealthyPopulation, "bNoUnhealthyPopulation");
	pXML->GetChildXmlValByName(&m_bBuildingOnlyHealthy, "bBuildingOnlyHealthy");
	// Trade routes and defense, as the buildings
	pXML->GetChildXmlValByName(&m_iCoastalTradeRoutes, "iCoastalTradeRoutes");
	pXML->GetChildXmlValByName(&m_iTradeRouteModifier, "iTradeRouteModifier");
	pXML->GetChildXmlValByName(&m_iForeignTradeRouteModifier, "iForeignTradeRouteModifier");
	pXML->GetChildXmlValByName(&m_iAllCityDefense, "iAllCityDefense");
	pXML->GetChildXmlValByName(&m_iBombardDefense, "iBombardDefense");
	pXML->GetChildXmlValByName(&m_iEspionageDefense, "iEspionageDefense");
}

// Turns the types and values of readTypeStringValueList() into a value per info (iNumInfos), the vector left empty when
// nothing is set. An unknown type shows the same message as FindInInfoClass (added by this mod)
static void resolveTypeValueList(const TCHAR* szListTag, std::vector<CvString>& aszTypes, std::vector<int>& aiValues, int iNumInfos, std::vector<int>& aiResult)
{
	char szMessage[1024];
	int iIndex;
	int i;

	aiResult.clear();
	for (i = 0; i < (int)aszTypes.size() && i < (int)aiValues.size(); i++)
	{
		iIndex = GC.getInfoTypeForString(aszTypes[i].GetCString(), true);
		if (iIndex < 0 || iIndex >= iNumInfos)
		{
			sprintf(szMessage, "Tag: %s in %s of CIV4TraitInfos.xml was incorrect", aszTypes[i].GetCString(), szListTag);
			gDLL->MessageBox(szMessage, "XML Error");
			continue;
		}
		if (aiResult.empty())
		{
			aiResult.resize(iNumInfos, 0);
		}
		aiResult[iIndex] += aiValues[i];
	}
	aszTypes.clear();
	aiValues.clear();
}

// NoTechProcesses (added by this mod): CvXMLLoadUtility::LoadPostMenuGlobals calls this after it loads the processes. The
// types stay as strings, in case it runs again
void CvTraitInfo::readPass4()
{
	resolveTypeList("NoTechProcesses", m_aszNoTechProcessTypes, GC.getNumProcessInfos(), &m_pabNoTechProcess);
}

bool CvTraitInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, "ShortDescription");
	setShortDescription(szTextVal);

	pXML->GetChildXmlValByName(&m_iHealth, "iHealth");
	pXML->GetChildXmlValByName(&m_iHappiness, "iHappiness");
	pXML->GetChildXmlValByName(&m_iMaxAnarchy, "iMaxAnarchy", -1);
	pXML->GetChildXmlValByName(&m_iUpkeepModifier, "iUpkeepModifier");
	pXML->GetChildXmlValByName(&m_iLevelExperienceModifier, "iLevelExperienceModifier");
	pXML->GetChildXmlValByName(&m_iGreatPeopleRateModifier, "iGreatPeopleRateModifier");
	pXML->GetChildXmlValByName(&m_iGreatGeneralRateModifier, "iGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, "iDomesticGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iMaxGlobalBuildingProductionModifier, "iMaxGlobalBuildingProductionModifier");
	pXML->GetChildXmlValByName(&m_iMaxTeamBuildingProductionModifier, "iMaxTeamBuildingProductionModifier");
	pXML->GetChildXmlValByName(&m_iMaxPlayerBuildingProductionModifier, "iMaxPlayerBuildingProductionModifier");
	// The rest, in parts in the order it was read before: the tags of the mod and some arrays of BtS (changed by this mod)
	readModValues(pXML);
	readCityFoundedUnits(pXML);
	readStartBias(pXML);
	readCityAndEconomyValues(pXML);
	readYieldAndUnitArrays(pXML);
	readFeatureRoutes(pXML);
	readObsolescenceAndHooks(pXML);
	readConditionalFreeTraits(pXML);
	readFreePromotionSets(pXML);
	readPlotYieldChanges(pXML);
	readUnitAndCityValues(pXML);
	readRevealAndStartValues(pXML);
	readCounterpartValues(pXML);

	return true;
}

// The index of a type of ConditionalFreeTraits (iNumInfos: the size of its info class), -1 for an empty string or NONE.
// An unknown type (or a type of another info class) shows the same message as FindInInfoClass.
static int resolveConditionalType(const CvString& szType, int iNumInfos)
{
	char szMessage[1024];
	int iIndex;

	if (szType.empty() || szType == "NONE")
	{
		return -1;
	}

	iIndex = GC.getInfoTypeForString(szType.GetCString(), true);
	if (iIndex < 0 || iIndex >= iNumInfos)
	{
		sprintf(szMessage, "Tag: %s in ConditionalFreeTraits of CIV4TraitInfos.xml was incorrect", szType.GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
		return -1;
	}

	return iIndex;
}

// Improvements, building classes and civics are loaded after traits (CvXMLLoadUtility::LoadPreMenuGlobals calls this
// after them)
// UniqueUnits: the unit must be of the class, as in the civilizations (added by this mod). Units are loaded after readPass3()
void CvTraitInfo::readUniqueUnits()
{
	char szMessage[1024];
	int i;

	m_aiUniqueUnitClasses.clear();
	m_aiUniqueUnits.clear();
	for (i = 0; i < (int)m_aszUniqueUnitTypes.size(); i++)
	{
		int iClass = GC.getInfoTypeForString(m_aszUniqueUnitClassTypes[i].GetCString(), true);
		int iUnit = GC.getInfoTypeForString(m_aszUniqueUnitTypes[i].GetCString(), true);
		if (iClass < 0 || iUnit < 0 || GC.getUnitInfo((UnitTypes)iUnit).getUnitClassType() != iClass)
		{
			sprintf(szMessage, "UniqueUnits of CIV4TraitInfos.xml: %s is not of %s", m_aszUniqueUnitTypes[i].GetCString(), m_aszUniqueUnitClassTypes[i].GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
		}
		else
		{
			m_aiUniqueUnitClasses.push_back(iClass);
			m_aiUniqueUnits.push_back(iUnit);
		}
	}
}

// A type of CIV4TraitInfos.xml kept as a string until readPass3() (added by this mod): -1 for an empty string or NONE, and
// for an unknown type (with a message box naming szTag)
static int resolveTraitTypeString(const CvString& szType, const char* szTag)
{
	char szMessage[1024];
	int iType;

	if (szType.empty() || szType == "NONE")
	{
		return -1;
	}

	iType = GC.getInfoTypeForString(szType.GetCString(), true);
	if (iType < 0)
	{
		sprintf(szMessage, "Tag: %s in %s of CIV4TraitInfos.xml was incorrect", szType.GetCString(), szTag);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	return iType;
}

// FoundingVassal (added by this mod): the civilization, the leader and the difficulty. The leaders and the civilizations are
// loaded after readPass3(), so CvXMLLoadUtility::LoadPreMenuGlobals calls this after the civilizations. NONE or no tag: -1
void CvTraitInfo::readFoundingVassalTypes()
{
	m_iFoundingVassalCivilization = resolveTraitTypeString(m_szFoundingVassalCivilization, "FoundingVassal/CivilizationType");
	m_iFoundingVassalLeader = resolveTraitTypeString(m_szFoundingVassalLeader, "FoundingVassal/LeaderType");
	m_iFoundingVassalHandicap = resolveTraitTypeString(m_szFoundingVassalHandicap, "FoundingVassal/HandicapType");
}

bool CvTraitInfo::readPass3()
{
	char szMessage[1024];
	int iRoute;
	int i;

	// StartBias/BonusBiases (added by this mod)
	SAFE_DELETE_ARRAY(m_piStartBiasBonuses);
	if (!m_aszStartBiasBonusTypes.empty())
	{
		m_piStartBiasBonuses = new int[GC.getNumBonusInfos()];
		for (i = 0; i < GC.getNumBonusInfos(); i++)
		{
			m_piStartBiasBonuses[i] = 0;
		}
		for (i = 0; i < (int)m_aszStartBiasBonusTypes.size(); i++)
		{
			int iBiasBonus = GC.getInfoTypeForString(m_aszStartBiasBonusTypes[i].GetCString());
			if (iBiasBonus > -1)
			{
				m_piStartBiasBonuses[iBiasBonus] = m_aiStartBiasBonusValues[i];
			}
		}
	}
	m_aszStartBiasBonusTypes.clear();
	m_aiStartBiasBonusValues.clear();

	// UniqueBuildings: the building must be of the class, as in the civilizations (added by this mod)
	m_aiUniqueBuildingClasses.clear();
	m_aiUniqueBuildings.clear();
	for (i = 0; i < (int)m_aszUniqueBuildingTypes.size(); i++)
	{
		int iClass = GC.getInfoTypeForString(m_aszUniqueBuildingClassTypes[i].GetCString(), true);
		int iBuilding = GC.getInfoTypeForString(m_aszUniqueBuildingTypes[i].GetCString(), true);
		if (iClass < 0 || iBuilding < 0 || GC.getBuildingInfo((BuildingTypes)iBuilding).getBuildingClassType() != iClass)
		{
			sprintf(szMessage, "UniqueBuildings of CIV4TraitInfos.xml: %s is not of %s", m_aszUniqueBuildingTypes[i].GetCString(), m_aszUniqueBuildingClassTypes[i].GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
		}
		else
		{
			m_aiUniqueBuildingClasses.push_back(iClass);
			m_aiUniqueBuildings.push_back(iBuilding);
		}
	}

	SAFE_DELETE_ARRAY(m_paiFeatureRoute);
	if (!m_aiFeatureRouteFeatures.empty())
	{
		m_paiFeatureRoute = new int[GC.getNumFeatureInfos()];
		for (i = 0; i < GC.getNumFeatureInfos(); i++)
		{
			m_paiFeatureRoute[i] = NO_ROUTE;
		}

		for (i = 0; i < (int)m_aiFeatureRouteFeatures.size(); i++)
		{
			iRoute = GC.getInfoTypeForString(m_aszFeatureRouteTypes[i].GetCString(), true);
			if (iRoute < 0 && !m_aszFeatureRouteTypes[i].empty() && m_aszFeatureRouteTypes[i] != "NONE")
			{
				sprintf(szMessage, "Tag: %s in FeatureRoutes of CIV4TraitInfos.xml was incorrect", m_aszFeatureRouteTypes[i].GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
			}
			m_paiFeatureRoute[m_aiFeatureRouteFeatures[i]] = std::max(iRoute, (int)NO_ROUTE);
		}
	}

	m_iCityRadiusCultureLevel = NO_CULTURELEVEL;
	if (!m_szCityRadiusCultureLevel.empty() && m_szCityRadiusCultureLevel != "NONE")
	{
		m_iCityRadiusCultureLevel = GC.getInfoTypeForString(m_szCityRadiusCultureLevel.GetCString(), true);
		if (m_iCityRadiusCultureLevel < 0)
		{
			sprintf(szMessage, "Tag: %s in CityRadiusCultureLevel of CIV4TraitInfos.xml was incorrect", m_szCityRadiusCultureLevel.GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
			m_iCityRadiusCultureLevel = NO_CULTURELEVEL;
		}
	}

	resolveYieldChanges("ImprovementYieldChanges", m_aszImprovementYieldChangeTypes, m_aaiImprovementYieldChangeValues, GC.getNumImprovementInfos(), m_aaiImprovementYieldChange);
	m_aszImprovementYieldChangeTypes.clear();
	m_aaiImprovementYieldChangeValues.clear();

	resolveYieldChanges("BonusYieldChanges", m_aszBonusYieldChangeTypes, m_aaiBonusYieldChangeValues, GC.getNumBonusInfos(), m_aaiBonusYieldChange);
	m_aszBonusYieldChangeTypes.clear();
	m_aaiBonusYieldChangeValues.clear();

	m_iNewCityCultureLevel = NO_CULTURELEVEL;
	if (!m_szNewCityCultureLevel.empty() && m_szNewCityCultureLevel != "NONE")
	{
		m_iNewCityCultureLevel = GC.getInfoTypeForString(m_szNewCityCultureLevel.GetCString(), true);
		if (m_iNewCityCultureLevel < 0)
		{
			sprintf(szMessage, "Tag: %s in NewCityCultureLevel of CIV4TraitInfos.xml was incorrect", m_szNewCityCultureLevel.GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
			m_iNewCityCultureLevel = NO_CULTURELEVEL;
		}
	}

	// ConditionalFreeTraits: items without a valid trait are removed. readConditionalFreeTraits keeps 7 types per item
	const int iNumTypes = 7;
	std::vector<ConditionalFreeTrait> aConditions;
	for (i = 0; i < (int)m_aConditionalFreeTraits.size() && (i * iNumTypes + iNumTypes - 1) < (int)m_aszConditionalFreeTraitTypes.size(); i++)
	{
		ConditionalFreeTrait kCondition = m_aConditionalFreeTraits[i];
		kCondition.iTrait = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes], GC.getNumTraitInfos());
		kCondition.iStateReligion = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes + 1], GC.getNumReligionInfos());
		kCondition.iEraMin = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes + 2], GC.getNumEraInfos());
		kCondition.iEraMax = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes + 3], GC.getNumEraInfos());
		kCondition.iPrereqCivic = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes + 4], GC.getNumCivicInfos());
		kCondition.iPrereqBonus = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes + 5], GC.getNumBonusInfos());	// added by this mod
		kCondition.iPrereqBuildingClass = resolveConditionalType(m_aszConditionalFreeTraitTypes[i * iNumTypes + 6], GC.getNumBuildingClassInfos());	// added by this mod
		if (kCondition.iTrait != NO_TRAIT)
		{
			aConditions.push_back(kCondition);
		}
	}
	m_aConditionalFreeTraits = aConditions;
	m_aszConditionalFreeTraitTypes.clear();

	resolveTypeList("ForbiddenCivics", m_aszForbiddenCivicTypes, GC.getNumCivicInfos(), &m_pabForbiddenCivic);
	m_aszForbiddenCivicTypes.clear();
	resolveTypeList("NoTechCivics", m_aszNoTechCivicTypes, GC.getNumCivicInfos(), &m_pabNoTechCivic);
	m_aszNoTechCivicTypes.clear();
	resolveTypeList("Conquest/ReligionBuildings", m_aszConquestReligionBuildingTypes, GC.getNumSpecialBuildingInfos(), &m_pabConquestReligionBuilding);
	m_aszConquestReligionBuildingTypes.clear();
	resolveTypeList("RevealBonuses", m_aszRevealBonusTypes, GC.getNumBonusInfos(), &m_pabRevealBonus);
	m_aszRevealBonusTypes.clear();
	resolveTypeValueList("BuildingHappinessChanges", m_aszBuildingHappinessChangeTypes, m_aiBuildingHappinessChangeValues, GC.getNumBuildingClassInfos(), m_aiBuildingHappinessChange);
	resolveTypeValueList("BuildingHealthChanges", m_aszBuildingHealthChangeTypes, m_aiBuildingHealthChangeValues, GC.getNumBuildingClassInfos(), m_aiBuildingHealthChange);
	resolveTypeValueList("BonusHappinessChanges", m_aszBonusHappinessChangeTypes, m_aiBonusHappinessChangeValues, GC.getNumBonusInfos(), m_aiBonusHappinessChange);
	resolveTypeValueList("BonusHealthChanges", m_aszBonusHealthChangeTypes, m_aiBonusHealthChangeValues, GC.getNumBonusInfos(), m_aiBonusHealthChange);
	resolveTypeValueList("FreeBonuses", m_aszFreeBonusTypes, m_aiFreeBonusValues, GC.getNumBonusInfos(), m_aiFreeBonus);
	// BonusProductionModifiers: the items with bWorldWondersOnly go to their own vector
	{
		std::vector<CvString> aszAll, aszWonders;
		std::vector<int> aiAll, aiWonders;
		for (i = 0; i < (int)m_aszBonusProductionModifierTypes.size(); i++)
		{
			if (m_aiBonusProductionModifierWonders[i] != 0)
			{
				aszWonders.push_back(m_aszBonusProductionModifierTypes[i]);
				aiWonders.push_back(m_aiBonusProductionModifierValues[i]);
			}
			else
			{
				aszAll.push_back(m_aszBonusProductionModifierTypes[i]);
				aiAll.push_back(m_aiBonusProductionModifierValues[i]);
			}
		}
		resolveTypeValueList("BonusProductionModifiers", aszAll, aiAll, GC.getNumBonusInfos(), m_aiBonusProductionModifier);
		resolveTypeValueList("BonusProductionModifiers", aszWonders, aiWonders, GC.getNumBonusInfos(), m_aiBonusWonderProductionModifier);
		m_aszBonusProductionModifierTypes.clear();
		m_aiBonusProductionModifierValues.clear();
		m_aiBonusProductionModifierWonders.clear();
	}

	// Hurrys (added by this mod): items without a valid hurry are removed
	{
		std::vector<TraitHurry> aHurrys;
		for (i = 0; i < (int)m_aHurrys.size() && i < (int)m_aszHurryTypes.size(); i++)
		{
			TraitHurry kHurry = m_aHurrys[i];
			kHurry.iHurry = GC.getInfoTypeForString(m_aszHurryTypes[i].GetCString(), true);
			if (kHurry.iHurry < 0 || kHurry.iHurry >= GC.getNumHurryInfos())
			{
				sprintf(szMessage, "Tag: %s in Hurrys of CIV4TraitInfos.xml was incorrect", m_aszHurryTypes[i].GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
				continue;
			}
			aHurrys.push_back(kHurry);
		}
		m_aHurrys = aHurrys;
		m_aszHurryTypes.clear();
	}

	resolveYieldChanges("ImprovedBonusYieldChanges", m_aszImprovedBonusYieldChangeTypes, m_aaiImprovedBonusYieldChangeValues, GC.getNumBonusInfos(), m_aaiImprovedBonusYieldChange);
	m_aszImprovedBonusYieldChangeTypes.clear();
	m_aaiImprovedBonusYieldChangeValues.clear();
	resolveTypeList("NoObsolescence/BuildingClasses", m_aszNoObsoleteBuildingClassTypes, GC.getNumBuildingClassInfos(), &m_pabNoObsoleteBuildingClass);
	m_aszNoObsoleteBuildingClassTypes.clear();
	resolveTypeList("NoObsolescence/SpecialBuildings", m_aszNoObsoleteSpecialBuildingTypes, GC.getNumSpecialBuildingInfos(), &m_pabNoObsoleteSpecialBuilding);
	m_aszNoObsoleteSpecialBuildingTypes.clear();
	resolveTypeList("NoObsolescence/Bonuses", m_aszNoObsoleteBonusTypes, GC.getNumBonusInfos(), &m_pabNoObsoleteBonus);
	m_aszNoObsoleteBonusTypes.clear();

	// SpecialBuildingProductionModifiers (added by this mod)
	m_aiSpecialBuildingProductionModifier.clear();
	for (i = 0; i < (int)m_aszSpecialBuildingProductionTypes.size(); i++)
	{
		int iSpecialBuilding = GC.getInfoTypeForString(m_aszSpecialBuildingProductionTypes[i].GetCString(), true);
		if (iSpecialBuilding < 0 || iSpecialBuilding >= GC.getNumSpecialBuildingInfos())
		{
			sprintf(szMessage, "Tag: %s in SpecialBuildingProductionModifiers of CIV4TraitInfos.xml was incorrect", m_aszSpecialBuildingProductionTypes[i].GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
			continue;
		}
		if (m_aiSpecialBuildingProductionModifier.empty())
		{
			m_aiSpecialBuildingProductionModifier.resize(GC.getNumSpecialBuildingInfos(), 0);
		}
		m_aiSpecialBuildingProductionModifier[iSpecialBuilding] += m_aiSpecialBuildingProductionValues[i];
	}
	m_aszSpecialBuildingProductionTypes.clear();
	m_aiSpecialBuildingProductionValues.clear();

	// CivicOptionExtraSlots (added by this mod)
	m_aiCivicOptionExtraSlots.clear();
	for (i = 0; i < (int)m_aszCivicOptionExtraSlotTypes.size(); i++)
	{
		int iCivicOption = GC.getInfoTypeForString(m_aszCivicOptionExtraSlotTypes[i].GetCString(), true);
		if (iCivicOption < 0 || iCivicOption >= GC.getNumCivicOptionInfos())
		{
			sprintf(szMessage, "Tag: %s in CivicOptionExtraSlots of CIV4TraitInfos.xml was incorrect", m_aszCivicOptionExtraSlotTypes[i].GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
			continue;
		}
		if (m_aiCivicOptionExtraSlots.empty())
		{
			m_aiCivicOptionExtraSlots.resize(GC.getNumCivicOptionInfos(), 0);
		}
		m_aiCivicOptionExtraSlots[iCivicOption] += m_aiCivicOptionExtraSlotValues[i];
	}
	m_aszCivicOptionExtraSlotTypes.clear();
	m_aiCivicOptionExtraSlotValues.clear();

	return true;
}
