//---------------------------------------------------------------------------------------
//
//  FILE:    CvGameTextMgrTrait.cpp
//
//  PURPOSE: The help text of the traits added by this mod (the parts of CvGameTextMgr::parseTraits),
//           moved out of CvGameTextMgr.cpp because of its size. The helpers shared with CvGameTextMgr.cpp
//           are declared in CvGameTextMgrUtils.h
//
//---------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvInfos.h"
#include "CvXMLLoadUtility.h"
#include "CvCity.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvGameAI.h"
#include "CvSelectionGroup.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvPopupInfo.h"
#include "FProfiler.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvGameTextMgrUtils.h"

// 1/10 % as text: 5 -> "0.5", 200 -> "20" (added by this mod)
static CvWString getPerMilleText(int iValue)
{
	return ((iValue % 10) == 0) ? CvWString::format(L"%d", iValue / 10) : CvWString::format(L"%d.%d", iValue / 10, iValue % 10);
}

// Joins the conditions of ConditionalFreeTraits (added by this mod)
static void appendConditionText(CvWString& szConditions, const CvWString& szCondition)
{
	if (!szConditions.empty())
	{
		szConditions += gDLL->getText("TXT_KEY_CONDITION_SEPARATOR");
	}
	szConditions += szCondition;
}

// One line of the help of a trait, with the newline and the indent of the trait (added by this mod)
static void appendTraitLine(CvWStringBuffer& szHelpString, const CvWString& szLine)
{
	szHelpString.append(NEWLINE L"  ");
	szHelpString.append(szLine);
}

// One line of the help of a trait with szKey and iValue, when iValue is not 0 (added by this mod)
static void appendTraitValueLine(CvWStringBuffer& szHelpString, const char* szKey, int iValue)
{
	if (iValue != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText(szKey, iValue));
	}
}

// One line of the help of a trait with szKey, when bFlag is set (added by this mod)
static void appendTraitFlagLine(CvWStringBuffer& szHelpString, const char* szKey, bool bFlag)
{
	if (bFlag)
	{
		appendTraitLine(szHelpString, gDLL->getText(szKey));
	}
}

// A name that links to its Civilopedia page (added by this mod)
CvWString getLinkText(const wchar* szName)
{
	return CvWString::format(L"<link=literal>%s</link>", szName);
}

// One entry of TerrainYieldChanges and the like: "+1 food: Forest, Jungle". iLast starts a new line when the value changes
// (added by this mod)
static void appendTraitPlotYieldChange(CvWStringBuffer& szHelpString, int iChange, int iYield, const wchar* szName, int& iLast, const char* szKey = "TXT_KEY_CIVIC_IMPROVEMENT_YIELD_CHANGE")
{
	if (iChange != 0)
	{
		CvWString szStart;
		szStart.Format(L"%s  %s", NEWLINE, gDLL->getText(szKey, iChange, GC.getYieldInfo((YieldTypes)iYield).getChar()).c_str());
		setListHelp(szHelpString, szStart.GetCString(), getLinkText(szName).GetCString(), L", ", (iChange != iLast));
		iLast = iChange;
	}
}

// The parts of parseTraits (added by this mod), in the order of the text.
void CvGameTextMgr::parseTraitCivicEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szText;
	int iI;

	// Civic-like modifiers. The civic text keys start with [ICON_BULLET] and no newline,
	// so the newline and the trait indent are added here.
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_FREE_XP", kTrait.getFreeExperience());
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_MILITARY_PRODUCTION", kTrait.getMilitaryProductionModifier());

	if (kTrait.getImprovementUpgradeRateModifier() != 0)
	{
		bool bFirstImprovement = true;
		for (iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
		{
			if (GC.getImprovementInfo((ImprovementTypes)iI).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_CIVIC_IMPROVEMENT_UPGRADE", kTrait.getImprovementUpgradeRateModifier()).c_str());
				szText = getLinkText(GC.getImprovementInfo((ImprovementTypes)iI).getDescription());
				setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstImprovement);
				bFirstImprovement = false;
			}
		}
	}

	if (kTrait.getDistanceMaintenanceModifier() != 0)
	{
		if (kTrait.getDistanceMaintenanceModifier() <= -100)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_DISTANCE_MAINT"));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_DISTANCE_MAINT_MOD", kTrait.getDistanceMaintenanceModifier()));
		}
	}

	if (kTrait.getNumCitiesMaintenanceModifier() != 0)
	{
		if (kTrait.getNumCitiesMaintenanceModifier() <= -100)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_NO_MAINT_NUM_CITIES"));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_NO_MAINT_NUM_CITIES_MOD", kTrait.getNumCitiesMaintenanceModifier()));
		}
	}

	if (kTrait.getWarWearinessModifier() != 0)
	{
		if (kTrait.getWarWearinessModifier() <= -100)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_NO_WAR_WEARINESS"));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_EXTRA_WAR_WEARINESS", kTrait.getWarWearinessModifier()));
		}
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_FREE_SPECIALISTS", kTrait.getFreeSpecialist());
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_TRADE_ROUTES", kTrait.getTradeRoutes());
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_EXPERIENCE_IN_BORDERS", kTrait.getExpInBorderModifier());

	if (kTrait.getCityRadiusChange() != 0)
	{
		if (kTrait.getCityRadiusCultureLevel() == NO_CULTURELEVEL)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CITY_RADIUS_CHANGE", kTrait.getCityRadiusChange(), CITY_PLOTS_MAX_RADIUS));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CITY_RADIUS_CHANGE_CULTURE", kTrait.getCityRadiusChange(), CITY_PLOTS_MAX_RADIUS, GC.getCultureLevelInfo((CultureLevelTypes)kTrait.getCityRadiusCultureLevel()).getTextKeyWide()));
		}
	}
}

// The conditions of an item of ConditionalFreeTraits as text: "at war, Buddhism is the state religion" (added by this mod).
// bShowRecord: with the record and the favorite civic of eActivePlayer. iSkipCivic: a PrereqCivic not to name (the help of
// that civic lists the item)
CvWString getConditionalFreeTraitConditions(const ConditionalFreeTrait& kCondition, bool bShowRecord, PlayerTypes eActivePlayer, int iSkipCivic)
{
	CvWString szConditions;
	CvWString szText;

	if (kCondition.bAtWar)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_AT_WAR"));
	}
	if (kCondition.bAtPeace)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_AT_PEACE"));
	}
	if (kCondition.iStateReligion != NO_RELIGION)
	{
		szText = getLinkText(GC.getReligionInfo((ReligionTypes)kCondition.iStateReligion).getDescription());
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_STATE_RELIGION", szText.GetCString()));
	}
	if (kCondition.bNoStateReligion)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_NO_STATE_RELIGION"));
	}
	if (kCondition.iEraMin != NO_ERA && kCondition.iEraMin == kCondition.iEraMax)
	{
		appendConditionText(szConditions, GC.getEraInfo((EraTypes)kCondition.iEraMin).getDescription());
	}
	else if (kCondition.iEraMin != NO_ERA && kCondition.iEraMax != NO_ERA)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_ERA_RANGE", GC.getEraInfo((EraTypes)kCondition.iEraMin).getTextKeyWide(), GC.getEraInfo((EraTypes)kCondition.iEraMax).getTextKeyWide()));
	}
	else if (kCondition.iEraMin != NO_ERA)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_ERA_MIN", GC.getEraInfo((EraTypes)kCondition.iEraMin).getTextKeyWide()));
	}
	else if (kCondition.iEraMax != NO_ERA)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_ERA_MAX", GC.getEraInfo((EraTypes)kCondition.iEraMax).getTextKeyWide()));
	}
	if (kCondition.iPrereqCivic != NO_CIVIC && kCondition.iPrereqCivic != iSkipCivic)
	{
		szText = getLinkText(GC.getCivicInfo((CivicTypes)kCondition.iPrereqCivic).getDescription());
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_CIVIC", szText.GetCString()));
	}
	// PrereqTech (added by this mod)
	if (kCondition.iPrereqTech != NO_TECH)
	{
		szText = getLinkText(GC.getTechInfo((TechTypes)kCondition.iPrereqTech).getDescription());
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_TECH", szText.GetCString()));
	}
	if (kCondition.bFavoriteCivic)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_FAVORITE_CIVIC"));
		if (bShowRecord && GC.getLeaderHeadInfo(GET_PLAYER(eActivePlayer).getLeaderType()).getFavoriteCivic() != NO_CIVIC)
		{
			szText = getLinkText(GC.getCivicInfo((CivicTypes)GC.getLeaderHeadInfo(GET_PLAYER(eActivePlayer).getLeaderType()).getFavoriteCivic()).getDescription());
			szConditions += gDLL->getText("TXT_KEY_CONDITION_FAVORITE_CIVIC_NOW", szText.GetCString());
		}
	}
	if (kCondition.iMinUnitsKilled > 0)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_UNITS_KILLED", kCondition.iMinUnitsKilled));
		if (bShowRecord)
		{
			szConditions += gDLL->getText("TXT_KEY_CONDITION_RECORD_NOW", GET_PLAYER(eActivePlayer).getNumUnitsKilled());
		}
	}
	if (kCondition.iMinCitiesConquered > 0)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_CITIES_CONQUERED", kCondition.iMinCitiesConquered));
		if (bShowRecord)
		{
			szConditions += gDLL->getText("TXT_KEY_CONDITION_RECORD_NOW", GET_PLAYER(eActivePlayer).getNumCitiesConquered());
		}
	}
	// bGoldenAge, bNoGoldenAge, bAnarchy, bNoAnarchy (added by this mod)
	if (kCondition.bGoldenAge)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_GOLDEN_AGE"));
	}
	if (kCondition.bNoGoldenAge)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_NO_GOLDEN_AGE"));
	}
	if (kCondition.bAnarchy)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_ANARCHY"));
	}
	if (kCondition.bNoAnarchy)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_NO_ANARCHY"));
	}
	// iMinCities, iMaxCities (added by this mod)
	if (kCondition.iMinCities > 0 || kCondition.iMaxCities > 0)
	{
		if (kCondition.iMinCities > 0 && kCondition.iMaxCities > 0)
		{
			appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_CITIES_RANGE", kCondition.iMinCities, kCondition.iMaxCities));
		}
		else if (kCondition.iMinCities > 0)
		{
			appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_MIN_CITIES", kCondition.iMinCities));
		}
		else
		{
			appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_MAX_CITIES", kCondition.iMaxCities));
		}
		if (bShowRecord)
		{
			szConditions += gDLL->getText("TXT_KEY_CONDITION_RECORD_NOW", GET_PLAYER(eActivePlayer).getNumCities());
		}
	}
	// PrereqBonus, PrereqBuildingClass (the default building of the class) (added by this mod)
	if (kCondition.iPrereqBonus != NO_BONUS)
	{
		szText = getLinkText(GC.getBonusInfo((BonusTypes)kCondition.iPrereqBonus).getDescription());
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_BONUS", szText.GetCString()));
	}
	if (kCondition.iPrereqBuildingClass != NO_BUILDINGCLASS)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)kCondition.iPrereqBuildingClass).getDefaultBuildingIndex();
		if (eBuilding != NO_BUILDING)
		{
			szText = getLinkText(GC.getBuildingInfo(eBuilding).getDescription());
		}
		else
		{
			szText = GC.getBuildingClassInfo((BuildingClassTypes)kCondition.iPrereqBuildingClass).getDescription();
		}
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_BUILDING", szText.GetCString()));
	}
	// bVassal, bHasVassal, iMinScoreRank (added by this mod)
	if (kCondition.bVassal)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_VASSAL"));
	}
	if (kCondition.bHasVassal)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_HAS_VASSAL"));
	}
	if (kCondition.iMinScoreRank > 0)
	{
		appendConditionText(szConditions, gDLL->getText("TXT_KEY_CONDITION_SCORE_RANK", kCondition.iMinScoreRank));
		if (bShowRecord)
		{
			szConditions += gDLL->getText("TXT_KEY_CONDITION_RECORD_NOW", GC.getGameINLINE().getPlayerRank(eActivePlayer) + 1);
		}
	}
	// PyCondition: its PyConditionText, or a generic text (added by this mod)
	if (!kCondition.szPyCondition.empty())
	{
		appendConditionText(szConditions, gDLL->getText(kCondition.szPyConditionText.empty() ? "TXT_KEY_CONDITION_PYTHON" : kCondition.szPyConditionText.GetCString()));
	}

	return szConditions;
}

void CvGameTextMgr::parseTraitConditionalFreeTraits(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szText;
	int iI;

	// The record so far and the favorite civic, for a trait of the player who reads this. Only in a game: on the screen that
	// chooses the civilization there is an active player whose leader is NO_LEADER, and hasTrait() cannot take that
	PlayerTypes eActivePlayer = GC.getGameINLINE().getActivePlayer();
	bool bShowRecord = (GC.getGameINLINE().isFinalInitialized() && eActivePlayer != NO_PLAYER && GET_PLAYER(eActivePlayer).getLeaderType() != NO_LEADER && GET_PLAYER(eActivePlayer).hasTrait(eTrait));

	// ConditionalFreeTraits: "Gains Aggressive (at war, Buddhism is the state religion)" (added by this mod)
	for (iI = 0; iI < kTrait.getNumConditionalFreeTraits(); iI++)
	{
		const ConditionalFreeTrait& kCondition = kTrait.getConditionalFreeTrait(iI);
		CvWString szConditions = getConditionalFreeTraitConditions(kCondition, bShowRecord, eActivePlayer, NO_CIVIC);

		szText = getLinkText(GC.getTraitInfo((TraitTypes)kCondition.iTrait).getDescription());
		if (szConditions.empty())
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CONDITIONAL_FREE_TRAIT_ALWAYS", szText.GetCString()));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CONDITIONAL_FREE_TRAIT", szText.GetCString(), szConditions.GetCString()));
		}
	}
}

// The part of the trait help for the cities and their founding (iMaxCities, new cities, occupation, iMinCityRangeChange, bFoundInForeignTerritory) (added by this mod)
void CvGameTextMgr::parseTraitCityFoundingEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

	// iMaxCities / iMaxCitiesPercent: "Can found up to 5 cities" on the map size of the game (added by this mod)
	if (kTrait.getMaxCitiesLimit(GC.getMapINLINE().getWorldSize()) >= 0)
	{
		appendTraitLine(szHelpString, gDLL->getText(((kTrait.getMaxCitiesPercent() != 0) ? "TXT_KEY_TRAIT_MAX_CITIES_MAP" : "TXT_KEY_TRAIT_MAX_CITIES"), kTrait.getMaxCitiesLimit(GC.getMapINLINE().getWorldSize())));
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_NEW_CITY_POPULATION", kTrait.getNewCityPopulation());

	// Regions (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_NEW_WORLD_CITY_POPULATION", kTrait.getNewWorldCityPopulation());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_OVERSEAS_CITY_POPULATION", kTrait.getOverseasCityPopulation());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_OVERSEAS_MAINTENANCE", kTrait.getOverseasMaintenanceModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_DISTANT_TRADE_ROUTE", kTrait.getDistantTradeRouteModifier());

	// iTechCostModifier, iAttitudeChange (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_TECH_COST", kTrait.getTechCostModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_ATTITUDE_CHANGE", kTrait.getAttitudeChange());

	// iAnarchyGoldenAgePercent (added by this mod)
	if (kTrait.getAnarchyGoldenAgePercent() > 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_ANARCHY_GOLDEN_AGE", kTrait.getAnarchyGoldenAgePercent()));
	}

	// EnemyAttrition/iPercent, EnemyAttrition/iLimit (added by this mod)
	if (kTrait.getEnemyAttritionInBorders() > 0)
	{
		if (kTrait.getEnemyAttritionLimit() < 100)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_ENEMY_ATTRITION_LIMIT", kTrait.getEnemyAttritionInBorders(), std::max(0, 100 - kTrait.getEnemyAttritionLimit())));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_ENEMY_ATTRITION", kTrait.getEnemyAttritionInBorders()));
		}
	}

	appendTraitFlagLine(szHelpString, "TXT_KEY_NEW_CITY_STATE_RELIGION", kTrait.isNewCityStateReligion());

	appendTraitValueLine(szHelpString, "TXT_KEY_OCCUPATION_TIME_MODIFIER", kTrait.getOccupationTimeModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONQUEST_CULTURE", kTrait.getConquestCulturePercent());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONQUEST_BUILDING_SURVIVE", kTrait.getConquestBuildingSurviveModifier());

	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_NO_CONQUEST_POPULATION_LOSS", kTrait.isNoConquestPopulationLoss());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_NO_AUTO_RAZE", kTrait.isNoAutoRaze());

	// bConquestLearnUniqueUnits and bConquestLearnUniqueBuildings (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_CONQUEST_LEARN_UNIQUE_UNITS", kTrait.isConquestLearnUniqueUnits());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_CONQUEST_LEARN_UNIQUE_BUILDINGS", kTrait.isConquestLearnUniqueBuildings());

	// Batch of 2026-11-17 (added by this mod). iStateReligionHappiness uses the civic text key
	if (kTrait.getStateReligionHappiness() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_RELIGION_HAPPINESS", abs(kTrait.getStateReligionHappiness()), ((kTrait.getStateReligionHappiness() > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR))));
	}

	if (kTrait.getStateReligionHealth() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_STATE_RELIGION_HEALTH", abs(kTrait.getStateReligionHealth()), ((kTrait.getStateReligionHealth() > 0) ? gDLL->getSymbolID(HEALTHY_CHAR) : gDLL->getSymbolID(UNHEALTHY_CHAR))));
	}

	// iStateReligionGreatPeopleRateModifier, iStateReligionUnitProductionModifier, iStateReligionBuildingProductionModifier,
	// iStateReligionFreeExperience (added by this mod): the civic text keys
	if (kTrait.getStateReligionGreatPeopleRateModifier() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_GREAT_PEOPLE_MOD_STATE_RELIGION", kTrait.getStateReligionGreatPeopleRateModifier(), gDLL->getSymbolID(RELIGION_CHAR)));
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_STATE_REL_TRAIN_BONUS", kTrait.getStateReligionUnitProductionModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_STATE_REL_BUILDING_BONUS", kTrait.getStateReligionBuildingProductionModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_STATE_REL_FREE_XP", kTrait.getStateReligionFreeExperience());

	if (kTrait.isAnyGreatPeopleClassRateModifier())
	{
		for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
		{
			if (kTrait.getGreatPeopleClassRateModifier(iUnitClass) != 0)
			{
				appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_GREAT_PEOPLE_CLASS_RATE", kTrait.getGreatPeopleClassRateModifier(iUnitClass), GC.getUnitClassInfo((UnitClassTypes)iUnitClass).getTextKeyWide()));
			}
		}
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_ENEMY_MOVE_COST_IN_BORDERS", kTrait.getEnemyMoveCostInBorders());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_REVENGE_COMBAT", kTrait.getRevengeCombatPercent());

	if (kTrait.getKnownTechCostModifier() != 0)
	{
		if (kTrait.getKnownTechCostLimit() != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_KNOWN_TECH_COST_LIMIT", kTrait.getKnownTechCostModifier(), kTrait.getKnownTechCostLimit()));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_KNOWN_TECH_COST", kTrait.getKnownTechCostModifier()));
		}
	}

	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_CONQUEST_HEAL", kTrait.isConquestHeal());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_STACK_CITY_DEFENSE", kTrait.isStackCityDefense());

	// iMinCityRangeChange: the range of this trait alone (added by this mod)
	if (kTrait.getMinCityRangeChange() != 0)
	{
		int iRange = std::max(0, GC.getMIN_CITY_RANGE() + kTrait.getMinCityRangeChange());
		if (iRange == 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_MIN_CITY_RANGE_ZERO", GC.getMIN_CITY_RANGE()));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_MIN_CITY_RANGE", iRange, GC.getMIN_CITY_RANGE()));
		}
	}

	// bFoundInForeignTerritory (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_FOUND_IN_FOREIGN_TERRITORY", kTrait.isFoundInForeignTerritory());
}

// The part of the trait help for spreads and units (iReligionSpreadChange, golden age moves, CityFoundedUnits) (added by this mod)
void CvGameTextMgr::parseTraitSpreadAndUnitEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szText;
	int iI;

	// iReligionSpreadChange / iCorporationSpreadChange (added by this mod)
	if (kTrait.getReligionSpreadChange() >= 100)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_RELIGION_SPREAD_ALWAYS"));
	}
	else if (kTrait.getReligionSpreadChange() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_RELIGION_SPREAD_CHANGE", kTrait.getReligionSpreadChange()));
	}
	if (kTrait.getCorporationSpreadChange() >= 100)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CORPORATION_SPREAD_ALWAYS"));
	}
	else if (kTrait.getCorporationSpreadChange() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CORPORATION_SPREAD_CHANGE", kTrait.getCorporationSpreadChange()));
	}

	// iGoldenAgeLandMovesChange / iGoldenAgeSeaMovesChange (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_GOLDEN_AGE_LAND_MOVES", kTrait.getGoldenAgeLandMovesChange());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_GOLDEN_AGE_SEA_MOVES", kTrait.getGoldenAgeSeaMovesChange());

	// GoldenAgeUnitClasses (added by this mod): "Great General can start a golden age"
	if (kTrait.getNumGoldenAgeUnitClasses() > 0)
	{
		szText.clear();
		for (iI = 0; iI < kTrait.getNumGoldenAgeUnitClasses(); iI++)
		{
			if (iI > 0)
			{
				szText += L", ";
			}
			szText += getLinkText(GC.getUnitClassInfo((UnitClassTypes)kTrait.getGoldenAgeUnitClass(iI)).getDescription());
		}
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_GOLDEN_AGE_UNIT_CLASSES", szText.GetCString()));
	}

	// iGoldenAgeUnitsChange (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_GOLDEN_AGE_UNITS_CHANGE", kTrait.getGoldenAgeUnitsChange());

	// CityFoundedUnits (added by this mod): "Each city founded gets 1 Galley (cities on the coast only)"
	for (iI = 0; iI < kTrait.getNumCityFoundedUnits(); iI++)
	{
		int iFoundedUnit = GC.getUnitClassInfo((UnitClassTypes)kTrait.getCityFoundedUnitClass(iI)).getDefaultUnitIndex();
		if (iFoundedUnit == NO_UNIT)
		{
			continue;
		}
		szText = getLinkText(GC.getUnitInfo((UnitTypes)iFoundedUnit).getDescription());
		szTempBuffer = gDLL->getText("TXT_KEY_TRAIT_CITY_FOUNDED_UNIT", szText.GetCString(), kTrait.getCityFoundedUnitNum(iI));
		if (kTrait.isCityFoundedUnitCoastalOnly(iI) || GC.getUnitInfo((UnitTypes)iFoundedUnit).getDomainType() == DOMAIN_SEA)
		{
			szTempBuffer += gDLL->getText("TXT_KEY_TRAIT_CITY_FOUNDED_UNIT_COASTAL");
		}
		appendTraitLine(szHelpString, szTempBuffer);
	}
}

// The part of the trait help for the territory (start bias, bBarbarianAlly, bInlandCanals, global warming, pillage) (added by this mod)
void CvGameTextMgr::parseTraitTerritoryEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

	// Start bias (added by this mod)
	CvWString szStartBias = getStartBiasText(kTrait);
	if (!szStartBias.empty())
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_START_BIAS", szStartBias.GetCString()));
	}

	// SeaStart (added by this mod): "Starts at sea with the starting units on Galleons (...)"
	if (kTrait.isSeaStart())
	{
		int iShip = NO_UNIT;
		if (kTrait.getSeaStartUnitClass() != NO_UNITCLASS)
		{
			iShip = GC.getUnitClassInfo((UnitClassTypes)kTrait.getSeaStartUnitClass()).getDefaultUnitIndex();
		}
		if (iShip != NO_UNIT)
		{
			CvWString szShip = getLinkText(GC.getUnitInfo((UnitTypes)iShip).getDescription());
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_SEA_START", szShip.GetCString()));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_SEA_START_NO_SHIP"));
		}
	}

	// FoundingVassal (added by this mod): "When founding the first city, Rome led by Caesar joins the game within 5 tiles of
	// it ...", then the difficulty of its AI modifiers
	if (kTrait.isFoundingVassal())
	{
		CvWString szCiv;
		if (kTrait.getFoundingVassalCivilization() != NO_CIVILIZATION)
		{
			szCiv = getLinkText(GC.getCivilizationInfo((CivilizationTypes)kTrait.getFoundingVassalCivilization()).getShortDescription());
		}
		else
		{
			szCiv = gDLL->getText("TXT_KEY_TRAIT_FOUNDING_VASSAL_ANY_CIV");
		}
		if (kTrait.getFoundingVassalLeader() != NO_LEADER)
		{
			CvWString szLeader = getLinkText(GC.getLeaderHeadInfo((LeaderHeadTypes)kTrait.getFoundingVassalLeader()).getDescription());
			szCiv = gDLL->getText("TXT_KEY_TRAIT_FOUNDING_VASSAL_LED_BY", szCiv.GetCString(), szLeader.GetCString());
		}
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_FOUNDING_VASSAL", szCiv.GetCString(), std::max(1, kTrait.getFoundingVassalRange())));
		if (kTrait.getFoundingVassalHandicap() != NO_HANDICAP)
		{
			CvWString szHandicapLine = L"  ";
			szHandicapLine += gDLL->getText("TXT_KEY_TRAIT_FOUNDING_VASSAL_HANDICAP", GC.getHandicapInfo((HandicapTypes)kTrait.getFoundingVassalHandicap()).getTextKeyWide());
			appendTraitLine(szHelpString, szHandicapLine);
		}
	}

	// bBarbarianAlly (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_BARBARIAN_ALLY", kTrait.isBarbarianAlly());

	// bBarbarianVision (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_BARBARIAN_VISION", kTrait.isBarbarianVision());

	// PassableTerrainsInBorders / PassableFeaturesInBorders and FeatureHealChanges (added by this mod)
	{
		CvWString szNames;
		int iK;

		for (iK = 0; iK < GC.getNumTerrainInfos(); ++iK)
		{
			if (kTrait.isPassableTerrainInBorders(iK))
			{
				szNames += (szNames.empty() ? L"" : L", ") + getLinkText(GC.getTerrainInfo((TerrainTypes)iK).getDescription());
			}
		}

		for (iK = 0; iK < GC.getNumFeatureInfos(); ++iK)
		{
			if (kTrait.isPassableFeatureInBorders(iK))
			{
				szNames += (szNames.empty() ? L"" : L", ") + getLinkText(GC.getFeatureInfo((FeatureTypes)iK).getDescription());
			}
		}

		if (!szNames.empty())
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_PASSABLE_IN_BORDERS", szNames.GetCString()));
		}

		for (iK = 0; iK < GC.getNumFeatureInfos(); ++iK)
		{
			if (kTrait.getFeatureHealChange(iK) != 0)
			{
				appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_FEATURE_HEAL", kTrait.getFeatureHealChange(iK), getLinkText(GC.getFeatureInfo((FeatureTypes)iK).getDescription()).GetCString()));
			}
		}
	}

	// bInlandCanals (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_INLAND_CANALS", kTrait.isInlandCanals());

	// bNoGlobalWarmingInBorders (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_NO_GLOBAL_WARMING_IN_BORDERS", kTrait.isNoGlobalWarmingInBorders());

	// bNoPillageInBorders (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_NO_PILLAGE_IN_BORDERS", kTrait.isNoPillageInBorders());
}

// The part of the trait help for the economy and the rest (interest, culture range, peace anger, ForbiddenCivics, costs, Civ5 abilities) (added by this mod)
void CvGameTextMgr::parseTraitEconomyEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szText;
	int iI;

	// GoldInterest/iPercent / GoldInterest/iMax: "3% of the treasury as gold every turn (at most 25)" (added by this mod)
	if (kTrait.getGoldInterestPercent() > 0)
	{
		if (kTrait.getGoldInterestMax() > 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_GOLD_INTEREST_MAX", kTrait.getGoldInterestPercent(), kTrait.getGoldInterestMax()));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_GOLD_INTEREST", kTrait.getGoldInterestPercent()));
		}
	}

	// iCultureRangeChange: "+1 culture range of all cities" (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CULTURE_RANGE_CHANGE", kTrait.getCultureRangeChange());

	// Revolts and flips by culture (added by this mod)
	if (kTrait.isNoRevoltAndNoCultureFlip())
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_NO_REVOLT_AND_NO_CULTURE_FLIP"));
	}
	else if (kTrait.isNoCultureFlip())
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_NO_CULTURE_FLIP"));
	}

	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_INSTANT_CULTURE_FLIP", kTrait.isInstantCultureFlip());

	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CULTURE_REVOLT_CHANCE", kTrait.getCultureRevoltChance());

	// PeaceAnger/iPerTurn: "Cities grow angry in peace" (added by this mod)
	if (kTrait.getPeaceAngerPerTurn() > 0 && kTrait.getPeaceAngerMax() > 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_PEACE_ANGER", getPerMilleText(kTrait.getPeaceAngerPerTurn()).GetCString(), getPerMilleText(kTrait.getPeaceAngerMax()).GetCString(), getPerMilleText(kTrait.getPeaceAngerDecayPerTurn()).GetCString()));
	}

	// ForbiddenCivics: "Cannot adopt: Slavery, Serfdom" (added by this mod)
	bool bFirstCivic = true;
	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (kTrait.isForbiddenCivic(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_FORBIDDEN_CIVICS").c_str());
			szText = getLinkText(GC.getCivicInfo((CivicTypes)iI).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstCivic);
			bFirstCivic = false;
		}
	}

	// NoTechCivics: "Can adopt without the tech: Caste System" (added by this mod)
	bFirstCivic = true;
	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (kTrait.isNoTechCivic(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_NO_TECH_CIVICS").c_str());
			szText = getLinkText(GC.getCivicInfo((CivicTypes)iI).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstCivic);
			bFirstCivic = false;
		}
	}

	// CivicOptionExtraSlots: "Can adopt 1 more civics of Legal at the same time" (added by this mod)
	for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (kTrait.getCivicOptionExtraSlots(iI) > 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CIVIC_OPTION_EXTRA_SLOTS", GC.getCivicOptionInfo((CivicOptionTypes)iI).getTextKeyWide(), kTrait.getCivicOptionExtraSlots(iI)));
		}
	}

	// iRouteMovementModifierInBorders, bConquestStateReligion, bConquestReligionBuildings (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_ROUTE_MOVEMENT_IN_BORDERS", kTrait.getRouteMovementModifierInBorders());

	// iCombatPercent (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_COMBAT_PERCENT", kTrait.getCombatPercent());

	// iHappinessPerReligion, CommercePerReligion, iDefenseCombatPercent, iEnemyWarWearinessModifier (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_HAPPINESS_PER_RELIGION", kTrait.getHappinessPerReligion());

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (kTrait.getCommercePerReligion(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_COMMERCE_PER_RELIGION", kTrait.getCommercePerReligion(iI), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_DEFENSE_COMBAT_PERCENT", kTrait.getDefenseCombatPercent());

	// AttitudeCombatPercents (added by this mod): from furious to friendly, then the cap
	for (iI = 0; iI < NUM_ATTITUDE_TYPES; iI++)
	{
		if (kTrait.getAttitudeCombatPercent(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_ATTITUDE_COMBAT_PERCENT", GC.getAttitudeInfo((AttitudeTypes)iI).getTextKeyWide(), kTrait.getAttitudeCombatPercent(iI)));
		}
	}
	if (kTrait.isAttitudeCombat() && kTrait.getAttitudeCombatMaxPercent() >= 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_ATTITUDE_COMBAT_MAX_PERCENT", kTrait.getAttitudeCombatMaxPercent()));
	}

	// WorldWonderCommerceChanges, GlobalWorldWonderCommerceChanges (added by this mod)
	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (kTrait.getWorldWonderCommerceChange(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_PER_CITY_WORLD_WONDER", kTrait.getWorldWonderCommerceChange(iI), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (kTrait.getGlobalWorldWonderCommerceChange(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_PER_WORLD_WONDER", kTrait.getGlobalWorldWonderCommerceChange(iI), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}
	}

	// NearbyCityCommerce (added by this mod): the values of the XML only, so it also works before the game starts
	if (kTrait.getNearbyCityRange() > 0)
	{
		for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			if (kTrait.getNearbyCityCommerce(iI) != 0)
			{
				appendTraitLine(szHelpString, gDLL->getText((kTrait.isNearbyCityDoubleInCapital() ? "TXT_KEY_TRAIT_NEARBY_CITY_COMMERCE_CAPITAL" : "TXT_KEY_TRAIT_NEARBY_CITY_COMMERCE"), kTrait.getNearbyCityCommerce(iI), GC.getCommerceInfo((CommerceTypes)iI).getChar(), kTrait.getNearbyCityRange()));
			}
		}
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_ENEMY_WAR_WEAR", kTrait.getEnemyWarWearinessModifier());

	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_CONQUEST_STATE_RELIGION", kTrait.isConquestStateReligion());

	// ConquestReligionBuildings: "Conquered cities get (when they could build it): Temple, Cathedral". Special buildings have
	// no Civilopedia page: no link
	bool bFirstConquestBuilding = true;
	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		if (kTrait.isConquestReligionBuilding(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_CONQUEST_RELIGION_BUILDINGS").c_str());
			setListHelp(szHelpString, szTempBuffer.GetCString(), GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).getDescription(), L", ", bFirstConquestBuilding);
			bFirstConquestBuilding = false;
		}
	}

	// iMaxNationalWondersPerCity (ForOCC): "National wonders per city: 4" / "no limit" (added by this mod)
	if (kTrait.getMaxNationalWondersPerCity() != 0)
	{
		szText = (kTrait.getMaxNationalWondersPerCity() == -1) ? gDLL->getText("TXT_KEY_TRAIT_NO_LIMIT") : CvWString::format(L"%d", kTrait.getMaxNationalWondersPerCity());
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_MAX_NATIONAL_WONDERS", szText.GetCString()));
	}

	if (kTrait.getMaxNationalWondersPerCityForOCC() != 0)
	{
		szText = (kTrait.getMaxNationalWondersPerCityForOCC() == -1) ? gDLL->getText("TXT_KEY_TRAIT_NO_LIMIT") : CvWString::format(L"%d", kTrait.getMaxNationalWondersPerCityForOCC());
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_MAX_NATIONAL_WONDERS_OCC", szText.GetCString()));
	}

	// bHumanCanBeVassal (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_HUMAN_CAN_BE_VASSAL", kTrait.isHumanCanBeVassal());

	// bGainTraitOnCapitalCapture / bGainTraitsOnElimination (added by this mod)
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_GAIN_TRAIT_ON_CAPITAL_CAPTURE", kTrait.isGainTraitOnCapitalCapture());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_GAIN_TRAITS_ON_ELIMINATION", kTrait.isGainTraitsOnElimination());

	// GainTraitsOnFriendly (added by this mod)
	if (kTrait.isGainTraitsOnFriendly())
	{
		appendTraitLine(szHelpString, gDLL->getText(kTrait.isGainTraitsOnFriendlyAll() ? "TXT_KEY_TRAIT_GAIN_TRAITS_ON_FRIENDLY_ALL" : "TXT_KEY_TRAIT_GAIN_TRAIT_ON_FRIENDLY"));
	}

	// RandomBasicTraits: "Every 5 turns, gains 2 random basic traits in place of the last ones" (added by this mod). The turns
	// are scaled by the game speed when it is known
	if (kTrait.isRandomBasicTraits() && kTrait.getRandomBasicTraitCount() > 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_RANDOM_BASIC_TRAITS", CvPlayer::getRandomBasicTraitTurnsForSpeed(kTrait.getRandomBasicTraitTurns()), kTrait.getRandomBasicTraitCount()));
	}

	// iUnitCostModifier / iBuildingCostModifier: "-20% production cost of units" (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_UNIT_COST_MODIFIER", kTrait.getUnitCostModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_BUILDING_COST_MODIFIER", kTrait.getBuildingCostModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_SUMMON_DURATION", kTrait.getSummonDuration());

	// Civ5 abilities (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_GOLDENAGE_MOD", kTrait.getGoldenAgeModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_GOLDEN_AGE_GREAT_PEOPLE", kTrait.getGoldenAgeGreatPeopleRateModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CAPITAL_BUILDING_PRODUCTION", kTrait.getCapitalBuildingProductionModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_PILLAGE_GOLD", kTrait.getPillageGoldModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CITY_CAPTURE_GOLD", kTrait.getCityCaptureGoldModifier());

	if (kTrait.getNewCityCultureLevel() != NO_CULTURELEVEL)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_NEW_CITY_CULTURE_LEVEL", GC.getCultureLevelInfo((CultureLevelTypes)kTrait.getNewCityCultureLevel()).getTextKeyWide()));
	}
}

void CvGameTextMgr::parseTraitUnitCombatEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	int iI;

	// UnitCombatFreeExperiences / UnitCombatProductionModifiers (building text keys)
	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kTrait.getUnitCombatFreeExperience(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_FREE_XP", GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide(), kTrait.getUnitCombatFreeExperience(iI)));
		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kTrait.getUnitCombatProductionModifier(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_BUILDS_FASTER_DOMAIN", GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide(), kTrait.getUnitCombatProductionModifier(iI)));
		}
	}

	// UnitCombatUpgradeDiscounts: shown as a cost change, so a 50% discount reads "-50%"
	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kTrait.getUnitCombatUpgradeDiscount(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_UNITCOMBAT_UPGRADE_COST_MODIFIER", GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide(), -kTrait.getUnitCombatUpgradeDiscount(iI)));
		}
	}

	// UpgradeAnywhereUnitCombats
	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		if (kTrait.isUpgradeAnywhereUnitCombat(iI))
		{
			appendTraitLine(szHelpString, gDLL->getText(kTrait.isUpgradeAnywhereExcludeEnemy(iI) ? "TXT_KEY_UNITCOMBAT_UPGRADE_ANYWHERE_NOT_ENEMY" : "TXT_KEY_UNITCOMBAT_UPGRADE_ANYWHERE", GC.getUnitCombatInfo((UnitCombatTypes)iI).getTextKeyWide()));
		}
	}
}

void CvGameTextMgr::parseTraitYieldEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	int iLast;
	int iI, iJ;

	// Yield / commerce modifiers: bNewLine=false and the newline, indent and bullet passed as szStart,
	// so nothing is written when all values are zero
	szTempBuffer.Format(L"%s  %c", NEWLINE, gDLL->getSymbolID(BULLET_CHAR));
	setYieldChangeHelp(szHelpString, szTempBuffer, L"", gDLL->getText("TXT_KEY_CIVIC_IN_ALL_CITIES"), kTrait.getYieldModifierArray(), true, false);
	setYieldChangeHelp(szHelpString, szTempBuffer, L"", gDLL->getText("TXT_KEY_CIVIC_IN_CAPITAL"), kTrait.getCapitalYieldModifierArray(), true, false);
	setCommerceChangeHelp(szHelpString, szTempBuffer, L"", gDLL->getText("TXT_KEY_CIVIC_IN_CAPITAL"), kTrait.getCapitalCommerceModifierArray(), true, false);
	setCommerceChangeHelp(szHelpString, szTempBuffer, L"", gDLL->getText("TXT_KEY_CIVIC_PER_SPECIALIST"), kTrait.getSpecialistExtraCommerceArray(), false, false);

	// SpecialistYieldChanges / SpecialistCommerceChanges: "Scientist: +1 food, +2 gold" (added by this mod)
	for (iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
	{
		CvWString szStart;
		szStart.Format(L"%s  %c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), getLinkText(GC.getSpecialistInfo((SpecialistTypes)iI).getDescription()).GetCString());
		if (kTrait.getSpecialistYieldChangeArray(iI) != NULL)
		{
			setYieldChangeHelp(szHelpString, szStart, L": ", L"", kTrait.getSpecialistYieldChangeArray(iI), false, false);
		}
		if (kTrait.getSpecialistCommerceChangeArray(iI) != NULL)
		{
			setCommerceChangeHelp(szHelpString, szStart, L": ", L"", kTrait.getSpecialistCommerceChangeArray(iI), false, false);
		}
	}

	// TerrainYieldChanges / FeatureYieldChanges: one line per yield and value, like the civic improvement yields
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		iLast = 0;

		for (iJ = 0; iJ < GC.getNumTerrainInfos(); ++iJ)
		{
			appendTraitPlotYieldChange(szHelpString, kTrait.getTerrainYieldChange(iJ, iI), iI, GC.getTerrainInfo((TerrainTypes)iJ).getDescription(), iLast);
		}

		for (iJ = 0; iJ < GC.getNumFeatureInfos(); ++iJ)
		{
			appendTraitPlotYieldChange(szHelpString, kTrait.getFeatureYieldChange(iJ, iI), iI, GC.getFeatureInfo((FeatureTypes)iJ).getDescription(), iLast);
		}

		for (iJ = 0; iJ < GC.getNumImprovementInfos(); ++iJ)
		{
			appendTraitPlotYieldChange(szHelpString, kTrait.getImprovementYieldChanges(iJ, iI), iI, GC.getImprovementInfo((ImprovementTypes)iJ).getDescription(), iLast);
		}

		for (iJ = 0; iJ < GC.getNumBonusInfos(); ++iJ)
		{
			appendTraitPlotYieldChange(szHelpString, kTrait.getBonusYieldChange(iJ, iI), iI, GC.getBonusInfo((BonusTypes)iJ).getDescription(), iLast);
		}

		// TerrainYieldChangesPerEra (added by this mod): a line of its own
		iLast = 0;
		for (iJ = 0; iJ < GC.getNumTerrainInfos(); ++iJ)
		{
			appendTraitPlotYieldChange(szHelpString, kTrait.getTerrainYieldChangePerEra(iJ, iI), iI, GC.getTerrainInfo((TerrainTypes)iJ).getDescription(), iLast, "TXT_KEY_TRAIT_YIELD_CHANGE_PER_ERA");
		}
	}

	// FeatureProductionChanges, FeatureGrowthChanges, FeatureHealthChanges: one line per feature (added by this mod). The growth
	// (1/10000) and the health (1/100) are shown as decimals
	for (iJ = 0; iJ < GC.getNumFeatureInfos(); ++iJ)
	{
		CvWString szFeature = getLinkText(GC.getFeatureInfo((FeatureTypes)iJ).getDescription());
		int iValue;

		iValue = kTrait.getFeatureProductionChange(iJ);
		if (iValue != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_FEATURE_PRODUCTION_CHANGE", szFeature.GetCString(), iValue));
		}

		iValue = kTrait.getFeatureGrowthChange(iJ);
		if (iValue != 0)
		{
			szTempBuffer.Format(L"%c%d.%02d", ((iValue > 0) ? L'+' : L'-'), abs(iValue) / 100, abs(iValue) % 100);
			// The feature twice: %s1 and %s3 (getText replaces only one occurrence of an argument)
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_FEATURE_GROWTH_CHANGE", szFeature.GetCString(), szTempBuffer.GetCString(), szFeature.GetCString()));
		}

		iValue = kTrait.getFeatureHealthChange(iJ);
		if (iValue != 0)
		{
			szTempBuffer.Format(L"%c%d.%02d", ((iValue > 0) ? L'+' : L'-'), abs(iValue) / 100, abs(iValue) % 100);
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_FEATURE_HEALTH_CHANGE", szFeature.GetCString(), szTempBuffer.GetCString()));
		}
	}

	// RiverPlotYieldChanges: "River Tiles: +1 food", with the building text key
	if (kTrait.getRiverPlotYieldChangeArray() != NULL)
	{
		szTempBuffer.Format(L"%s  %c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_BUILDING_RIVER_PLOTS").c_str());
		setYieldChangeHelp(szHelpString, szTempBuffer, L": ", L"", kTrait.getRiverPlotYieldChangeArray(), false, false);
	}

	// RiverEdgeYieldPercents (added by this mod): the yield for 1, 2, 3 and 4 river edges, "+0, +0, +1, +1 food"
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		int iPercent = kTrait.getRiverEdgeYieldPercent(iI);
		if (iPercent != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_RIVER_EDGE_YIELD", iPercent / 100, (2 * iPercent) / 100, (3 * iPercent) / 100, (4 * iPercent) / 100, GC.getYieldInfo((YieldTypes)iI).getChar()));
		}
	}
}

void CvGameTextMgr::parseTraitUniqueElements(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	int iI;

	// UniqueUnits and UniqueBuildings: "Unique unit: X (instead of Y)" (added by this mod)
	for (iI = 0; iI < kTrait.getNumUniqueUnits(); iI++)
	{
		CvUnitInfo& kUnique = GC.getUnitInfo((UnitTypes)kTrait.getUniqueUnit(iI));
		int iDefault = GC.getUnitClassInfo((UnitClassTypes)kTrait.getUniqueUnitClass(iI)).getDefaultUnitIndex();
		CvWString szUnique;
		CvWString szDefault;
		szUnique = getLinkText(kUnique.getDescription());
		szDefault = getLinkText((iDefault != NO_UNIT) ? GC.getUnitInfo((UnitTypes)iDefault).getDescription() : L"");
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_UNIQUE_UNIT", szUnique.GetCString(), szDefault.GetCString()));
	}

	for (iI = 0; iI < kTrait.getNumUniqueBuildings(); iI++)
	{
		CvBuildingInfo& kUnique = GC.getBuildingInfo((BuildingTypes)kTrait.getUniqueBuilding(iI));
		int iDefault = GC.getBuildingClassInfo((BuildingClassTypes)kTrait.getUniqueBuildingClass(iI)).getDefaultBuildingIndex();
		CvWString szUnique;
		CvWString szDefault;
		szUnique = getLinkText(kUnique.getDescription());
		szDefault = getLinkText((iDefault != NO_BUILDING) ? GC.getBuildingInfo((BuildingTypes)iDefault).getDescription() : L"");
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_UNIQUE_BUILDING", szUnique.GetCString(), szDefault.GetCString()));
	}

	// Bonuses that need this trait (PrereqOrTraits): "Unique resource: X" (added by this mod)
	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		const CvBonusInfo& kBonus = GC.getBonusInfo((BonusTypes)iI);
		for (int iBonusTrait = 0; iBonusTrait < kBonus.getNumPrereqOrTraits(); iBonusTrait++)
		{
			if (kBonus.getPrereqOrTraits(iBonusTrait) == eTrait)
			{
				CvWString szBonus;
				szBonus = getLinkText(kBonus.getDescription());
				appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_UNIQUE_BONUS", szBonus.GetCString()));
			}
		}
	}

	// Civics that need this trait (PrereqOrTraits, ReplacedCivic): "Unique civic: X (instead of Y)" (added by this mod)
	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		const CvCivicInfo& kCivic = GC.getCivicInfo((CivicTypes)iI);
		for (int iCivicTrait = 0; iCivicTrait < kCivic.getNumPrereqOrTraits(); iCivicTrait++)
		{
			if (kCivic.getPrereqOrTraits(iCivicTrait) == eTrait)
			{
				CvWString szCivic;
				szCivic = getLinkText(kCivic.getDescription());
				szHelpString.append(NEWLINE L"  ");
				if (kCivic.getReplacedCivic() != NO_CIVIC)
				{
					CvWString szReplaced;
					szReplaced = getLinkText(GC.getCivicInfo((CivicTypes)kCivic.getReplacedCivic()).getDescription());
					szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_UNIQUE_CIVIC_REPLACES", szCivic.GetCString(), szReplaced.GetCString()));
				}
				else
				{
					szHelpString.append(gDLL->getText("TXT_KEY_TRAIT_UNIQUE_CIVIC", szCivic.GetCString()));
				}
			}
		}
	}

	// Projects that need this trait (PrereqOrTraits): "Unique project: X" (added by this mod)
	for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		const std::vector<int>& aiProjectTraits = GC.getProjectInfo((ProjectTypes)iI).getPrereqOrTraitsVector();
		if (std::find(aiProjectTraits.begin(), aiProjectTraits.end(), (int)eTrait) != aiProjectTraits.end())
		{
			CvWString szProject;
			szProject = getLinkText(GC.getProjectInfo((ProjectTypes)iI).getDescription());
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_UNIQUE_PROJECT", szProject.GetCString()));
		}
	}
}

void CvGameTextMgr::parseTraitMiseryAndLakeEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;

	// AngryYieldChanges and the like (added by this mod)
	buildMiseryChangesString(szHelpString, kTrait.getAngryYieldChangeArray(), kTrait.getAngryCommerceChangeArray(), kTrait.getUnhealthyYieldChangeArray(), kTrait.getUnhealthyCommerceChangeArray(), true);

	// LakePlotYieldChanges: "Lake Tiles: +1 production" (added by this mod)
	if (kTrait.getLakePlotYieldChangeArray() != NULL)
	{
		szTempBuffer.Format(L"%s  %c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_TRAIT_LAKE_PLOTS").c_str());
		setYieldChangeHelp(szHelpString, szTempBuffer, L": ", L"", kTrait.getLakePlotYieldChangeArray(), false, false);
	}

	// LakePlotYieldChangesPerEra (added by this mod)
	if (kTrait.getLakePlotYieldChangePerEraArray() != NULL)
	{
		szTempBuffer.Format(L"%s  %c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_TRAIT_LAKE_PLOTS_PER_ERA").c_str());
		setYieldChangeHelp(szHelpString, szTempBuffer, L": ", L"", kTrait.getLakePlotYieldChangePerEraArray(), false, false);
	}
}

// A value in 1/100 as "+1" or "+0.50" (added by this mod)
static CvWString formatHundredths(int iValue)
{
	CvWString szValue;

	if (iValue % 100 == 0)
	{
		szValue.Format(L"%c%d", ((iValue < 0) ? L'-' : L'+'), abs(iValue) / 100);
	}
	else
	{
		szValue.Format(L"%c%d.%02d", ((iValue < 0) ? L'-' : L'+'), abs(iValue) / 100, abs(iValue) % 100);
	}

	return szValue;
}

// The tags of 2026-11-18 (added by this mod): upgrades, combat experience, celebration, specialist slots, hurry, improved
// bonuses and the changes per extra happiness and health
void CvGameTextMgr::parseTraitUnitAndCityEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	int iValue;
	int iLast;
	int iI, iJ;

	iValue = kTrait.getMaxExperienceAfterUpgrade();
	if (iValue < 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_KEEP_UPGRADE_EXPERIENCE"));
	}
	else if (iValue > 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_MAX_UPGRADE_EXPERIENCE", iValue));
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_EXP_OUTSIDE_BORDERS", kTrait.getExpOutsideBordersChange());

	iValue = kTrait.getWeLoveTheKingChance();
	if (iValue >= 100)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_WE_LOVE_THE_KING_ALWAYS", GC.getDefineINT("WE_LOVE_THE_KING_POPULATION_MIN_POPULATION")));
	}
	else if (iValue > 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_WE_LOVE_THE_KING_CHANCE", iValue, GC.getDefineINT("WE_LOVE_THE_KING_POPULATION_MIN_POPULATION")));
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iValue = kTrait.getSpecialistSlotChange(iI);
		if (iValue != 0)
		{
			CvWString szSpecialist = getLinkText(GC.getSpecialistInfo((SpecialistTypes)iI).getDescription());
			if (iValue < 0)
			{
				appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_SPECIALIST_UNLIMITED", szSpecialist.GetCString()));
			}
			else
			{
				appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_SPECIALIST_SLOTS", szSpecialist.GetCString(), iValue));
			}
		}
	}

	// Hurrys: "Can hurry with gold without the civic (units): cost -25%, +3 experience"
	for (iI = 0; iI < kTrait.getNumHurrys(); iI++)
	{
		const TraitHurry& kHurry = kTrait.getHurry(iI);
		CvWString szTargets;
		CvWString szExtras;

		if (kHurry.bUnitsOnly)
		{
			szTargets += gDLL->getText("TXT_KEY_TRAIT_HURRY_TARGET_UNITS");
		}
		if (kHurry.bBuildingsOnly)
		{
			szTargets += (szTargets.empty() ? L"" : L", ") + gDLL->getText("TXT_KEY_TRAIT_HURRY_TARGET_BUILDINGS");
		}
		if (kHurry.bWondersOnly)
		{
			szTargets += (szTargets.empty() ? L"" : L", ") + gDLL->getText("TXT_KEY_TRAIT_HURRY_TARGET_WONDERS");
		}
		if (kHurry.bWorldWondersOnly)
		{
			szTargets += (szTargets.empty() ? L"" : L", ") + gDLL->getText("TXT_KEY_TRAIT_HURRY_TARGET_WORLD_WONDERS");
		}
		if (kHurry.bNationalWondersOnly)
		{
			szTargets += (szTargets.empty() ? L"" : L", ") + gDLL->getText("TXT_KEY_TRAIT_HURRY_TARGET_NATIONAL_WONDERS");
		}
		if (!szTargets.empty())
		{
			szTargets = gDLL->getText("TXT_KEY_TRAIT_HURRY_TARGETS", szTargets.GetCString());
		}

		if (kHurry.iCostModifier != 0)
		{
			szExtras += gDLL->getText("TXT_KEY_TRAIT_HURRY_COST", kHurry.iCostModifier);
		}
		if (kHurry.bIgnoreCostModifier)
		{
			szExtras += (szExtras.empty() ? L"" : L", ") + gDLL->getText("TXT_KEY_TRAIT_HURRY_IGNORE_COST");
		}
		if (kHurry.iExperience != 0)
		{
			szExtras += (szExtras.empty() ? L"" : L", ") + gDLL->getText("TXT_KEY_TRAIT_HURRY_EXPERIENCE", kHurry.iExperience);
		}
		if (!szExtras.empty())
		{
			szExtras = gDLL->getText("TXT_KEY_TRAIT_HURRY_EXTRAS", szExtras.GetCString());
		}

		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_HURRY", GC.getHurryInfo((HurryTypes)kHurry.iHurry).getTextKeyWide(), szTargets.GetCString(), szExtras.GetCString()));
	}

	// ImprovedBonusYieldChanges: one line per yield and value, "+3 commerce from these developed resources: gold, silver"
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		iLast = 0;

		for (iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
		{
			iValue = kTrait.getImprovedBonusYieldChange(iJ, iI);
			if (iValue != 0)
			{
				CvWString szStart;
				szStart.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_IMPROVED_BONUS_YIELD_CHANGE", iValue, GC.getYieldInfo((YieldTypes)iI).getChar()).c_str());
				setListHelp(szHelpString, szStart.GetCString(), getLinkText(GC.getBonusInfo((BonusTypes)iJ).getDescription()).GetCString(), L", ", (iValue != iLast));
				iLast = iValue;
			}
		}
	}

	// HappyYieldPercents and the like: "+0.50 food per extra happiness"
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (kTrait.getHappyYieldPercent(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_HAPPY_PERCENT", formatHundredths(kTrait.getHappyYieldPercent(iI)).GetCString(), GC.getYieldInfo((YieldTypes)iI).getChar()));
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (kTrait.getHappyCommercePercent(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_HAPPY_PERCENT", formatHundredths(kTrait.getHappyCommercePercent(iI)).GetCString(), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}
	}

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (kTrait.getHealthyYieldPercent(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_HEALTHY_PERCENT", formatHundredths(kTrait.getHealthyYieldPercent(iI)).GetCString(), GC.getYieldInfo((YieldTypes)iI).getChar()));
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (kTrait.getHealthyCommercePercent(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_HEALTHY_PERCENT", formatHundredths(kTrait.getHealthyCommercePercent(iI)).GetCString(), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}
	}
}

// The part of the trait help for the counterparts of the tags that existed for only one of happiness/health or
// yield/commerce (added by this mod). The texts of the civics and buildings, with a newline and an indent
void CvGameTextMgr::parseTraitCounterpartEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szFirstBuffer;
	CvWString szText;
	int iValue;
	int iLast;
	int iI;

	// YieldChanges and SpecialistExtraYields: "+1[food] in all cities", "+1[production] per specialist"
	szTempBuffer.Format(L"%s  %c", NEWLINE, gDLL->getSymbolID(BULLET_CHAR));
	setYieldChangeHelp(szHelpString, szTempBuffer, L"", gDLL->getText("TXT_KEY_CIVIC_IN_ALL_CITIES"), kTrait.getYieldChangeArray(), false, false);
	setYieldChangeHelp(szHelpString, szTempBuffer, L"", gDLL->getText("TXT_KEY_CIVIC_PER_SPECIALIST"), kTrait.getSpecialistExtraYieldArray(), false, false);

	// iHealthPerReligion, YieldPerReligion
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_HEALTH_PER_RELIGION", kTrait.getHealthPerReligion());

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (kTrait.getYieldPerReligion(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_COMMERCE_PER_RELIGION", kTrait.getYieldPerReligion(iI), GC.getYieldInfo((YieldTypes)iI).getChar()));
		}
	}

	// WorldWonderYieldChanges, GlobalWorldWonderYieldChanges: the texts of the commerce versions
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (kTrait.getWorldWonderYieldChange(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_PER_CITY_WORLD_WONDER", kTrait.getWorldWonderYieldChange(iI), GC.getYieldInfo((YieldTypes)iI).getChar()));
		}
	}

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (kTrait.getGlobalWorldWonderYieldChange(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_PER_WORLD_WONDER", kTrait.getGlobalWorldWonderYieldChange(iI), GC.getYieldInfo((YieldTypes)iI).getChar()));
		}
	}

	// iNonStateReligionHappiness: the text of the civics
	iValue = kTrait.getNonStateReligionHappiness();
	if (iValue != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_NON_STATE_REL_HAPPINESS_WITH_STATE", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR))));
	}

	// FeatureHappinessChanges: "+1[happy] from Forest, Jungle", as the civics
	iLast = 0;
	for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		iValue = kTrait.getFeatureHappinessChange(iI);
		if (iValue != 0)
		{
			szFirstBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_CIVIC_FEATURE_HAPPINESS", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR))).c_str());
			szText = getLinkText(GC.getFeatureInfo((FeatureTypes)iI).getDescription());
			setListHelp(szHelpString, szFirstBuffer, szText, L", ", (iValue != iLast));
			iLast = iValue;
		}
	}

	// BuildingHappinessChanges, BuildingHealthChanges: "+1[happy] from Colosseum", as the civics (the building class)
	for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		iValue = kTrait.getBuildingHappinessChange(iI);
		if (iValue != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_BUILDING_HAPPINESS", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR)), GC.getBuildingClassInfo((BuildingClassTypes)iI).getTextKeyWide()));
		}

		iValue = kTrait.getBuildingHealthChange(iI);
		if (iValue != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_BUILDING_HAPPINESS", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HEALTHY_CHAR) : gDLL->getSymbolID(UNHEALTHY_CHAR)), GC.getBuildingClassInfo((BuildingClassTypes)iI).getTextKeyWide()));
		}
	}

	// BonusHappinessChanges, BonusHealthChanges: "+1[happy] from Wine, Silk", as the buildings
	iLast = 0;
	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		iValue = kTrait.getBonusHappinessChange(iI);
		if (iValue != 0)
		{
			szFirstBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_HEALTH_HAPPINESS_CHANGE", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR))).c_str());
			szText = getLinkText(GC.getBonusInfo((BonusTypes)iI).getDescription());
			setListHelp(szHelpString, szFirstBuffer, szText, L", ", (iValue != iLast));
			iLast = iValue;
		}
	}

	iLast = 0;
	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		iValue = kTrait.getBonusHealthChange(iI);
		if (iValue != 0)
		{
			szFirstBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_BUILDING_HEALTH_HAPPINESS_CHANGE", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HEALTHY_CHAR) : gDLL->getSymbolID(UNHEALTHY_CHAR))).c_str());
			szText = getLinkText(GC.getBonusInfo((BonusTypes)iI).getDescription());
			setListHelp(szHelpString, szFirstBuffer, szText, L", ", (iValue != iLast));
			iLast = iValue;
		}
	}

	// Trade routes and defense: the building text keys
	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_COASTAL_TRADE_ROUTES", kTrait.getCoastalTradeRoutes());
	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_TRADE_ROUTE_MOD", kTrait.getTradeRouteModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_FOREIGN_TRADE_ROUTE_MOD", kTrait.getForeignTradeRouteModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_DEFENSE_MOD_ALL_CITIES", kTrait.getAllCityDefense());

	if (kTrait.getBombardDefense() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_BOMBARD_DEFENSE_MOD", -kTrait.getBombardDefense()));
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_ESPIONAGE_DEFENSE_MOD", kTrait.getEspionageDefense());

	// The world size of the game, also on the screens before a game, where it may not be set yet
	WorldSizeTypes eWorldSize = GC.getMapINLINE().getWorldSize();

	// Cities and population: the civic and building text keys
	iValue = kTrait.getLargestCityHappiness();
	if (iValue != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_LARGEST_CITIES_HAPPINESS", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR)), ((eWorldSize != NO_WORLDSIZE) ? GC.getWorldInfo(eWorldSize).getTargetNumCities() : 0)));
	}

	iValue = kTrait.getHappyPerMilitaryUnit();
	if (iValue != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_UNIT_HAPPINESS", abs(iValue), ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR))));
	}

	appendTraitFlagLine(szHelpString, "TXT_KEY_CIVIC_NO_POP_UNHEALTHY", kTrait.isNoUnhealthyPopulation());
	appendTraitFlagLine(szHelpString, "TXT_KEY_CIVIC_NO_BUILDING_UNHEALTHY", kTrait.isBuildingOnlyHealthy());

	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_STORES_FOOD", kTrait.getFoodKept());

	// FreeBonuses, BonusProductionModifiers, CommerceHappinesses, CommerceHealths: the building text keys
	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (kTrait.getFreeBonus(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_PROVIDES", kTrait.getFreeBonus(iI), GC.getBonusInfo((BonusTypes)iI).getTextKeyWide(), GC.getBonusInfo((BonusTypes)iI).getChar()));
		}
	}

	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (kTrait.getBonusProductionModifier(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_BONUS_PRODUCTION", kTrait.getBonusProductionModifier(iI), GC.getBonusInfo((BonusTypes)iI).getTextKeyWide()));
		}

		if (kTrait.getBonusWonderProductionModifier(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_BONUS_WONDER_PRODUCTION", kTrait.getBonusWonderProductionModifier(iI), GC.getBonusInfo((BonusTypes)iI).getTextKeyWide()));
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iValue = kTrait.getCommerceHappiness(iI);
		if (iValue != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_PER_LEVEL", ((iValue > 0) ? gDLL->getSymbolID(HAPPY_CHAR) : gDLL->getSymbolID(UNHAPPY_CHAR)), abs(100 / iValue), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}

		iValue = kTrait.getCommerceHealth(iI);
		if (iValue != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_PER_LEVEL", ((iValue > 0) ? gDLL->getSymbolID(HEALTHY_CHAR) : gDLL->getSymbolID(UNHEALTHY_CHAR)), abs(100 / iValue), GC.getCommerceInfo((CommerceTypes)iI).getChar()));
		}
	}

	// TerrainTrades: "Connects the trade network: Desert"
	bool bFirstTerrain = true;
	for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (kTrait.isTerrainTrade(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_TERRAIN_TRADE").c_str());
			szText = getLinkText(GC.getTerrainInfo((TerrainTypes)iI).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstTerrain);
			bFirstTerrain = false;
		}
	}

	// CapitalTerrainYieldPercents: "Desert in the city radius of the capital: +0.5[food], +1[commerce] each in the capital"
	for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		CvWString szYields;
		int iYield;
		for (iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			iValue = kTrait.getCapitalTerrainYieldPercent(iI, iYield);
			if (iValue != 0)
			{
				CvWString szOne;
				szOne.Format(L"%s%c", formatHundredths(iValue).GetCString(), GC.getYieldInfo((YieldTypes)iYield).getChar());
				szYields += (szYields.empty() ? L"" : L", ") + szOne;
			}
		}
		if (!szYields.empty())
		{
			szText = getLinkText(GC.getTerrainInfo((TerrainTypes)iI).getDescription());
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_CAPITAL_TERRAIN_YIELD", szText.GetCString(), szYields.GetCString()));
		}
	}

	// Unit support and maintenance: the civic and building text keys
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_FREE_UNITS", kTrait.getBaseFreeUnits());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_FREE_UNITS_POPULATION", kTrait.getFreeUnitsPopulationPercent());
	appendTraitValueLine(szHelpString, "TXT_KEY_CIVIC_FREE_MILITARY_UNITS", kTrait.getBaseFreeMilitaryUnits());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_FREE_MILITARY_UNITS_POPULATION", kTrait.getFreeMilitaryUnitsPopulationPercent());

	if (kTrait.getGoldPerUnit() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_SUPPORT_COSTS", kTrait.getGoldPerUnit(), GC.getCommerceInfo(COMMERCE_GOLD).getChar()));
	}

	if (kTrait.getGoldPerMilitaryUnit() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_MILITARY_SUPPORT_COSTS", kTrait.getGoldPerMilitaryUnit(), GC.getCommerceInfo(COMMERCE_GOLD).getChar()));
	}

	if (kTrait.getCorporationMaintenanceModifier() != 0)
	{
		if (kTrait.getCorporationMaintenanceModifier() <= -100)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_NO_MAINT_CORPORATION"));
		}
		else
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_NO_MAINT_CORPORATION_MOD", kTrait.getCorporationMaintenanceModifier()));
		}
	}

	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_MAINT_MOD", kTrait.getMaintenanceModifier());

	// Military: the civic and building text keys. The draft with the modifier of the world size, as the civics
	if (kTrait.getMaxConscript() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_CIVIC_CONSCRIPTION", (kTrait.getMaxConscript() * std::max(0, (((eWorldSize != NO_WORLDSIZE) ? GC.getWorldInfo(eWorldSize).getMaxConscriptModifier() : 0) + 100))) / 100));
	}

	// The other children of Conscription (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONSCRIPT_POPULATION", kTrait.getConscriptPopulationModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONSCRIPT_MIN_POPULATION", kTrait.getConscriptMinCityPopulationChange());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONSCRIPT_MIN_CULTURE", kTrait.getConscriptMinCulturePercentChange());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONSCRIPT_ANGER", kTrait.getConscriptPopAngerChange());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_CONSCRIPT_ANGER_LENGTH", kTrait.getConscriptAngerLengthPercent());

	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_NO_CONSCRIPT_EXPERIENCE_PENALTY", kTrait.isNoConscriptExperiencePenalty());
	appendTraitFlagLine(szHelpString, "TXT_KEY_CIVIC_MILITARY_FOOD", kTrait.isMilitaryFoodProduction());

	appendTraitValueLine(szHelpString, "TXT_KEY_BUILDING_HEAL_MOD", kTrait.getHealRateChange());

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		if (kTrait.getDomainFreeExperience(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_FREE_XP", GC.getDomainInfo((DomainTypes)iI).getTextKeyWide(), kTrait.getDomainFreeExperience(iI)));
		}

		if (kTrait.getDomainProductionModifier(iI) != 0)
		{
			appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_BUILDING_BUILDS_FASTER_DOMAIN", GC.getDomainInfo((DomainTypes)iI).getTextKeyWide(), kTrait.getDomainProductionModifier(iI)));
		}
	}
}

// The part of the trait help for the tags of 2026-11-19: RevealBonuses, NoTechProcesses, FreeTechs (added by this mod)
void CvGameTextMgr::parseTraitRevealAndStartEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szText;
	bool bFirst;
	int iI;

	// "Reveals without the technology: Uranium"
	bFirst = true;
	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (kTrait.isRevealBonus(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_REVEAL_BONUSES").c_str());
			szText = getLinkText(GC.getBonusInfo((BonusTypes)iI).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirst);
			bFirst = false;
		}
	}

	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_REVEAL_WONDER_PRODUCTION", kTrait.isRevealWonderProduction());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_REVEAL_WAR_PLANS", kTrait.isRevealWarPlans());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_REVEAL_HIDDEN_ATTITUDE", kTrait.isRevealHiddenAttitude());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_LONG_COUNT_CALENDAR", kTrait.isLongCountCalendar());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_IMPROVEMENT_UPGRADE_UNWORKED", kTrait.isImprovementUpgradeUnworked());
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_UNSETTLE_GREAT_PEOPLE", kTrait.isUnsettleGreatPeople());

	// StackPromotions: "Melee Units on the same plot as Sun Tzu or Great General: Combat I, Drill I", one line for each set
	for (iI = 0; iI < kTrait.getNumStackPromotionSets(); iI++)
	{
		const StackPromotionSet& kSet = kTrait.getStackPromotionSet(iI);
		CvWString szClasses;
		CvWString szCombats;
		CvWString szPromotions;
		int iJ;

		for (iJ = 0; iJ < (int)kSet.aiUnitClasses.size(); iJ++)
		{
			szClasses += ((iJ > 0) ? CvWString(gDLL->getText("TXT_KEY_OR")) : CvWString(L""));
			szClasses += GC.getUnitClassInfo((UnitClassTypes)kSet.aiUnitClasses[iJ]).getDescription();
		}

		for (iJ = 0; iJ < (int)kSet.aiUnitCombats.size(); iJ++)
		{
			szCombats += ((iJ > 0) ? L", " : L"");
			szCombats += GC.getUnitCombatInfo((UnitCombatTypes)kSet.aiUnitCombats[iJ]).getDescription();
		}
		if (szCombats.empty())
		{
			szCombats = gDLL->getText("TXT_KEY_TRAIT_STACK_PROMOTION_UNITS");
		}

		for (iJ = 0; iJ < (int)kSet.aiPromotions.size(); iJ++)
		{
			szPromotions += ((iJ > 0) ? L", " : L"");
			szPromotions += getLinkText(GC.getPromotionInfo((PromotionTypes)kSet.aiPromotions[iJ]).getDescription());
		}

		CvWString szLine = gDLL->getText("TXT_KEY_TRAIT_STACK_PROMOTION", szClasses.GetCString(), szCombats.GetCString(), szPromotions.GetCString());
		// bStackUnitsToo: " (Great General too)"
		if (kSet.bStackUnitsToo)
		{
			szLine += gDLL->getText("TXT_KEY_TRAIT_STACK_PROMOTION_UNITS_TOO", szClasses.GetCString());
		}
		appendTraitLine(szHelpString, szLine);
	}

	// UnsettleSpecialists: "Settled great people that can become units again: Confucius (settled)"
	if (!kTrait.isUnsettleGreatPeople())
	{
		bFirst = true;
		for (iI = 0; iI < (int)kTrait.getUnsettleSpecialists().size(); iI++)
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_UNSETTLE_SPECIALISTS").c_str());
			szText = getLinkText(GC.getSpecialistInfo((SpecialistTypes)kTrait.getUnsettleSpecialists()[iI]).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirst);
			bFirst = false;
		}
	}
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_IGNORE_EVENTS", kTrait.isIgnoreEvents());

	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_EVENT_CHANCE_CHANGE", kTrait.getEventChanceChange());

	if (kTrait.isNoFirstEventDelay())
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_NO_FIRST_EVENT_DELAY", GC.getDefineINT("FIRST_EVENT_DELAY_TURNS")));
	}

	// Deals
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_DEAL_BUY_CITIES", kTrait.isBuyCities());

	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_DEAL_CITY_VALUE_MODIFIER", kTrait.getDealCityValueModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_DEAL_GOLD_LIMIT_MODIFIER", kTrait.getDealGoldLimitModifier());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_DEAL_GOLD_VALUE_MODIFIER", kTrait.getDealGoldValueModifier());

	// iInflationModifier (added by this mod)
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_INFLATION_MODIFIER", kTrait.getInflationModifier());

	// LendLease
	appendTraitFlagLine(szHelpString, "TXT_KEY_TRAIT_LEND_LEASE", kTrait.isLendLease());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_LEND_LEASE_RESEARCH_PERCENT", kTrait.getLendLeaseResearchPercent());
	appendTraitValueLine(szHelpString, "TXT_KEY_TRAIT_LEND_LEASE_GOLD_PERCENT", kTrait.getLendLeaseGoldPercent());
	if (kTrait.getLendLeaseAttitudeChange() != 0)
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_LEND_LEASE_ATTITUDE", kTrait.getLendLeaseAttitudeChange(), ((kTrait.getLendLeaseAttitudeLimit() <= 0 || kTrait.getLendLeaseAttitudeLimit() > 4) ? 4 : kTrait.getLendLeaseAttitudeLimit())));
	}

	// "Can build without the technology: Research". The processes are loaded when a game is launched: none on the screens
	// before (GC.getNumProcessInfos() is 0)
	bFirst = true;
	for (iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (kTrait.isNoTechProcess(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_NO_TECH_PROCESSES").c_str());
			setListHelp(szHelpString, szTempBuffer.GetCString(), GC.getProcessInfo((ProcessTypes)iI).getDescription(), L", ", bFirst);
			bFirst = false;
		}
	}

	// "Starts with: The Wheel, Bronze Working"
	bFirst = true;
	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (kTrait.isFreeTech(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_FREE_TECHS").c_str());
			szText = getLinkText(GC.getTechInfo((TechTypes)iI).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirst);
			bFirst = false;
		}
	}

	parseTraitExplorationRewards(szHelpString, eTrait);
}

// ExplorationRewards (added by this mod): "Each 10 water plots revealed: +1 gold", one line per entry, then one note with a
// link to the concept of the regions
void CvGameTextMgr::parseTraitExplorationRewards(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	static const char* aszPlotKeys[NUM_EXPLORE_PLOT_TYPES] =
	{
		"TXT_KEY_EXPLORE_PLOT_WATER",
		"TXT_KEY_EXPLORE_PLOT_HOME",
		"TXT_KEY_EXPLORE_PLOT_OVERSEAS_OLD_WORLD",
		"TXT_KEY_EXPLORE_PLOT_OVERSEAS_NEW_WORLD",
	};
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szAmount;
	CvWString szText;
	int iPlotType;
	int iCommerce;
	int iPercent;
	int iPlots;
	int iI;

	if (kTrait.getNumExplorationRewards() <= 0)
	{
		return;
	}

	for (iI = 0; iI < kTrait.getNumExplorationRewards(); iI++)
	{
		iPlotType = kTrait.getExplorationRewardPlotType(iI);
		iCommerce = kTrait.getExplorationRewardCommerceType(iI);
		iPercent = kTrait.getExplorationRewardPercent(iI);
		if (iPlotType < 0 || iPlotType >= NUM_EXPLORE_PLOT_TYPES || iCommerce < 0 || iCommerce >= NUM_COMMERCE_TYPES || iPercent <= 0)
		{
			continue;
		}

		// 10 -> each 10 plots +1; 250 -> each plot +2.5; 30 -> each plot +0.3
		if ((100 % iPercent) == 0)
		{
			iPlots = 100 / iPercent;
			szAmount = L"1";
		}
		else
		{
			iPlots = 1;
			if ((iPercent % 10) == 0)
			{
				szAmount.Format(L"%d.%d", iPercent / 100, (iPercent % 100) / 10);
			}
			else
			{
				szAmount.Format(L"%d.%02d", iPercent / 100, iPercent % 100);
			}
		}
		if ((iPercent % 100) == 0)
		{
			szAmount.Format(L"%d", iPercent / 100);
		}

		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_EXPLORATION_REWARD", gDLL->getText(aszPlotKeys[iPlotType]).GetCString(), iPlots, szAmount.GetCString(), GC.getCommerceInfo((CommerceTypes)iCommerce).getChar()));
	}

	szText = getLinkText(gDLL->getText("TXT_KEY_CONCEPT_REGIONS").GetCString());
	appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_EXPLORATION_REWARD_NOTE", szText.GetCString()));
}

static bool isTypeInList(const std::vector<int>& aiTypes, int iType)
{
	return (std::find(aiTypes.begin(), aiTypes.end(), iType) != aiTypes.end());
}

// The part of the trait help for PrereqOrTraits and DisableTraits of the espionage missions and PrereqOrTraits of the
// processes (added by this mod). The missions and the processes are loaded when a game is launched: none on the screens
// before (GC.getNumEspionageMissionInfos() is 0)
void CvGameTextMgr::parseTraitEspionageMissions(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvWString szTempBuffer;
	bool bFirst;
	int iI;

	// "Unique process: Espionage"
	bFirst = true;
	for (iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (isTypeInList(GC.getProcessInfo((ProcessTypes)iI).getPrereqOrTraitsVector(), eTrait))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_UNIQUE_PROCESSES").c_str());
			setListHelp(szHelpString, szTempBuffer.GetCString(), GC.getProcessInfo((ProcessTypes)iI).getDescription(), L", ", bFirst);
			bFirst = false;
		}
	}

	// "Espionage missions: Assassinate a Great Person"
	bFirst = true;
	for (iI = 0; iI < GC.getNumEspionageMissionInfos(); iI++)
	{
		CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo((EspionageMissionTypes)iI);
		if (isTypeInList(kMission.getPrereqOrTraitsVector(), eTrait) && !isTypeInList(kMission.getDisableTraitsVector(), eTrait) && kMission.getCost() != -1)
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_ENABLED_ESPIONAGE_MISSIONS").c_str());
			setListHelp(szHelpString, szTempBuffer.GetCString(), kMission.getDescription(), L", ", bFirst);
			bFirst = false;
		}
	}

	// "Cannot do the espionage missions: Poison Water"
	bFirst = true;
	for (iI = 0; iI < GC.getNumEspionageMissionInfos(); iI++)
	{
		CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo((EspionageMissionTypes)iI);
		if (isTypeInList(kMission.getDisableTraitsVector(), eTrait) && kMission.getCost() != -1)
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_DISABLED_ESPIONAGE_MISSIONS").c_str());
			setListHelp(szHelpString, szTempBuffer.GetCString(), kMission.getDescription(), L", ", bFirst);
			bFirst = false;
		}
	}
}

void CvGameTextMgr::parseTraitUniqueImprovements(CvWStringBuffer& szHelpString, TraitTypes eTrait)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szText;
	int iI, iJ;

	// PrereqOrTraits of the builds: "Unique improvement: Terrace", "Unique improvement: Terrace (replaces Farm)" (added by
	// this mod)
	for (iI = 0; iI < GC.getNumBuildInfos(); ++iI)
	{
		CvBuildInfo& kBuild = GC.getBuildInfo((BuildTypes)iI);

		for (iJ = 0; iJ < kBuild.getNumPrereqOrTraits(); ++iJ)
		{
			if (kBuild.getPrereqOrTraits(iJ) == eTrait)
			{
				szText = getLinkText(getBuildResultDescription(kBuild));
				if (kBuild.getReplacedBuild() != NO_BUILD)
				{
					CvWString szReplaced;
					szReplaced = getLinkText(getBuildResultDescription(GC.getBuildInfo((BuildTypes)kBuild.getReplacedBuild())));
					szTempBuffer.Format(L"%s  %c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_TRAIT_UNIQUE_IMPROVEMENT_REPLACES", szText.GetCString(), szReplaced.GetCString()).c_str());
				}
				else
				{
					szTempBuffer.Format(L"%s  %c%s", NEWLINE, gDLL->getSymbolID(BULLET_CHAR), gDLL->getText("TXT_KEY_TRAIT_UNIQUE_IMPROVEMENT", szText.GetCString()).c_str());
				}
				szHelpString.append(szTempBuffer);
				break;
			}
		}
	}
}

void CvGameTextMgr::parseTraitFeatureAndObsolescenceEffects(CvWStringBuffer& szHelpString, TraitTypes eTrait, CivilizationTypes eCivilization)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvWString szTempBuffer;
	CvWString szText;
	int iI, iJ;

	// MaintainFeatures: "Improvements keep: Forest, Jungle"
	bool bFirstFeature = true;
	for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		if (kTrait.isMaintainFeatures(iI))
		{
			szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_MAINTAIN_FEATURES").c_str());
			szText = getLinkText(GC.getFeatureInfo((FeatureTypes)iI).getDescription());
			setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstFeature);
			bFirstFeature = false;
		}
	}

	// FeatureRoutes: "Counts as Road in our territory (movement and trade network): Forest, Jungle", a line for each
	// route type
	for (iJ = 0; iJ < GC.getNumRouteInfos(); ++iJ)
	{
		bFirstFeature = true;
		for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
		{
			if (kTrait.getFeatureRouteType(iI) == iJ)
			{
				szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_FEATURE_ROUTES", GC.getRouteInfo((RouteTypes)iJ).getTextKeyWide()).c_str());
				szText = getLinkText(GC.getFeatureInfo((FeatureTypes)iI).getDescription());
				setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstFeature);
				bFirstFeature = false;
			}
		}
	}

	// bIgnoreBuildingObsolescence: "Buildings never become obsolete". Otherwise NoObsoleteBuildingClasses and
	// NoObsoleteSpecialBuildings: "Never obsolete: Castle, Monastery" (the buildings of the civilization)
	if (kTrait.isIgnoreBuildingObsolescence())
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_IGNORE_BUILDING_OBSOLESCENCE"));
	}
	else
	{
		bool bFirstBuilding = true;
		for (iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
		{
			if (kTrait.isNoObsoleteBuildingClass(iI))
			{
				BuildingTypes eLoopBuilding = NO_BUILDING;
				if (eCivilization != NO_CIVILIZATION)
				{
					eLoopBuilding = (BuildingTypes)GC.getCivilizationInfo(eCivilization).getCivilizationBuildings(iI);
				}
				if (eLoopBuilding == NO_BUILDING)
				{
					eLoopBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
				}

				if (eLoopBuilding != NO_BUILDING)
				{
					szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_NO_OBSOLETE_BUILDINGS").c_str());
					szText = getLinkText(GC.getBuildingInfo(eLoopBuilding).getDescription());
					setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstBuilding);
					bFirstBuilding = false;
				}
			}
		}

		// Special buildings have no Civilopedia page: no link
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
		{
			if (kTrait.isNoObsoleteSpecialBuilding(iI))
			{
				szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_NO_OBSOLETE_BUILDINGS").c_str());
				setListHelp(szHelpString, szTempBuffer.GetCString(), GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).getDescription(), L", ", bFirstBuilding);
				bFirstBuilding = false;
			}
		}
	}

	// bIgnoreBonusObsolescence: "Resources never become obsolete". Otherwise NoObsoleteBonuses: "Never obsolete: Fur, Ivory"
	if (kTrait.isIgnoreBonusObsolescence())
	{
		appendTraitLine(szHelpString, gDLL->getText("TXT_KEY_TRAIT_IGNORE_BONUS_OBSOLESCENCE"));
	}
	else
	{
		bool bFirstBonus = true;
		for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (kTrait.isNoObsoleteBonus(iI))
			{
				szTempBuffer.Format(L"%s  %s", NEWLINE, gDLL->getText("TXT_KEY_TRAIT_NO_OBSOLETE_BONUSES").c_str());
				szText = getLinkText(GC.getBonusInfo((BonusTypes)iI).getDescription());
				setListHelp(szHelpString, szTempBuffer.GetCString(), szText.GetCString(), L", ", bFirstBonus);
				bFirstBonus = false;
			}
		}
	}
}
