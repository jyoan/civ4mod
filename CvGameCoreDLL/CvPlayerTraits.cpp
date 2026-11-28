// CvPlayerTraits.cpp (added by this mod)
//
// The functions of CvPlayer added by this mod for the traits (and a few other tags of the mod), moved out of
// CvPlayer.cpp because of its size: the traits gained and lost during the game, the trait counters and hooks,
// the sums and flags of the trait tags (sumTraitValues, hasTraitFlag) and the effects built on them.

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvPlayer.h"
#include "CvGameCoreUtils.h"
#include "CvArtFileMgr.h"
#include "CvDiploParameters.h"
#include "CvInitCore.h"
#include "CyArgsList.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"
#include "CvDiploParameters.h"
#include "FProfiler.h"
#include "CvGameTextMgr.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"
#include "CyPlayer.h"
#include "CvEventReporter.h"

#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"

// The traits the player starts with: the leader's, and the civilization's (<Traits> in CIV4CivilizationInfos.xml)
bool CvPlayer::hasBaseTrait(TraitTypes eTrait) const
{
	FAssertMsg((getLeaderType() >= 0), "getLeaderType() is less than zero");
	FAssertMsg((eTrait >= 0), "eTrait is less than zero");
	if (GC.getLeaderHeadInfo(getLeaderType()).hasTrait(eTrait))
	{
		return true;
	}

	return ((getCivilizationType() != NO_CIVILIZATION) && GC.getCivilizationInfo(getCivilizationType()).hasTrait(eTrait));
}

// How many times the trait is given during the game (added by this mod). The player has the trait while it is above 0.
int CvPlayer::getFreeTraitCount(TraitTypes eTrait) const
{
	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");
	return ((m_paiFreeTraitCount != NULL) ? m_paiFreeTraitCount[eTrait] : 0);
}

// Gives (iChange > 0) or takes away (iChange < 0) the trait during the game (added by this mod). When the player gains or
// loses the trait (it is not one of the base traits), its effects are added or removed: the values of processTrait, and
// the stored values that depend on the traits (updateTraitDependents). The other trait effects are summed up when used.
// Must be called on every computer of a multiplayer game (not from the interface code).
void CvPlayer::changeFreeTraitCount(TraitTypes eTrait, int iChange)
{
	std::vector<bool> abOldObsoleteBuilding;
	bool bOldHasTrait;
	bool bUpdate;
	int iI;

	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");

	if (iChange == 0)
	{
		return;
	}

	// Unused player slots (CvTeam::processTech goes through them): only the count. CvPlayer::init resets it.
	if (getLeaderType() == NO_LEADER)
	{
		m_paiFreeTraitCount[eTrait] += iChange;
		return;
	}

	bOldHasTrait = hasTrait(eTrait);

	// While updateTraitDependents runs (a building it processes gives or takes away a trait), it also handles this change
	bUpdate = (!m_bUpdatingTraitDependents && (bOldHasTrait != (hasBaseTrait(eTrait) || (getFreeTraitCount(eTrait) + iChange > 0))));
	if (bUpdate)
	{
		// Before the change: which buildings are obsolete in our cities (a trait can keep them from becoming obsolete)
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			abOldObsoleteBuilding.push_back(isObsoleteBuilding((BuildingTypes)iI));
		}
	}

	// PrereqOrTraits of the bonuses (added by this mod)
	if (bOldHasTrait != (hasBaseTrait(eTrait) || (getFreeTraitCount(eTrait) + iChange > 0)))
	{
		updateTraitBonusPlots(eTrait, false);
	}

	m_paiFreeTraitCount[eTrait] += iChange;
	FAssert(getFreeTraitCount(eTrait) >= 0);

	if (bOldHasTrait != hasTrait(eTrait))
	{
		updateTraitBonusPlots(eTrait, true);

		// bBarbarianAlly (added by this mod)
		if (GC.getTraitInfo(eTrait).isBarbarianAlly())
		{
			GET_TEAM(getTeam()).updateBarbarianAlly();
		}

		// bBarbarianVision (added by this mod)
		if (GC.getTraitInfo(eTrait).isBarbarianVision())
		{
			GET_TEAM(getTeam()).updateBarbarianVision();
		}

		processTrait(eTrait, (hasTrait(eTrait) ? 1 : -1));

		if (bUpdate)
		{
			updateTraitDependents(abOldObsoleteBuilding);
		}

		// The traits this trait gives while their conditions hold
		if (GC.getTraitInfo(eTrait).getNumConditionalFreeTraits() > 0)
		{
			updateConditionalTraits();
		}

		doTraitGainedPythonHook(eTrait, hasTrait(eTrait));	// PyOnGained / PyOnLost (added by this mod)
	}
}

// Temporary traits (added by this mod): turns left while the player has the trait for a limited time, 0 if not temporary
int CvPlayer::getTraitTurns(TraitTypes eTrait) const
{
	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");
	return ((m_paiTraitTurns != NULL) ? m_paiTraitTurns[eTrait] : 0);
}

// Gives the trait for iTurns turns (0 ends it). A temporary trait counts once in getFreeTraitCount while its turns are
// above 0, so a trait the player also has otherwise stays. doTraitTurns counts the turns down. The player gets a message
// when it gains or loses the trait this way. Python: gainTraitForTurns of CvHookUtils.py
void CvPlayer::setTraitTurns(TraitTypes eTrait, int iTurns)
{
	CvWString szBuffer;
	bool bOldHasTrait;
	int iOldTurns;

	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");

	if (m_paiTraitTurns == NULL || getLeaderType() == NO_LEADER)
	{
		return;
	}

	iTurns = std::max(0, iTurns);
	iOldTurns = m_paiTraitTurns[eTrait];
	if (iOldTurns == iTurns)
	{
		return;
	}

	m_paiTraitTurns[eTrait] = iTurns;

	if ((iOldTurns == 0) || (iTurns == 0))
	{
		bOldHasTrait = hasTrait(eTrait);
		changeFreeTraitCount(eTrait, ((iTurns > 0) ? 1 : -1));

		if (!bOldHasTrait && hasTrait(eTrait))
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_TEMPORARY_TRAIT_GAINED", GC.getTraitInfo(eTrait).getTextKeyWide(), iTurns);
			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
		else if (bOldHasTrait && !hasTrait(eTrait))
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_TEMPORARY_TRAIT_ENDED", GC.getTraitInfo(eTrait).getTextKeyWide());
			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGEEND", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
		}
	}
}

// At the start of the player's turn: one turn less for each temporary trait (added by this mod)
void CvPlayer::doTraitTurns()
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (getTraitTurns((TraitTypes)iI) > 0)
		{
			setTraitTurns((TraitTypes)iI, getTraitTurns((TraitTypes)iI) - 1);
		}
	}
}

// The counter of a trait (added by this mod): a number of the player that the Python hooks of the trait can count with
// (e.g. great people born, wars declared on the player). Nothing in the DLL reads it. Kept when the trait is lost.
int CvPlayer::getTraitCounter(TraitTypes eTrait) const
{
	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");
	return ((m_paiTraitCounter != NULL) ? m_paiTraitCounter[eTrait] : 0);
}

void CvPlayer::setTraitCounter(TraitTypes eTrait, int iValue)
{
	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");
	if (m_paiTraitCounter != NULL)
	{
		m_paiTraitCounter[eTrait] = iValue;
	}
}

void CvPlayer::changeTraitCounter(TraitTypes eTrait, int iChange)
{
	setTraitCounter(eTrait, getTraitCounter(eTrait) + iChange);
}

// GainTraitsOnFriendly of the traits (added by this mod): whether the traits of ePlayer were checked when ePlayer became friendly
bool CvPlayer::isFriendlyTraitGainedFrom(PlayerTypes ePlayer) const
{
	return (ePlayer >= 0 && ePlayer < MAX_PLAYERS) ? m_abFriendlyTraitGainedFrom[ePlayer] : false;
}

// GainTraitsOnFriendly of the traits (added by this mod): at the start of the turn of this player, while it has a trait with
// GainTraitsOnFriendly, each AI player (alive, met, not barbarian) whose true attitude to this player (AI_getAttitude with bForced
// false, without the automatic friendly of the team and the vassals) is friendly gives its basic traits (gainBasicTraitsOf; bAll of
// any of the traits gives all of them, otherwise one at random). Once per AI player: it is marked even when nothing is gained (all
// the candidates are had already), and it is not checked again when it becomes friendly again later
void CvPlayer::doGainTraitsOnFriendly()
{
	PlayerTypes eLoopPlayer;
	bool bAll;
	int iI;

	if (getLeaderType() == NO_LEADER || !hasTraitFlag(&CvTraitInfo::isGainTraitsOnFriendly))
	{
		return;
	}

	bAll = hasTraitFlag(&CvTraitInfo::isGainTraitsOnFriendlyAll);

	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		eLoopPlayer = (PlayerTypes)iI;

		if (eLoopPlayer == getID() || m_abFriendlyTraitGainedFrom[iI])
		{
			continue;
		}

		CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
		if (!kLoopPlayer.isAlive() || kLoopPlayer.isHuman() || kLoopPlayer.isBarbarian() || kLoopPlayer.getLeaderType() == NO_LEADER)
		{
			continue;
		}

		if (!GET_TEAM(getTeam()).isHasMet(kLoopPlayer.getTeam()))
		{
			continue;
		}

		if (kLoopPlayer.AI_getAttitude(getID(), false) >= ATTITUDE_FRIENDLY)
		{
			m_abFriendlyTraitGainedFrom[iI] = true;
			gainBasicTraitsOf(eLoopPlayer, bAll, "TXT_KEY_MISC_TRAIT_GAINED_BY_FRIENDLY");
		}
	}
}

// RandomBasicTraits of the traits (added by this mod): whether the trait is one of the random basic traits given now
bool CvPlayer::isRandomBasicTrait(TraitTypes eTrait) const
{
	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");
	return ((m_paiRandomBasicTrait != NULL) && (m_paiRandomBasicTrait[eTrait] > 0));
}

int CvPlayer::getRandomBasicTraitTurn() const
{
	return m_iRandomBasicTraitTurn;
}

// iTurns of RandomBasicTraits scaled by the game speed (growth percent, as the event times), at least 1. Also for the help text,
// so it works before a game is started
int CvPlayer::getRandomBasicTraitTurnsForSpeed(int iTurns)
{
	GameSpeedTypes eSpeed = GC.getGameINLINE().getGameSpeedType();

	if (eSpeed != NO_GAMESPEED)
	{
		iTurns = (iTurns * GC.getGameSpeedInfo(eSpeed).getGrowthPercent()) / 100;
	}

	return std::max(1, iTurns);
}

// RandomBasicTraits (iTurns, iCount) of the traits (added by this mod; the Insane trait of
// FFH2). At the start of the turn (and at the start of the game, CvGame::setInitialItems): when the player has a trait with
// RandomBasicTraits and iTurns (scaled by the game speed) have passed since the last change (or there was
// none), the random basic traits given last time are replaced by iCount new ones. The candidates are the
// basic traits (bBasicTrait) the player does not have otherwise (the ones given last time are candidates again); no trait
// twice. Several such traits: the count is summed, the shortest interval is used. When the player no longer has such a
// trait, the given traits are taken away. The player gets a message with the new traits
void CvPlayer::doRandomBasicTraits()
{
	std::vector<TraitTypes> aeCandidates;
	std::vector<bool> abNew;
	CvWString szList;
	CvWString szBuffer;
	TraitTypes eSource;
	int iTurns;
	int iCount;
	int iPick;
	int iI;

	if (m_paiRandomBasicTrait == NULL || getLeaderType() == NO_LEADER)
	{
		return;
	}

	eSource = NO_TRAIT;
	iTurns = 0;		// the shortest iTurns above 0; 0 when every trait has iTurns 0 (the traits are given once and kept)
	iCount = 0;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.isRandomBasicTraits() && kTrait.getRandomBasicTraitCount() > 0 && hasTrait((TraitTypes)iI))
		{
			if (eSource == NO_TRAIT)
			{
				eSource = (TraitTypes)iI;
			}

			if (kTrait.getRandomBasicTraitTurns() > 0)
			{
				iTurns = ((iTurns > 0) ? std::min(iTurns, kTrait.getRandomBasicTraitTurns()) : kTrait.getRandomBasicTraitTurns());
			}
			iCount += kTrait.getRandomBasicTraitCount();
		}
	}

	if (eSource == NO_TRAIT)
	{
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (m_paiRandomBasicTrait[iI] > 0)
			{
				m_paiRandomBasicTrait[iI] = 0;
				changeFreeTraitCount((TraitTypes)iI, -1);
			}
		}
		m_iRandomBasicTraitTurn = -1;
		return;
	}

	if (m_iRandomBasicTraitTurn >= 0 && (iTurns <= 0 || (GC.getGameINLINE().getGameTurn() - m_iRandomBasicTraitTurn) < getRandomBasicTraitTurnsForSpeed(iTurns)))
	{
		return;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		abNew.push_back(false);

		if (GC.getTraitInfo((TraitTypes)iI).isBasicTrait() && !hasBaseTrait((TraitTypes)iI) && (getFreeTraitCount((TraitTypes)iI) - m_paiRandomBasicTrait[iI]) <= 0)
		{
			aeCandidates.push_back((TraitTypes)iI);
		}
	}

	while (iCount > 0 && !aeCandidates.empty())
	{
		iPick = GC.getGameINLINE().getSorenRandNum((int)aeCandidates.size(), "Random basic trait");
		abNew[aeCandidates[iPick]] = true;
		aeCandidates.erase(aeCandidates.begin() + iPick);
		iCount--;
	}

	// The traits that stay are not taken away and given again (their effects and PyOnLost / PyOnGained do not run)
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (m_paiRandomBasicTrait[iI] > 0 && !abNew[iI])
		{
			m_paiRandomBasicTrait[iI] = 0;
			changeFreeTraitCount((TraitTypes)iI, -1);
		}
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (abNew[iI])
		{
			if (m_paiRandomBasicTrait[iI] == 0)
			{
				m_paiRandomBasicTrait[iI] = 1;
				changeFreeTraitCount((TraitTypes)iI, 1);
			}

			if (!szList.empty())
			{
				szList += L", ";
			}
			szList += GC.getTraitInfo((TraitTypes)iI).getDescription();
		}
	}

	m_iRandomBasicTraitTurn = GC.getGameINLINE().getGameTurn();

	if (!szList.empty())
	{
		szBuffer = gDLL->getText("TXT_KEY_MISC_RANDOM_BASIC_TRAITS", GC.getTraitInfo(eSource).getTextKeyWide(), szList.GetCString());
		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
	}
}

// PyOnGained / PyOnLost of a trait (added by this mod): CvTraitInterface.py evaluates the expression with pPlayer and eTrait
// when the player gains or loses the trait during the game (changeFreeTraitCount: conditions, FreeTraits, temporary
// traits, Python). Not for the traits the player has when the game starts.
void CvPlayer::doTraitGainedPythonHook(TraitTypes eTrait, bool bGained)
{
	TraitPythonHookTypes eHook = (bGained ? TRAIT_PYHOOK_GAINED : TRAIT_PYHOOK_LOST);

	if (isObserver() || !GC.getGameINLINE().isFinalInitialized())
	{
		return;
	}

	if (CvString(GC.getTraitInfo(eTrait).getPythonHook(eHook)).empty())
	{
		return;
	}

	CyPlayer* pyPlayer = new CyPlayer(this);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlayer));
	argsList.add((int)eTrait);
	gDLL->getPythonIFace()->callFunction(PYTraitModule, (bGained ? "onGained" : "onLost"), argsList.makeFunctionArgs());
	delete pyPlayer;	// the python function must not hold on to it
}

// The part of getFreeTraitCount given by the ConditionalFreeTraits of the traits of the player (added by this mod)
int CvPlayer::getConditionalTraitCount(TraitTypes eTrait) const
{
	FAssertMsg(eTrait >= 0, "eTrait is expected to be non-negative (invalid Index)");
	FAssertMsg(eTrait < GC.getNumTraitInfos(), "eTrait is expected to be within maximum bounds (invalid Index)");
	return ((m_paiConditionalTraitCount != NULL) ? m_paiConditionalTraitCount[eTrait] : 0);
}

// All the conditions of an item of ConditionalFreeTraits hold for the player
bool CvPlayer::isConditionalFreeTraitActive(const ConditionalFreeTrait& kCondition) const
{
	bool bAtWar = (GET_TEAM(getTeam()).getAtWarCount(true) > 0);

	if ((kCondition.bAtWar && !bAtWar) || (kCondition.bAtPeace && bAtWar))
	{
		return false;
	}

	// bDefensiveWar (added by this mod)
	if (kCondition.bDefensiveWar && !GET_TEAM(getTeam()).isAtDefensiveWar())
	{
		return false;
	}

	if (kCondition.iStateReligion != NO_RELIGION && getStateReligion() != kCondition.iStateReligion)
	{
		return false;
	}

	if (kCondition.bNoStateReligion && getStateReligion() != NO_RELIGION)
	{
		return false;
	}

	if ((kCondition.iEraMin != NO_ERA && getCurrentEra() < kCondition.iEraMin) || (kCondition.iEraMax != NO_ERA && getCurrentEra() > kCondition.iEraMax))
	{
		return false;
	}

	if (kCondition.iPrereqCivic != NO_CIVIC && !isCivicOrReplacing((CivicTypes)kCondition.iPrereqCivic))
	{
		return false;
	}

	// The leader of the player, as the traits: with random personalities the AI plays by another favorite civic
	if (kCondition.bFavoriteCivic)
	{
		CivicTypes eFavoriteCivic = (CivicTypes)GC.getLeaderHeadInfo(getLeaderType()).getFavoriteCivic();
		if (eFavoriteCivic == NO_CIVIC || !isCivicOrReplacing(eFavoriteCivic))
		{
			return false;
		}
	}

	if (getNumUnitsKilled() < kCondition.iMinUnitsKilled || getNumCitiesConquered() < kCondition.iMinCitiesConquered)
	{
		return false;
	}

	// PrereqTech (added by this mod)
	if (kCondition.iPrereqTech != NO_TECH && !GET_TEAM(getTeam()).isHasTech((TechTypes)kCondition.iPrereqTech))
	{
		return false;
	}

	// bGoldenAge, bNoGoldenAge, bAnarchy, bNoAnarchy (added by this mod)
	if ((kCondition.bGoldenAge && !isGoldenAge()) || (kCondition.bNoGoldenAge && isGoldenAge()))
	{
		return false;
	}

	if ((kCondition.bAnarchy && !isAnarchy()) || (kCondition.bNoAnarchy && isAnarchy()))
	{
		return false;
	}

	// iMinCities, iMaxCities (added by this mod)
	if ((kCondition.iMinCities > 0 && getNumCities() < kCondition.iMinCities) || (kCondition.iMaxCities > 0 && getNumCities() > kCondition.iMaxCities))
	{
		return false;
	}

	// PrereqBonus (added by this mod): the bonuses of the plot group of the capital, as the trade screen shows them
	if (kCondition.iPrereqBonus != NO_BONUS && getNumAvailableBonuses((BonusTypes)kCondition.iPrereqBonus) <= 0)
	{
		return false;
	}

	// PrereqBuildingClass (added by this mod): a building of the class, obsolete or not
	if (kCondition.iPrereqBuildingClass != NO_BUILDINGCLASS && getBuildingClassCount((BuildingClassTypes)kCondition.iPrereqBuildingClass) <= 0)
	{
		return false;
	}

	// bVassal, bHasVassal (added by this mod)
	if ((kCondition.bVassal && !GET_TEAM(getTeam()).isAVassal()) || (kCondition.bHasVassal && GET_TEAM(getTeam()).getVassalCount() <= 0))
	{
		return false;
	}

	// iMinScoreRank (added by this mod): the rank of the last update of the scores (CvGame::updateScore), 1 for the top
	if (kCondition.iMinScoreRank > 0 && GC.getGameINLINE().getPlayerRank(getID()) + 1 < kCondition.iMinScoreRank)
	{
		return false;
	}

	// PyCondition (added by this mod): last, after the other conditions. CvHookInterface.py evaluates the expression with
	// pPlayer and eTrait (the trait given). It must not change the game
	if (!kCondition.szPyCondition.empty())
	{
		CyPlayer* pyPlayer = new CyPlayer(const_cast<CvPlayer*>(this));
		CyArgsList argsList;
		long lResult = 0;

		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlayer));
		argsList.add(kCondition.iTrait);
		argsList.add(kCondition.szPyCondition.GetCString());
		gDLL->getPythonIFace()->callFunction(PYHookModule, "isConditionTrue", argsList.makeFunctionArgs(), &lResult);
		delete pyPlayer;	// the python function must not hold on to it

		if (lResult == 0)
		{
			return false;
		}
	}

	return true;
}

// ConditionalFreeTraits (added by this mod): gives and takes away the traits whose conditions changed. Called where a
// condition can change (war and peace, the state religion, the era, the civics, a trait with ConditionalFreeTraits
// gained or lost) and at the start of each turn. A given trait can give traits itself, so this repeats until nothing
// changes (at most once per trait). bNotify: the player gets a message for each trait gained (the record of the player).
void CvPlayer::updateConditionalTraits(bool bNotify)
{
	std::vector<int> aiCount;
	CvWString szBuffer;
	bool bChanged;
	bool bOldHasTrait;
	int iPass;
	int iChange;
	int iI, iJ;

	if (m_bUpdatingConditionalTraits || getLeaderType() == NO_LEADER || m_paiConditionalTraitCount == NULL)
	{
		return;
	}

	m_bUpdatingConditionalTraits = true;

	for (iPass = 0; iPass <= GC.getNumTraitInfos(); iPass++)
	{
		aiCount.assign(GC.getNumTraitInfos(), 0);

		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (hasTrait((TraitTypes)iI))
			{
				for (iJ = 0; iJ < GC.getTraitInfo((TraitTypes)iI).getNumConditionalFreeTraits(); iJ++)
				{
					if (isConditionalFreeTraitActive(GC.getTraitInfo((TraitTypes)iI).getConditionalFreeTrait(iJ)))
					{
						aiCount[GC.getTraitInfo((TraitTypes)iI).getConditionalFreeTraitType(iJ)]++;
					}
				}
			}
		}

		bChanged = false;

		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			iChange = aiCount[iI] - m_paiConditionalTraitCount[iI];
			if (iChange != 0)
			{
				bOldHasTrait = hasTrait((TraitTypes)iI);
				m_paiConditionalTraitCount[iI] = aiCount[iI];
				changeFreeTraitCount((TraitTypes)iI, iChange);
				bChanged = true;

				if (bNotify && !bOldHasTrait && hasTrait((TraitTypes)iI))
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_TRAIT_GAINED_BY_RECORD", GC.getTraitInfo((TraitTypes)iI).getTextKeyWide());
					gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
				}
			}
		}

		if (!bChanged)
		{
			break;
		}
	}

	m_bUpdatingConditionalTraits = false;
}

// The record of the player for ConditionalFreeTraits (added by this mod). It only grows, so these traits stay
int CvPlayer::getNumUnitsKilled() const
{
	return m_iNumUnitsKilled;
}

void CvPlayer::changeNumUnitsKilled(int iChange)
{
	m_iNumUnitsKilled += iChange;
	updateConditionalTraits(true);
}

int CvPlayer::getNumCitiesConquered() const
{
	return m_iNumCitiesConquered;
}

void CvPlayer::changeNumCitiesConquered(int iChange)
{
	m_iNumCitiesConquered += iChange;
	updateConditionalTraits(true);
}

// After the player gains or loses traits: updates the stored values that depend on the traits. abObsoleteBuilding holds
// which buildings were obsolete in our cities (their effects removed) before the change.
void CvPlayer::updateTraitDependents(std::vector<bool>& abObsoleteBuilding)
{
	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	bool bObsolete;
	bool bChanged;
	int iLoop;
	int iI;

	m_bUpdatingTraitDependents = true;

	// Buildings a trait keeps from becoming obsolete (as CvTeam::changeObsoleteBuildingCount). A building processed here
	// can give or take away traits (FreeTraits), which can change other buildings: repeat until nothing changes.
	// abObsoleteBuilding follows what the cities have processed, so no building is processed twice.
	do
	{
		bChanged = false;

		for (iI = 0; iI < GC.getNumBuildingInfos() && iI < (int)abObsoleteBuilding.size(); iI++)
		{
			bObsolete = isObsoleteBuilding((BuildingTypes)iI);

			if (abObsoleteBuilding[iI] != bObsolete)
			{
				abObsoleteBuilding[iI] = bObsolete;
				bChanged = true;

				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					if (pLoopCity->getNumBuilding((BuildingTypes)iI) > 0)
					{
						pLoopCity->processBuilding((BuildingTypes)iI, (bObsolete ? -pLoopCity->getNumBuilding((BuildingTypes)iI) : pLoopCity->getNumBuilding((BuildingTypes)iI)), true);
					}
				}
			}
		}
	}
	while (bChanged);

	m_bUpdatingTraitDependents = false;

	updateMaxAnarchyTurns();

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		updateExtraYieldThreshold((YieldTypes)iI);
	}

	// City radius (iCityRadiusChange) and borders (iCultureRangeChange)
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updatePlotRadius();
		pLoopCity->updateCultureRange(true);
	}

	// Plot yields in our territory (TerrainYieldChanges, FeatureYieldChanges, ImprovementYieldChanges, BonusYieldChanges,
	// RiverPlotYieldChanges), also of the plots no city works
	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getOwnerINLINE() == getID())
		{
			pLoopPlot->updateYield();
		}
	}

	// Trade network (FeatureRoutes). Traits rarely change, so this is not limited to the traits with FeatureRoutes.
	updatePlotGroups();

	// Feature health in the city radius (FeatureHealthChanges), and the commerce for each religion (CommercePerReligion)
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateFeatureHealth();
		pLoopCity->updateReligionCommerce();
		pLoopCity->updateCommerce();	// (Global)WorldWonderCommerceChanges
		pLoopCity->updateMiseryChanges();	// AngryYieldChanges, HappyYieldPercents and the like
		pLoopCity->updateFeatureHappiness();	// FeatureHappinessChanges
		pLoopCity->updateExtraBuildingHappiness();	// BuildingHappinessChanges
		pLoopCity->updateExtraBuildingHealth();	// BuildingHealthChanges
		pLoopCity->updateReligionHappiness();	// iNonStateReligionHappiness
		pLoopCity->updateDynamicBuildingYields();	// YieldChanges, YieldPerReligion, (Global)WorldWonderYieldChanges
	}

	// Maintenance of the overseas cities (iOverseasMaintenanceModifier) and trade routes with distant cities
	// (iDistantTradeRouteModifier)
	updateMaintenance();
	updateTradeRoutes();

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->AI_setAssignWorkDirty(true);
		pLoopCity->setInfoDirty(true);
	}

	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}

// Python hooks of the traits (added by this mod): for each trait of the player with the expression of the hook
// (PyOnGameStart etc.), calls the function of CvTraitInterface.py that evaluates it. The arguments are the player and the
// trait, then the city (PyOnCityFounded, PyOnCityAcquired), then the old owner, bConquest and bTrade (PyOnCityAcquired).
// PyOnCityLost: the city (now of the new owner), the new owner (in eOldOwner), bConquest, bTrade, and the population and
// the culture level of the city before it was lost. PyOnBuildingBuilt: the city and the building it completed.
// Called where CvEventReporter reports the Python events, which happens the same way on every computer.
// PyOnDiscovered of a tech (added by this mod): CvHookInterface.py evaluates the expression with pPlayer, eTech and bFirst
// (the team of the player is the first to have it)
void CvPlayer::doTechPythonHook(TechTypes eTech, bool bFirst)
{
	if (isObserver())
	{
		return;
	}

	CyPlayer* pyPlayer = new CyPlayer(this);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlayer));
	argsList.add((int)eTech);
	argsList.add(bFirst ? 1 : 0);
	gDLL->getPythonIFace()->callFunction(PYHookModule, "onTechDiscovered", argsList.makeFunctionArgs());
	delete pyPlayer;	// the python function must not hold on to it
}

// PyOnAdopt (bAdopt) or PyOnRevoke of a civic (added by this mod): CvHookInterface.py evaluates the expression with pPlayer
// and eCivic
void CvPlayer::doCivicPythonHook(CivicTypes eCivic, bool bAdopt)
{
	if (isObserver())
	{
		return;
	}

	CyPlayer* pyPlayer = new CyPlayer(this);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlayer));
	argsList.add((int)eCivic);
	gDLL->getPythonIFace()->callFunction(PYHookModule, (bAdopt ? "onCivicAdopt" : "onCivicRevoke"), argsList.makeFunctionArgs());
	delete pyPlayer;	// the python function must not hold on to it
}

void CvPlayer::doTraitPythonHook(TraitPythonHookTypes eHook, CvCity* pCity, PlayerTypes eOldOwner, bool bConquest, bool bTrade, CorporationTypes eCorporation, int iPopulation, CultureLevelTypes eCultureLevel, BuildingTypes eBuilding, ReligionTypes eReligion)
{
	// Not for the player watching in the AI observe mode, who has nothing to act on
	if (isObserver())
	{
		return;
	}

	std::vector<int> aiArgs;

	if (eHook == TRAIT_PYHOOK_CITY_ACQUIRED)
	{
		aiArgs.push_back((int)eOldOwner);
		aiArgs.push_back(bConquest ? 1 : 0);
		aiArgs.push_back(bTrade ? 1 : 0);
	}
	else if (eHook == TRAIT_PYHOOK_CITY_LOST)
	{
		aiArgs.push_back((int)eOldOwner);
		aiArgs.push_back(bConquest ? 1 : 0);
		aiArgs.push_back(bTrade ? 1 : 0);
		aiArgs.push_back(iPopulation);
		aiArgs.push_back((int)eCultureLevel);
	}
	else if (eHook == TRAIT_PYHOOK_CORPORATION_FOUNDED)
	{
		aiArgs.push_back((int)eCorporation);
	}
	else if (eHook == TRAIT_PYHOOK_BUILDING_BUILT)
	{
		aiArgs.push_back((int)eBuilding);
	}
	else if (eHook == TRAIT_PYHOOK_RELIGION_FOUNDED || eHook == TRAIT_PYHOOK_LAST_RELIGION_CLAIMED)
	{
		aiArgs.push_back((int)eReligion);
	}
	else if (eHook == TRAIT_PYHOOK_OTHER_RELIGION_FOUNDED)
	{
		aiArgs.push_back((int)eReligion);
		aiArgs.push_back((int)eOldOwner);	// the founder
	}

	callTraitPythonHook(eHook, NULL, pCity, aiArgs);
}

// The trait hooks whose arguments after pPlayer and eTrait are up to two ints (added by this mod): PyOnTechAcquired (eTech,
// bFirst), PyOnGoldenAgeStart / End (none), PyOnWarDeclared (iOtherTeam, bAggressor), PyOnPeaceMade (iOtherTeam),
// PyOnEraChanged (eEra, iOldEra)
void CvPlayer::doTraitPythonHookInts(TraitPythonHookTypes eHook, int iNumArgs, int iArg1, int iArg2)
{
	std::vector<int> aiArgs;

	if (iNumArgs > 0)
	{
		aiArgs.push_back(iArg1);
	}
	if (iNumArgs > 1)
	{
		aiArgs.push_back(iArg2);
	}

	callTraitPythonHook(eHook, NULL, NULL, aiArgs);
}

// The common part of the trait hooks (added by this mod): for each trait of the player with the expression of the hook,
// calls the function of CvTraitInterface.py with pPlayer, eTrait, then the unit (PyOnGreatPersonBorn), the city (when
// pCity is given; always for PyOnGreatPersonBorn, which may have none: CyCity.isNone), then the ints of aiArgs
void CvPlayer::callTraitPythonHook(TraitPythonHookTypes eHook, CvUnit* pUnit, CvCity* pCity, const std::vector<int>& aiArgs)
{
	// Not for the player watching in the AI observe mode, who has nothing to act on
	if (isObserver())
	{
		return;
	}

	static const char* aszFunctions[NUM_TRAIT_PYHOOK_TYPES] = { "onGameStart", "onBeginTurn", "onCityFounded", "onCityAcquired", "onCorporationFounded", "onCityLost", "onBuildingBuilt", "onReligionFounded", "onGained", "onLost", "onTechAcquired", "onGreatPersonBorn", "onGoldenAgeStart", "onGoldenAgeEnd", "onWarDeclared", "onPeaceMade", "onEraChanged", "onFirstContact", "onProjectBuilt", "onVassalGained", "onReligionSpread", "onCivicChanged", "onCircumnavigated", "onOtherCityFounded", "onPlayerEliminated", "onImprovementChanged", "onLastReligionClaimed", "onOtherReligionFounded" };	// onGained and onLost: doTraitGainedPythonHook
	bool bHasUnit = (eHook == TRAIT_PYHOOK_GREAT_PERSON_BORN);
	bool bHasCity = (pCity != NULL || eHook == TRAIT_PYHOOK_GREAT_PERSON_BORN);
	CyPlayer* pyPlayer;
	CyUnit* pyUnit;
	CyCity* pyCity;
	int iI;
	int iJ;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI) && !CvString(GC.getTraitInfo((TraitTypes)iI).getPythonHook(eHook)).empty())
		{
			CyArgsList argsList;

			pyPlayer = new CyPlayer(this);
			argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlayer));
			argsList.add(iI);

			pyUnit = NULL;
			if (bHasUnit)
			{
				pyUnit = new CyUnit(pUnit);
				argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));
			}

			pyCity = NULL;
			if (bHasCity)
			{
				pyCity = new CyCity(pCity);
				argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));
			}

			for (iJ = 0; iJ < (int)aiArgs.size(); iJ++)
			{
				argsList.add(aiArgs[iJ]);
			}

			gDLL->getPythonIFace()->callFunction(PYTraitModule, aszFunctions[eHook], argsList.makeFunctionArgs());

			// The python functions must not hold on to these pointers
			delete pyPlayer;
			SAFE_DELETE(pyUnit);
			SAFE_DELETE(pyCity);
		}
	}
}

bool CvPlayer::hasPrereqAndCivics(BuildingTypes eBuilding) const
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iI;

	for (iI = 0; iI < kBuilding.getNumPrereqAndCivics(); iI++)
	{
		if (!isCivicOrReplacing((CivicTypes)kBuilding.getPrereqAndCivic(iI)))
		{
			return false;
		}
	}

	return true;
}

bool CvPlayer::hasPrereqOrCivics(BuildingTypes eBuilding) const
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iI;

	if (kBuilding.getNumPrereqOrCivics() == 0)
	{
		return true;
	}

	for (iI = 0; iI < kBuilding.getNumPrereqOrCivics(); iI++)
	{
		if (isCivicOrReplacing((CivicTypes)kBuilding.getPrereqOrCivic(iI)))
		{
			return true;
		}
	}

	return false;
}

// PrereqOrTraits of units, buildings and civics: an empty list needs no trait, otherwise one of the traits is enough
bool CvPlayer::hasPrereqOrTrait(const std::vector<int>& aiPrereqOrTraits) const
{
	int iI;

	if (aiPrereqOrTraits.empty())
	{
		return true;
	}

	for (iI = 0; iI < (int)aiPrereqOrTraits.size(); iI++)
	{
		if (hasTrait((TraitTypes)aiPrereqOrTraits[iI]))
		{
			return true;
		}
	}

	return false;
}

// A trait the player has says yes (added by this mod). False for an unused player slot (no leader)
bool CvPlayer::hasTraitFlag(bool (CvTraitInfo::*pGetter)() const) const
{
	int iI;

	if (getLeaderType() == NO_LEADER)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if ((GC.getTraitInfo((TraitTypes)iI).*pGetter)() && hasTrait((TraitTypes)iI))
		{
			return true;
		}
	}

	return false;
}

bool CvPlayer::hasTraitFlag(bool (CvTraitInfo::*pGetter)(int) const, int iArg) const
{
	int iI;

	if (getLeaderType() == NO_LEADER)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if ((GC.getTraitInfo((TraitTypes)iI).*pGetter)(iArg) && hasTrait((TraitTypes)iI))
		{
			return true;
		}
	}

	return false;
}

// The sum of a value of the traits the player has (added by this mod)
int CvPlayer::sumTraitValues(int (CvTraitInfo::*pGetter)() const) const
{
	int iSum = 0;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iSum += (GC.getTraitInfo((TraitTypes)iI).*pGetter)();
		}
	}

	return iSum;
}

int CvPlayer::sumTraitValues(int (CvTraitInfo::*pGetter)(int) const, int iArg) const
{
	int iSum = 0;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iSum += (GC.getTraitInfo((TraitTypes)iI).*pGetter)(iArg);
		}
	}

	return iSum;
}

int CvPlayer::sumTraitValues(int (CvTraitInfo::*pGetter)(int, int) const, int iArg1, int iArg2) const
{
	int iSum = 0;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iSum += (GC.getTraitInfo((TraitTypes)iI).*pGetter)(iArg1, iArg2);
		}
	}

	return iSum;
}

int CvPlayer::getUnitCombatFreeExperience(UnitCombatTypes eUnitCombat) const
{
	int iExperience = sumTraitValues(&CvTraitInfo::getUnitCombatFreeExperience, eUnitCombat);
	int iI;

	FAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat is expected to be within maximum bounds");

	for (iI = 0; iI < getNumCivicEntries(); iI++)	// every civic slot (changed by this mod)
	{
		if (getCivicByEntry(iI) != NO_CIVIC)
		{
			iExperience += GC.getCivicInfo(getCivicByEntry(iI)).getUnitCombatFreeExperience(eUnitCombat);
		}
	}

	return iExperience;
}

int CvPlayer::getUnitCombatProductionModifier(UnitCombatTypes eUnitCombat) const
{
	int iModifier = sumTraitValues(&CvTraitInfo::getUnitCombatProductionModifier, eUnitCombat);
	int iI;

	FAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat is expected to be within maximum bounds");

	for (iI = 0; iI < getNumCivicEntries(); iI++)	// every civic slot (changed by this mod)
	{
		if (getCivicByEntry(iI) != NO_CIVIC)
		{
			iModifier += GC.getCivicInfo(getCivicByEntry(iI)).getUnitCombatProductionModifier(eUnitCombat);
		}
	}

	return iModifier;
}

int CvPlayer::getUnitCombatUpgradeDiscount(UnitCombatTypes eUnitCombat) const
{
	int iDiscount = sumTraitValues(&CvTraitInfo::getUnitCombatUpgradeDiscount, eUnitCombat);
	int iI;

	FAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat is expected to be within maximum bounds");

	for (iI = 0; iI < getNumCivicEntries(); iI++)	// every civic slot (changed by this mod)
	{
		if (getCivicByEntry(iI) != NO_CIVIC)
		{
			iDiscount += GC.getCivicInfo(getCivicByEntry(iI)).getUnitCombatUpgradeDiscount(eUnitCombat);
		}
	}

	return iDiscount;
}

int CvPlayer::getTraitTerrainYieldChange(TerrainTypes eTerrain, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getTerrainYieldChange, eTerrain, eYield);
}

int CvPlayer::getTraitTerrainYieldChangePerEra(TerrainTypes eTerrain, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getTerrainYieldChangePerEra, eTerrain, eYield);
}

int CvPlayer::getTraitFeatureYieldChange(FeatureTypes eFeature, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getFeatureYieldChange, eFeature, eYield);
}

int CvPlayer::getTraitImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getImprovementYieldChanges, eImprovement, eYield);
}

int CvPlayer::getTraitRiverPlotYieldChange(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getRiverPlotYieldChange, eYield);
}

// RiverEdgeYieldPercents of the traits (added by this mod): see CvPlot::calculateTraitYieldChange
int CvPlayer::getTraitRiverEdgeYieldPercent(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getRiverEdgeYieldPercent, eYield);
}

int CvPlayer::getTraitLakePlotYieldChange(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getLakePlotYieldChange, eYield);
}

int CvPlayer::getTraitLakePlotYieldChangePerEra(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getLakePlotYieldChangePerEra, eYield);
}

// The traits let the units of the player enter the impassable pPlot in its territory (the caller checks the territory).
// Checked in the order of CvPlot::isImpassable: the peak (TERRAIN_PEAK), the feature, the terrain
bool CvPlayer::isTraitPassableInBorders(const CvPlot* pPlot) const
{
	if (pPlot->isPeak())
	{
		TerrainTypes ePeakTerrain = (TerrainTypes)GC.getInfoTypeForString("TERRAIN_PEAK", true);
		return ((ePeakTerrain != NO_TERRAIN) && hasTraitFlag(&CvTraitInfo::isPassableTerrainInBorders, ePeakTerrain));
	}

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		return hasTraitFlag(&CvTraitInfo::isPassableFeatureInBorders, pPlot->getFeatureType());
	}

	return hasTraitFlag(&CvTraitInfo::isPassableTerrainInBorders, pPlot->getTerrainType());
}

// CITY_PLOTS_DEFAULT_RADIUS and the iCityRadiusChange of the traits whose CityRadiusCultureLevel the city has
int CvPlayer::getCityRadius(CultureLevelTypes eCultureLevel) const
{
	int iRadius = CITY_PLOTS_DEFAULT_RADIUS;
	int iCultureLevel;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iCultureLevel = GC.getTraitInfo((TraitTypes)iI).getCityRadiusCultureLevel();

			if (iCultureLevel == NO_CULTURELEVEL || eCultureLevel >= iCultureLevel)
			{
				iRadius += GC.getTraitInfo((TraitTypes)iI).getCityRadiusChange();
			}
		}
	}

	return range(iRadius, 1, CITY_PLOTS_MAX_RADIUS);
}

int CvPlayer::getNextCityRadius() const
{
	return getCityRadius((CultureLevelTypes)(GC.getNumCultureLevelInfos() - 1));
}

int CvPlayer::getSummonDuration() const
{
	return std::max(0, sumTraitValues(&CvTraitInfo::getSummonDuration));
}

bool CvPlayer::isMaintainFeatures(FeatureTypes eFeature) const
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes)iI).isMaintainFeatures(eFeature))
			{
				return true;
			}
		}
	}

	return false;
}

// A trait keeps the building from becoming obsolete for this player (bIgnoreBuildingObsolescence,
// NoObsoleteBuildingClasses, NoObsoleteSpecialBuildings). Both obsolete techs (the building's and its special
// building's) are ignored.
bool CvPlayer::isIgnoreObsoleteBuilding(BuildingTypes eBuilding) const
{
	SpecialBuildingTypes eSpecialBuilding;
	int iBuildingClass;
	int iI;

	eSpecialBuilding = (SpecialBuildingTypes)GC.getBuildingInfo(eBuilding).getSpecialBuildingType();
	if (eSpecialBuilding != NO_SPECIALBUILDING && isIgnoreObsoleteSpecialBuilding(eSpecialBuilding))
	{
		return true;
	}

	iBuildingClass = GC.getBuildingInfo(eBuilding).getBuildingClassType();

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes)iI).isIgnoreBuildingObsolescence() || GC.getTraitInfo((TraitTypes)iI).isNoObsoleteBuildingClass(iBuildingClass))
			{
				return true;
			}
		}
	}

	return false;
}

// A trait keeps all the buildings of the special building type from becoming obsolete for this player
bool CvPlayer::isIgnoreObsoleteSpecialBuilding(SpecialBuildingTypes eSpecialBuilding) const
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes)iI).isIgnoreBuildingObsolescence() || GC.getTraitInfo((TraitTypes)iI).isNoObsoleteSpecialBuilding(eSpecialBuilding))
			{
				return true;
			}
		}
	}

	return false;
}

// Obsolete for the team (CvTeam::isObsoleteBuilding, from techs) and not kept by a trait of this player. The buildings
// of the cities use this, so a building can be obsolete in the cities of one team member and active in another's.
bool CvPlayer::isObsoleteBuilding(BuildingTypes eBuilding) const
{
	return (GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding) && !isIgnoreObsoleteBuilding(eBuilding));
}

// The most cities the player can found (added by this mod): the smallest city limit of the traits (iMaxCities,
// iMaxCitiesPercent) on the map size of the game. -1: no limit.
// The world wonders in all the cities of the player (added by this mod)
int CvPlayer::getNumWorldWonders() const
{
	CvCity* pLoopCity;
	int iCount = 0;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCount += pLoopCity->getNumWorldWonders();
	}

	return iCount;
}

// BuildingCombatPercents (added by this mod): the buildings of the class, or of all the classes of the special building, in
// all the cities of the player. getBuildingClassCount leaves out the obsolete buildings (CvCity::processBuilding), and keeps
// those a trait keeps from becoming obsolete. The class goes by its default building (special building and religion). With
// bStateReligion, only the classes of the state religion of the player count (none without a state religion).
int CvPlayer::getNumActiveBuildingsForCombat(BuildingClassTypes eBuildingClass, SpecialBuildingTypes eSpecialBuilding, bool bStateReligion) const
{
	BuildingTypes eDefaultBuilding;
	int iReligion;
	int iCount = 0;
	int iI;

	if (bStateReligion && getStateReligion() == NO_RELIGION)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		if (eBuildingClass != NO_BUILDINGCLASS && iI != eBuildingClass)
		{
			continue;
		}

		if (getBuildingClassCount((BuildingClassTypes)iI) <= 0)
		{
			continue;
		}

		eDefaultBuilding = (BuildingTypes)GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex();
		if (eDefaultBuilding == NO_BUILDING)
		{
			continue;
		}

		if (eBuildingClass == NO_BUILDINGCLASS && GC.getBuildingInfo(eDefaultBuilding).getSpecialBuildingType() != eSpecialBuilding)
		{
			continue;
		}

		if (bStateReligion)
		{
			iReligion = GC.getBuildingInfo(eDefaultBuilding).getReligionType();
			if (iReligion == NO_RELIGION)
			{
				iReligion = GC.getBuildingInfo(eDefaultBuilding).getPrereqReligion();
			}
			if (iReligion != getStateReligion())
			{
				continue;
			}
		}

		iCount += getBuildingClassCount((BuildingClassTypes)iI);
	}

	return iCount;
}

// After a world wonder or a building with the world wonder changes is added or removed (added by this mod): the yields
// and the building commerce of all the cities
// What GlobalBuildingExtraCommerces of the player's buildings add to one building of the class (added by this mod)
int CvPlayer::getBuildingClassExtraCommerce(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce) const
{
	return ((m_paiBuildingClassExtraCommerce != NULL) ? m_paiBuildingClassExtraCommerce[eBuildingClass * NUM_COMMERCE_TYPES + eCommerce] : 0);
}

void CvPlayer::changeBuildingClassExtraCommerce(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce, int iChange)
{
	CvCity* pLoopCity;
	int iLoop;

	if (iChange != 0 && m_paiBuildingClassExtraCommerce != NULL)
	{
		m_paiBuildingClassExtraCommerce[eBuildingClass * NUM_COMMERCE_TYPES + eCommerce] += iChange;

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->updateBuildingCommerce();
		}
	}
}

// SpecialistCommerceChanges of the traits (added by this mod): commerce each specialist of the type adds, as
// getSpecialistExtraCommerce does for all specialists. The cities add it to their base commerce.
int CvPlayer::getSpecialistTypeExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	FAssertMsg(eSpecialist >= 0 && eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist out of range");
	FAssertMsg(eCommerce >= 0 && eCommerce < NUM_COMMERCE_TYPES, "eCommerce out of range");
	return ((m_paiSpecialistTypeExtraCommerce != NULL) ? m_paiSpecialistTypeExtraCommerce[eSpecialist * NUM_COMMERCE_TYPES + eCommerce] : 0);
}

void CvPlayer::changeSpecialistTypeExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce, int iChange)
{
	if (iChange != 0 && m_paiSpecialistTypeExtraCommerce != NULL)
	{
		m_paiSpecialistTypeExtraCommerce[eSpecialist * NUM_COMMERCE_TYPES + eCommerce] += iChange;
		updateCommerce(eCommerce);
		AI_makeAssignWorkDirty();
	}
}

// The player effects of the bonuses (added by this mod, FFH2 applies them in the capital): the value of each bonus the capital
// has, counted once, or once per copy with bModifierPerBonus. 0 without a capital
int CvPlayer::getCapitalBonusEffect(int (CvBonusInfo::*pGetter)() const) const
{
	CvCity* pCapital = getCapitalCity();
	int iTotal = 0;
	int iValue;
	int iI;

	if (pCapital == NULL)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		iValue = (GC.getBonusInfo((BonusTypes)iI).*pGetter)();

		if (iValue != 0)
		{
			iTotal += iValue * pCapital->getBonusEffectCount((BonusTypes)iI);
		}
	}

	return iTotal;
}

// iHealChange of the bonuses (FFH2): heal of the player's units in friendly territory, % per turn
int CvPlayer::getBonusHealChange() const
{
	return getCapitalBonusEffect(&CvBonusInfo::getHealChange);
}

// iHealChangeEnemy of the bonuses (FFH2): heal of enemy units in the player's territory, % per turn
int CvPlayer::getBonusHealChangeEnemy() const
{
	return getCapitalBonusEffect(&CvBonusInfo::getHealChangeEnemy);
}

// iDiscoverRandModifier of the bonuses (FFH2): % to the chance an improvement of the player discovers a bonus
int CvPlayer::getBonusDiscoverRandModifier() const
{
	return getCapitalBonusEffect(&CvBonusInfo::getDiscoverRandModifier);
}

// Recalculates the tech changes of buildings in all the cities (added by this mod)
void CvPlayer::updateBuildingChangeEffects()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateBuildingCommerce();
		pLoopCity->updateDynamicBuildingYields();
		pLoopCity->updateExtraBuildingHappiness();
		pLoopCity->updateExtraBuildingHealth();
		pLoopCity->updateBuildingChangeSpecialists();
	}
}

void CvPlayer::updateWorldWonderEffects()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateDynamicBuildingYields();
		pLoopCity->updateBuildingCommerce();
		pLoopCity->updateCommerce();	// (Global)WorldWonderCommerceChanges of the traits, which are not building commerce
	}
}

// MIN_CITY_RANGE with iMinCityRangeChange of the traits (added by this mod): 0 lets cities be next to each other
int CvPlayer::getMinCityRange() const
{
	if (getLeaderType() == NO_LEADER)
	{
		return GC.getMIN_CITY_RANGE();
	}

	return std::max(0, GC.getMIN_CITY_RANGE() + sumTraitValues(&CvTraitInfo::getMinCityRangeChange));
}

// bFoundInForeignTerritory of the traits (added by this mod)
bool CvPlayer::isFoundInForeignTerritory() const
{
	return hasTraitFlag(&CvTraitInfo::isFoundInForeignTerritory);
}

// iGoldenAgeLandMovesChange / iGoldenAgeSeaMovesChange of the traits (added by this mod): the moves the units of the domain get
// during a golden age. CvUnit::baseMoves asks only during a golden age
int CvPlayer::getGoldenAgeMovesChange(DomainTypes eDomain) const
{
	if (getLeaderType() == NO_LEADER)
	{
		return 0;
	}

	switch (eDomain)
	{
	case DOMAIN_LAND:
		return sumTraitValues(&CvTraitInfo::getGoldenAgeLandMovesChange);
	case DOMAIN_SEA:
		return sumTraitValues(&CvTraitInfo::getGoldenAgeSeaMovesChange);
	default:
		return 0;
	}
}

// bInlandCanals of the traits (added by this mod)
bool CvPlayer::isInlandCanals() const
{
	return hasTraitFlag(&CvTraitInfo::isInlandCanals);
}

// bNoGlobalWarmingInBorders of the traits (added by this mod)
bool CvPlayer::isNoGlobalWarmingInBorders() const
{
	return hasTraitFlag(&CvTraitInfo::isNoGlobalWarmingInBorders);
}

// bNoPillageInBorders of the traits (added by this mod)
bool CvPlayer::isNoPillageInBorders() const
{
	return hasTraitFlag(&CvTraitInfo::isNoPillageInBorders);
}

// Gold this turn from GoldInterest/iPercent / GoldInterest/iMax of the traits (added by this mod): each trait gives its %
// of the treasury, at most its maximum. Part of calculateGoldRate, so doGold gives it and the interface shows it.
int CvPlayer::getGoldInterest() const
{
	int iInterest = 0;
	int iTraitInterest;
	int iI;

	if (getGold() <= 0)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI) && GC.getTraitInfo((TraitTypes)iI).getGoldInterestPercent() > 0)
		{
			iTraitInterest = (getGold() * GC.getTraitInfo((TraitTypes)iI).getGoldInterestPercent()) / 100;
			if (GC.getTraitInfo((TraitTypes)iI).getGoldInterestMax() > 0)
			{
				iTraitInterest = std::min(iTraitInterest, GC.getTraitInfo((TraitTypes)iI).getGoldInterestMax());
			}
			iInterest += iTraitInterest;
		}
	}

	return iInterest;
}

// PrereqOrTraits of the bonuses (added by this mod): the plots with the bonuses of the trait count for the team only while a
// player of the team has it. Also the obsolete bonuses the trait keeps (bIgnoreBonusObsolescence, NoObsoleteBonuses). bAdd
// false before the trait changes, true after it
void CvPlayer::updateTraitBonusPlots(TraitTypes eTrait, bool bAdd)
{
	std::vector<int> aiBonuses;
	int iI, iJ;

	// FreeBonuses (added by this mod): the trade network of the capital, which counts them (as the imported bonuses)
	if (GC.getTraitInfo(eTrait).isAnyFreeBonus() && getCapitalCity() != NULL)
	{
		getCapitalCity()->plot()->updatePlotGroupBonus(bAdd);
	}

	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (GC.getBonusInfo((BonusTypes)iI).getTechObsolete() != NO_TECH && GC.getTraitInfo(eTrait).isBonusObsolescenceIgnored(iI))
		{
			aiBonuses.push_back(iI);
			continue;
		}

		// RevealBonuses (added by this mod)
		if (GC.getTraitInfo(eTrait).isRevealBonus(iI))
		{
			aiBonuses.push_back(iI);
			continue;
		}

		for (iJ = 0; iJ < GC.getBonusInfo((BonusTypes)iI).getNumPrereqOrTraits(); iJ++)
		{
			if (GC.getBonusInfo((BonusTypes)iI).getPrereqOrTraits(iJ) == eTrait)
			{
				aiBonuses.push_back(iI);
				break;
			}
		}
	}

	if (aiBonuses.empty())
	{
		return;
	}

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getBonusType() != NO_BONUS && std::find(aiBonuses.begin(), aiBonuses.end(), (int)pLoopPlot->getBonusType()) != aiBonuses.end())
		{
			if (pLoopPlot->getTeam() == getTeam())
			{
				pLoopPlot->updatePlotGroupBonus(bAdd);
			}

			if (bAdd)
			{
				pLoopPlot->updateYield();
				pLoopPlot->setLayoutDirty(true);
			}
		}
	}
}

// The active player in the AI observe mode (added by this mod): alive with nothing, and out of the game for everyone else
bool CvPlayer::isObserver() const
{
	return (GC.getGameINLINE().isObserveMode() && getID() == GC.getGameINLINE().getActivePlayer());
}

// ReplacedCivic (added by this mod): the player can adopt a civic that replaces this one. A replacement the player cannot
// adopt yet, for lack of the tech, leaves the replaced civic available. A civic can only replace an earlier one, so going
// through canDoCivics ends
bool CvPlayer::isCivicReplaced(CivicTypes eCivic) const
{
	int iI;

	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getCivicInfo((CivicTypes)iI).getReplacedCivic() == eCivic && canDoCivics((CivicTypes)iI))
		{
			return true;
		}
	}

	return false;
}

// The traits eCivic gives this player through bFavoriteCivic of ConditionalFreeTraits (added by this mod): it is the favorite
// civic of the leader or replaces it (ReplacedCivic). For the displays, it does not look at the other conditions of the items
void CvPlayer::getFavoriteCivicTraits(CivicTypes eCivic, std::vector<TraitTypes>& aeTraits) const
{
	int iLoopCivic;
	int iI, iJ;

	aeTraits.clear();

	if (getLeaderType() == NO_LEADER || GC.getLeaderHeadInfo(getLeaderType()).getFavoriteCivic() == NO_CIVIC)
	{
		return;
	}

	// A civic can only replace an earlier one, so this ends
	iLoopCivic = eCivic;
	while (iLoopCivic != NO_CIVIC && iLoopCivic != GC.getLeaderHeadInfo(getLeaderType()).getFavoriteCivic())
	{
		iLoopCivic = GC.getCivicInfo((CivicTypes)iLoopCivic).getReplacedCivic();
	}

	if (iLoopCivic == NO_CIVIC)
	{
		return;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			for (iJ = 0; iJ < GC.getTraitInfo((TraitTypes)iI).getNumConditionalFreeTraits(); iJ++)
			{
				const ConditionalFreeTrait& kCondition = GC.getTraitInfo((TraitTypes)iI).getConditionalFreeTrait(iJ);
				if (kCondition.bFavoriteCivic && std::find(aeTraits.begin(), aeTraits.end(), (TraitTypes)kCondition.iTrait) == aeTraits.end())
				{
					aeTraits.push_back((TraitTypes)kCondition.iTrait);
				}
			}
		}
	}
}

bool CvPlayer::isFavoriteCivicWithTraits(CivicTypes eCivic) const
{
	std::vector<TraitTypes> aeTraits;

	getFavoriteCivicTraits(eCivic, aeTraits);
	return !aeTraits.empty();
}

// For the lists that show what a player will get with the techs, as canBuildByTraits (added by this mod)
bool CvPlayer::canDoCivicByTraits(CivicTypes eCivic) const
{
	int iI;

	if (!hasPrereqOrTrait(GC.getCivicInfo(eCivic).getPrereqOrTraitsVector()))
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getCivicInfo((CivicTypes)iI).getReplacedCivic() == eCivic && hasPrereqOrTrait(GC.getCivicInfo((CivicTypes)iI).getPrereqOrTraitsVector()))
		{
			return false;
		}
	}

	return true;
}

// isCivic, and a civic that replaces this one counts as this one for what asks for it (added by this mod)
bool CvPlayer::isCivicOrReplacing(CivicTypes eCivic) const
{
	int iI;

	if (isCivic(eCivic))
	{
		return true;
	}

	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getCivicInfo((CivicTypes)iI).getReplacedCivic() == eCivic && isCivicOrReplacing((CivicTypes)iI))
		{
			return true;
		}
	}

	return false;
}

// The initial civic of the civilization, or what replaces it for this player without needing a tech (added by this mod)
CivicTypes CvPlayer::getInitialCivic(CivicOptionTypes eCivicOption) const
{
	CivicTypes eCivic = (CivicTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(eCivicOption);
	int iI;

	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getCivicInfo((CivicTypes)iI).getReplacedCivic() == eCivic && GC.getCivicInfo((CivicTypes)iI).getTechPrereq() == NO_TECH && hasPrereqOrTrait(GC.getCivicInfo((CivicTypes)iI).getPrereqOrTraitsVector()))
		{
			eCivic = (CivicTypes)iI;
		}
	}

	return eCivic;
}

// The unit of the unit class for this player (added by this mod): UniqueUnits of the traits, then the civilization, then the unit
// learned by conquest (bConquestLearnUniqueUnits) for a class where the first two give the default unit
int CvPlayer::getCivilizationUnit(int iUnitClass) const
{
	int iUnit;

	iUnit = getNativeCivilizationUnit(iUnitClass);

	if (iUnitClass >= 0 && m_paiLearnedUnit != NULL && m_paiLearnedUnit[iUnitClass] != NO_UNIT)
	{
		if (iUnit == GC.getUnitClassInfo((UnitClassTypes)iUnitClass).getDefaultUnitIndex() && isConquestLearnUniqueUnits())
		{
			return m_paiLearnedUnit[iUnitClass];
		}
	}

	return iUnit;
}

// The same for buildings (added by this mod)
int CvPlayer::getCivilizationBuilding(int iBuildingClass) const
{
	int iBuilding;

	iBuilding = getNativeCivilizationBuilding(iBuildingClass);

	if (iBuildingClass >= 0 && m_paiLearnedBuilding != NULL && m_paiLearnedBuilding[iBuildingClass] != NO_BUILDING)
	{
		if (iBuilding == GC.getBuildingClassInfo((BuildingClassTypes)iBuildingClass).getDefaultBuildingIndex() && isConquestLearnUniqueBuildings())
		{
			return m_paiLearnedBuilding[iBuildingClass];
		}
	}

	return iBuilding;
}

// The unit of the unit class without the learned ones (added by this mod): UniqueUnits of the traits, before the civilization.
// The first trait in the order of the XML wins
int CvPlayer::getNativeCivilizationUnit(int iUnitClass) const
{
	int iUnit;
	int iI;

	if (getLeaderType() != NO_LEADER)
	{
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GC.getTraitInfo((TraitTypes)iI).getNumUniqueUnits() > 0 && hasTrait((TraitTypes)iI))
			{
				iUnit = GC.getTraitInfo((TraitTypes)iI).getUniqueUnitForClass(iUnitClass);
				if (iUnit != NO_UNIT)
				{
					return iUnit;
				}
			}
		}
	}

	return GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitClass);
}

// The same for UniqueBuildings (added by this mod)
int CvPlayer::getNativeCivilizationBuilding(int iBuildingClass) const
{
	int iBuilding;
	int iI;

	if (getLeaderType() != NO_LEADER)
	{
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GC.getTraitInfo((TraitTypes)iI).getNumUniqueBuildings() > 0 && hasTrait((TraitTypes)iI))
			{
				iBuilding = GC.getTraitInfo((TraitTypes)iI).getUniqueBuildingForClass(iBuildingClass);
				if (iBuilding != NO_BUILDING)
				{
					return iBuilding;
				}
			}
		}
	}

	return GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iBuildingClass);
}

// bConquestLearnUniqueUnits and bConquestLearnUniqueBuildings of the traits (added by this mod): see learnConquestUniques
bool CvPlayer::isConquestLearnUniqueUnits() const
{
	return hasTraitFlag(&CvTraitInfo::isConquestLearnUniqueUnits);
}

bool CvPlayer::isConquestLearnUniqueBuildings() const
{
	return hasTraitFlag(&CvTraitInfo::isConquestLearnUniqueBuildings);
}

int CvPlayer::getLearnedUnit(int iUnitClass) const
{
	return ((m_paiLearnedUnit != NULL && iUnitClass >= 0 && iUnitClass < GC.getNumUnitClassInfos()) ? m_paiLearnedUnit[iUnitClass] : NO_UNIT);
}

int CvPlayer::getLearnedBuilding(int iBuildingClass) const
{
	return ((m_paiLearnedBuilding != NULL && iBuildingClass >= 0 && iBuildingClass < GC.getNumBuildingClassInfos()) ? m_paiLearnedBuilding[iBuildingClass] : NO_BUILDING);
}

// bGainTraitOnCapitalCapture (bAll false: acquireCity, when this player conquers the capital of eFrom),
// bGainTraitsOnElimination (bAll true: setAlive, when eFrom is eliminated) and GainTraitsOnFriendly (doGainTraitsOnFriendly, when
// eFrom becomes friendly) of the traits (added by this mod). The candidates are
// the basic traits (bBasicTrait) of the leader and the civilization of eFrom (hasBaseTrait, not the traits eFrom gained during
// the game) that this player does not have yet. bAll false gives one of them at random, bAll true gives all of them. Nothing
// when there is no candidate. The traits are given for good (changeFreeTraitCount), with the message szMessageKey (%s1 the leader
// of eFrom, %s2 the trait) to this player
void CvPlayer::gainBasicTraitsOf(PlayerTypes eFrom, bool bAll, const char* szMessageKey)
{
	std::vector<TraitTypes> aeTraits;
	CvWString szBuffer;
	TraitTypes eTrait;
	int iI;

	if (eFrom == NO_PLAYER || eFrom == getID() || GET_PLAYER(eFrom).getLeaderType() == NO_LEADER)
	{
		return;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).isBasicTrait() && GET_PLAYER(eFrom).hasBaseTrait((TraitTypes)iI) && !hasTrait((TraitTypes)iI))
		{
			aeTraits.push_back((TraitTypes)iI);
		}
	}

	if (aeTraits.empty())
	{
		return;
	}

	if (!bAll)
	{
		eTrait = aeTraits[GC.getGameINLINE().getSorenRandNum((int)aeTraits.size(), "Gain basic trait")];
		aeTraits.clear();
		aeTraits.push_back(eTrait);
	}

	for (iI = 0; iI < (int)aeTraits.size(); iI++)
	{
		changeFreeTraitCount(aeTraits[iI], 1);

		szBuffer = gDLL->getText(szMessageKey, GET_PLAYER(eFrom).getNameKey(), GC.getTraitInfo(aeTraits[iI]).getTextKeyWide());
		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
	}
}

// bConquestLearnUniqueUnits and bConquestLearnUniqueBuildings of the traits (added by this mod): called by acquireCity when this
// player conquers a city of eOldOwner, before the buildings of the city are given to this player. This player learns the unique
// units and buildings of the civilization of eOldOwner (CIV4CivilizationInfos.xml, not the traits of eOldOwner) for the classes
// where it has no unique of its own (getNativeCivilizationUnit / getNativeCivilizationBuilding give the default one). A later
// one replaces an earlier one of the same class. The existing units stay as they are. In the cities of this player, the
// buildings of the class are rebuilt as the learned one, and the orders and the production of the old unit or building move to
// the learned one. The free buildings (setNumFreeBuilding) stay as they are, since the things that give them take them back
void CvPlayer::learnConquestUniques(PlayerTypes eOldOwner)
{
	CLLNode<OrderData>* pOrderNode;
	CvCity* pLoopCity;
	CvWString szList;
	CvWString szBuffer;
	CivilizationTypes eOldCiv;
	UnitTypes eOldUnit;
	UnitTypes eNewUnit;
	BuildingTypes eOldBuilding;
	BuildingTypes eNewBuilding;
	PlayerTypes eBuildingOwner;
	bool bUnits;
	bool bBuildings;
	int iNum;
	int iTime;
	int iLoop;
	int iI;

	if (eOldOwner == NO_PLAYER || eOldOwner == getID() || GET_PLAYER(eOldOwner).isBarbarian())
	{
		return;
	}

	if (m_paiLearnedUnit == NULL || m_paiLearnedBuilding == NULL)
	{
		return;
	}

	bUnits = isConquestLearnUniqueUnits();
	bBuildings = isConquestLearnUniqueBuildings();
	eOldCiv = GET_PLAYER(eOldOwner).getCivilizationType();

	if ((!bUnits && !bBuildings) || eOldCiv == NO_CIVILIZATION)
	{
		return;
	}

	if (bUnits)
	{
		for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			eNewUnit = (UnitTypes)GC.getCivilizationInfo(eOldCiv).getCivilizationUnits(iI);

			if (eNewUnit == NO_UNIT || eNewUnit == GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex() || eNewUnit == m_paiLearnedUnit[iI])
			{
				continue;
			}

			if (getNativeCivilizationUnit(iI) != GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex())
			{
				continue;	// a unique of its own (or a class the player cannot train)
			}

			eOldUnit = (UnitTypes)getCivilizationUnit(iI);
			m_paiLearnedUnit[iI] = eNewUnit;

			if (eOldUnit != NO_UNIT && eOldUnit != eNewUnit)
			{
				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					for (pOrderNode = pLoopCity->headOrderQueueNode(); pOrderNode != NULL; pOrderNode = pLoopCity->nextOrderQueueNode(pOrderNode))
					{
						if (pOrderNode->m_data.eOrderType == ORDER_TRAIN && pOrderNode->m_data.iData1 == eOldUnit)
						{
							pOrderNode->m_data.iData1 = eNewUnit;
						}
					}

					if (pLoopCity->getUnitProduction(eOldUnit) > 0)
					{
						pLoopCity->changeUnitProduction(eNewUnit, pLoopCity->getUnitProduction(eOldUnit));
						pLoopCity->setUnitProductionTime(eNewUnit, pLoopCity->getUnitProductionTime(eOldUnit));
						pLoopCity->setUnitProduction(eOldUnit, 0);
						pLoopCity->setUnitProductionTime(eOldUnit, 0);
					}
				}
			}

			if (!szList.empty())
			{
				szList += L", ";
			}
			szList += GC.getUnitInfo(eNewUnit).getDescription();
		}
	}

	if (bBuildings)
	{
		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			eNewBuilding = (BuildingTypes)GC.getCivilizationInfo(eOldCiv).getCivilizationBuildings(iI);

			if (eNewBuilding == NO_BUILDING || eNewBuilding == GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex() || eNewBuilding == m_paiLearnedBuilding[iI])
			{
				continue;
			}

			if (getNativeCivilizationBuilding(iI) != GC.getBuildingClassInfo((BuildingClassTypes)iI).getDefaultBuildingIndex())
			{
				continue;	// a unique of its own (or a class the player cannot build)
			}

			eOldBuilding = (BuildingTypes)getCivilizationBuilding(iI);
			m_paiLearnedBuilding[iI] = eNewBuilding;

			if (eOldBuilding != NO_BUILDING && eOldBuilding != eNewBuilding)
			{
				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					// rebuilt even where the learned one could not be built (bWater and so on)
					iNum = pLoopCity->getNumRealBuilding(eOldBuilding);
					if (iNum > 0)
					{
						eBuildingOwner = (PlayerTypes)pLoopCity->getBuildingOriginalOwner(eOldBuilding);
						iTime = pLoopCity->getBuildingOriginalTime(eOldBuilding);
						pLoopCity->setNumRealBuilding(eOldBuilding, 0);
						pLoopCity->setNumRealBuildingTimed(eNewBuilding, std::min(pLoopCity->getNumRealBuilding(eNewBuilding) + iNum, GC.getCITY_MAX_NUM_BUILDINGS()), false, eBuildingOwner, iTime);
						pLoopCity->AI_setAssignWorkDirty(true);
						pLoopCity->setInfoDirty(true);
					}

					for (pOrderNode = pLoopCity->headOrderQueueNode(); pOrderNode != NULL; pOrderNode = pLoopCity->nextOrderQueueNode(pOrderNode))
					{
						if (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT && pOrderNode->m_data.iData1 == eOldBuilding)
						{
							pOrderNode->m_data.iData1 = eNewBuilding;
						}
					}

					if (pLoopCity->getBuildingProduction(eOldBuilding) > 0)
					{
						pLoopCity->changeBuildingProduction(eNewBuilding, pLoopCity->getBuildingProduction(eOldBuilding));
						pLoopCity->setBuildingProductionTime(eNewBuilding, pLoopCity->getBuildingProductionTime(eOldBuilding));
						pLoopCity->setBuildingProduction(eOldBuilding, 0);
						pLoopCity->setBuildingProductionTime(eOldBuilding, 0);
					}
				}
			}

			if (!szList.empty())
			{
				szList += L", ";
			}
			szList += GC.getBuildingInfo(eNewBuilding).getDescription();
		}
	}

	if (!szList.empty())
	{
		szBuffer = gDLL->getText("TXT_KEY_MISC_LEARNED_CONQUEST_UNIQUES", GC.getCivilizationInfo(eOldCiv).getShortDescriptionKey(), szList.GetCString());
		gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}

// iStateReligionHealth of the traits (added by this mod): health of the cities with the state religion (CvCity::goodHealth)
int CvPlayer::getTraitStateReligionHealth() const
{
	return sumTraitValues(&CvTraitInfo::getStateReligionHealth);
}

// GreatPeopleClassRateModifiers of the traits (added by this mod): % of the great people points of the unit class
int CvPlayer::getGreatPeopleClassRateModifier(UnitClassTypes eUnitClass) const
{
	return sumTraitValues(&CvTraitInfo::getGreatPeopleClassRateModifier, eUnitClass);
}

bool CvPlayer::isAnyGreatPeopleClassRateModifier() const
{
	return hasTraitFlag(&CvTraitInfo::isAnyGreatPeopleClassRateModifier);
}

// iEnemyMoveCostInBorders of the traits (added by this mod): see CvPlot::movementCost
int CvPlayer::getEnemyMoveCostInBorders() const
{
	return sumTraitValues(&CvTraitInfo::getEnemyMoveCostInBorders);
}

// iRevengeCombatPercent of the traits (added by this mod): combat % of the units of this player against the units of ePlayer,
// when ePlayer has ever conquered a city of this player (acquireCity records it)
int CvPlayer::getRevengeCombatModifier(PlayerTypes ePlayer) const
{
	if (ePlayer == NO_PLAYER || ePlayer == getID() || !isCityConqueredBy(ePlayer))
	{
		return 0;
	}

	return sumTraitValues(&CvTraitInfo::getRevengeCombatPercent);
}

bool CvPlayer::isCityConqueredBy(PlayerTypes ePlayer) const
{
	return (ePlayer >= 0 && ePlayer < MAX_PLAYERS) ? m_abCityConqueredBy[ePlayer] : false;
}

void CvPlayer::setCityConqueredBy(PlayerTypes ePlayer, bool bNewValue)
{
	if (ePlayer >= 0 && ePlayer < MAX_PLAYERS && ePlayer != getID())
	{
		m_abCityConqueredBy[ePlayer] = bNewValue;
	}
}

// SpecialBuildingProductionModifiers of the traits (added by this mod): see CvCity::getProductionModifier
int CvPlayer::getSpecialBuildingProductionModifier(SpecialBuildingTypes eSpecialBuilding) const
{
	if (eSpecialBuilding == NO_SPECIALBUILDING)
	{
		return 0;
	}

	return sumTraitValues(&CvTraitInfo::getSpecialBuildingProductionModifier, eSpecialBuilding);
}

// KnownTechCost/iModifier of the traits (added by this mod): see CvTeam::getResearchCost
int CvPlayer::getKnownTechCostModifier() const
{
	return sumTraitValues(&CvTraitInfo::getKnownTechCostModifier);
}

// KnownTechCost/iLimit of the traits with KnownTechCost/iModifier (added by this mod): the lowest one, -100 to 0. 0 means no limit
// (the cost goes down to the minimum of 1)
int CvPlayer::getKnownTechCostLimit() const
{
	int iLimit = 0;
	bool bNoLimit = false;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).getKnownTechCostModifier() != 0 && hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes)iI).getKnownTechCostLimit() == 0)
			{
				bNoLimit = true;
			}
			iLimit = std::min(iLimit, GC.getTraitInfo((TraitTypes)iI).getKnownTechCostLimit());
		}
	}

	return bNoLimit ? -100 : range(iLimit, -100, 0);
}

// bConquestHeal of the traits (added by this mod): see CvUnit::setXY
bool CvPlayer::isConquestHeal() const
{
	return hasTraitFlag(&CvTraitInfo::isConquestHeal);
}

// bStackCityDefense of the traits (added by this mod): see CvCity::getTotalDefense
bool CvPlayer::isStackCityDefense() const
{
	return hasTraitFlag(&CvTraitInfo::isStackCityDefense);
}

// bRevealWonderProduction of the traits (added by this mod): see CvGameTextMgr::getRevealedPlansString
bool CvPlayer::isRevealWonderProduction() const
{
	return hasTraitFlag(&CvTraitInfo::isRevealWonderProduction);
}

// bRevealWarPlans of the traits (added by this mod): see CvGameTextMgr::getRevealedPlansString
bool CvPlayer::isRevealWarPlans() const
{
	return hasTraitFlag(&CvTraitInfo::isRevealWarPlans);
}

// bRevealHiddenAttitude of the traits (added by this mod): see CvGameTextMgr::getAttitudeString
bool CvPlayer::isRevealHiddenAttitude() const
{
	return hasTraitFlag(&CvTraitInfo::isRevealHiddenAttitude);
}

// bLongCountCalendar of the traits (added by this mod): see CvGameTextMgr::setInterfaceTime
bool CvPlayer::isLongCountCalendar() const
{
	return hasTraitFlag(&CvTraitInfo::isLongCountCalendar);
}

// bImprovementUpgradeUnworked of the traits (added by this mod): see CvPlot::doTurn
bool CvPlayer::isImprovementUpgradeUnworked() const
{
	return hasTraitFlag(&CvTraitInfo::isImprovementUpgradeUnworked);
}

// bUnsettleGreatPeople of the traits (added by this mod): see CvCity::unsettleGreatPerson
bool CvPlayer::isUnsettleGreatPeople() const
{
	return hasTraitFlag(&CvTraitInfo::isUnsettleGreatPeople);
}

// bClaimLastReligion of the traits (added by this mod): see CvTeam::getLastReligionClaim
bool CvPlayer::isClaimLastReligion() const
{
	return hasTraitFlag(&CvTraitInfo::isClaimLastReligion);
}

// bClaimLastReligion of the traits (added by this mod): the player can take the last religion now (alive, not a barbarian,
// with a city for the holy city)
bool CvPlayer::canClaimLastReligion() const
{
	return (isAlive() && !isBarbarian() && getNumCities() > 0 && isClaimLastReligion());
}

// bClaimLastReligion of the traits (added by this mod): the team of the player gets the tech of eSlotReligion (the slot
// CvTeam::setHasTech reserved for this player) and the player founds the religion, with the free units, then
// PyOnLastReligionClaimed. With the option to choose religions, the religion is the one the AI would choose, also for a
// human player (no popup)
void CvPlayer::claimLastReligion(ReligionTypes eSlotReligion)
{
	TechTypes eTech = (TechTypes)GC.getReligionInfo(eSlotReligion).getTechPrereq();
	ReligionTypes eReligion = eSlotReligion;
	CvCity* pHolyCity;

	// The slot is taken, so the tech does not found the religion for another player of the team
	if (eTech != NO_TECH && !GET_TEAM(getTeam()).isHasTech(eTech))
	{
		GET_TEAM(getTeam()).setHasTech(eTech, true, getID(), true, true);
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_PICK_RELIGION))
	{
		eReligion = AI_chooseReligion();
	}

	if (eReligion == NO_RELIGION || GC.getGameINLINE().isReligionFounded(eReligion))
	{
		return;
	}

	foundReligion(eReligion, eSlotReligion, true);

	pHolyCity = GC.getGameINLINE().getHolyCity(eReligion);
	if (pHolyCity != NULL && pHolyCity->getOwnerINLINE() == getID())
	{
		doTraitPythonHook(TRAIT_PYHOOK_LAST_RELIGION_CLAIMED, pHolyCity, NO_PLAYER, false, false, NO_CORPORATION, 0, NO_CULTURELEVEL, NO_BUILDING, eReligion);
	}
}

// GreatPeopleHurry/iGoldPerPoint of the traits (added by this mod): gold per great people point of the purchase from the
// great people bar of a city. 0 or less: no purchase. See CvCity::hurryGreatPeople
int CvPlayer::getGreatPeopleHurryGold() const
{
	return sumTraitValues(&CvTraitInfo::getGreatPeopleHurryGold);
}

// GreatPeopleHurry/UnitClassType of the traits (added by this mod): the great person unit of the purchase, the unit of the
// civilization for the class of the first trait with a purchase and a class. NO_UNIT: by the shares of the city
UnitTypes CvPlayer::getGreatPeopleHurryUnit() const
{
	int iI;

	if (getCivilizationType() == NO_CIVILIZATION || getLeaderType() == NO_LEADER)
	{
		return NO_UNIT;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.getGreatPeopleHurryGold() > 0 && kTrait.getGreatPeopleHurryUnitClass() != NO_UNITCLASS && hasTrait((TraitTypes)iI))
		{
			return (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kTrait.getGreatPeopleHurryUnitClass());
		}
	}

	return NO_UNIT;
}

// StackPromotions of the traits (added by this mod)
bool CvPlayer::isAnyStackPromotions() const
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).getNumStackPromotionSets() > 0 && hasTrait((TraitTypes)iI))
		{
			return true;
		}
	}

	return false;
}

static bool isInTypeList(const std::vector<int>& aiTypes, int iType)
{
	return (std::find(aiTypes.begin(), aiTypes.end(), iType) != aiTypes.end());
}

// The unit gets the promotions of the set: it has a unit combat, one of StackUnitCombats when they are given
static bool isStackPromotionSetTarget(const StackPromotionSet& kSet, const CvUnit* pUnit)
{
	if (pUnit->getUnitCombatType() == NO_UNITCOMBAT)
	{
		return false;
	}

	return (kSet.aiUnitCombats.empty() || isInTypeList(kSet.aiUnitCombats, pUnit->getUnitCombatType()));
}

// StackPromotions of the traits (added by this mod): while a group moves unit by unit (CvSelectionGroup::groupMove), the
// promotions are not updated, and the plots are updated after all the units have moved. Otherwise the units still on the old
// plot lose the promotions when the great general (or another unit of a set) moves first, and a unit whose moves come from
// the promotions cannot move any more and is split from the group
void CvPlayer::setStackPromotionsDeferred(bool bNewValue)
{
	m_bStackPromotionsDeferred = bNewValue;
}

// StackPromotions of the traits (added by this mod): our units on the plot have the promotions of a set while another unit
// of ours of one of its classes is on the plot (StackUnitClasses), when they are its targets (StackUnitCombats), and lose
// them otherwise. Called when a unit enters or leaves the plot (CvUnit::setXY) and when a trait with StackPromotions is
// gained or lost. With bStackUnitsToo, the units of its classes have the promotions too while one of its targets (another unit)
// is on the plot. The promotions should be used only for this (state promotions without UnitCombats), because they are
// removed without checking how the unit got them
void CvPlayer::updateStackPromotions(CvPlot* pPlot)
{
	std::vector<const StackPromotionSet*> apSets;
	std::vector<int> aiPromotions;
	std::vector<CvUnit*> apUnits;
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	bool bHas;
	int iI;
	int iJ;
	int iK;
	int iP;
	int iS;

	if (pPlot == NULL || m_bStackPromotionsDeferred || !isAnyStackPromotions())
	{
		return;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			for (iJ = 0; iJ < GC.getTraitInfo((TraitTypes)iI).getNumStackPromotionSets(); iJ++)
			{
				const StackPromotionSet& kSet = GC.getTraitInfo((TraitTypes)iI).getStackPromotionSet(iJ);
				apSets.push_back(&kSet);
				for (iK = 0; iK < (int)kSet.aiPromotions.size(); iK++)
				{
					if (!isInTypeList(aiPromotions, kSet.aiPromotions[iK]))
					{
						aiPromotions.push_back(kSet.aiPromotions[iK]);
					}
				}
			}
		}
	}

	pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);
		if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getID() && !pLoopUnit->isDelayedDeath())
		{
			apUnits.push_back(pLoopUnit);
		}
	}

	for (iK = 0; iK < (int)apUnits.size(); iK++)
	{
		pLoopUnit = apUnits[iK];

		for (iP = 0; iP < (int)aiPromotions.size(); iP++)
		{
			// the promotion may be in several sets: it is kept when any of them applies
			bHas = false;
			for (iS = 0; iS < (int)apSets.size() && !bHas; iS++)
			{
				if (!isInTypeList(apSets[iS]->aiPromotions, aiPromotions[iP]))
				{
					continue;
				}

				if (isStackPromotionSetTarget(*apSets[iS], pLoopUnit))
				{
					for (iI = 0; iI < (int)apUnits.size(); iI++)
					{
						if (iI != iK && isInTypeList(apSets[iS]->aiUnitClasses, apUnits[iI]->getUnitClassType()))
						{
							bHas = true;
							break;
						}
					}
				}

				// bStackUnitsToo (added by this mod): a unit of the classes, while a target is with it
				if (!bHas && apSets[iS]->bStackUnitsToo && isInTypeList(apSets[iS]->aiUnitClasses, pLoopUnit->getUnitClassType()))
				{
					for (iI = 0; iI < (int)apUnits.size(); iI++)
					{
						if (iI != iK && isStackPromotionSetTarget(*apSets[iS], apUnits[iI]))
						{
							bHas = true;
							break;
						}
					}
				}
			}

			if (pLoopUnit->isHasPromotion((PromotionTypes)aiPromotions[iP]) != bHas)
			{
				pLoopUnit->setHasPromotion((PromotionTypes)aiPromotions[iP], bHas);
			}
		}
	}
}

// After a trait with StackPromotions is gained or lost (added by this mod). Removes the promotions of the lost trait too
void CvPlayer::updateAllStackPromotions(TraitTypes eTrait, int iChange)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
	CvUnit* pLoopUnit;
	PromotionTypes ePromotion;
	int iLoop;
	int iJ;
	int iK;

	if (iChange < 0)
	{
		for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			for (iJ = 0; iJ < kTrait.getNumStackPromotionSets(); iJ++)
			{
				for (iK = 0; iK < kTrait.getStackPromotionSetNumPromotions(iJ); iK++)
				{
					ePromotion = (PromotionTypes)kTrait.getStackPromotionSetPromotion(iJ, iK);
					if (pLoopUnit->isHasPromotion(ePromotion))
					{
						pLoopUnit->setHasPromotion(ePromotion, false);
					}
				}
			}
		}
	}

	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		updateStackPromotions(pLoopUnit->plot());
	}
}

// iDomesticGreatGeneralRateModifier of the specialists (added by this mod): great general points of the combat on a plot in
// the radius of our cities, from the specialists of those cities (a Great Wall for one city)
int CvPlayer::getCityRadiusGreatGeneralRateModifier(const CvPlot* pPlot) const
{
	CvCity* pLoopCity;
	int iModifier = 0;
	int iCityModifier;
	int iIndex;
	int iLoop;
	int iI;

	if (pPlot == NULL)
	{
		return 0;
	}

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCityModifier = 0;
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (GC.getSpecialistInfo((SpecialistTypes)iI).getDomesticGreatGeneralRateModifier() != 0)
			{
				iCityModifier += (pLoopCity->getSpecialistCount((SpecialistTypes)iI) + pLoopCity->getFreeSpecialistCount((SpecialistTypes)iI)) * GC.getSpecialistInfo((SpecialistTypes)iI).getDomesticGreatGeneralRateModifier();
			}
		}

		if (iCityModifier != 0)
		{
			iIndex = pLoopCity->getCityPlotIndex(pPlot);
			if (iIndex >= 0 && iIndex < pLoopCity->getNumCityPlots())
			{
				iModifier += iCityModifier;
			}
		}
	}

	return iModifier;
}

// bUnsettleGreatPeople (all of them) or UnsettleSpecialists of the traits (added by this mod): the great people settled as
// this specialist can become units again. See CvCity::canUnsettleGreatPerson
bool CvPlayer::canUnsettleSpecialist(SpecialistTypes eSpecialist) const
{
	return (isUnsettleGreatPeople() || hasTraitFlag(&CvTraitInfo::isUnsettleSpecialist, (int)eSpecialist));
}

// bIgnoreEvents of the traits (added by this mod): see CvDLLButtonPopup::launchEventPopup and ignoreEvent
bool CvPlayer::isIgnoreEvents() const
{
	return hasTraitFlag(&CvTraitInfo::isIgnoreEvents);
}

// iEventChanceChange of the traits (added by this mod): see doEvents
int CvPlayer::getEventChanceChange() const
{
	return sumTraitValues(&CvTraitInfo::getEventChanceChange);
}

// bNoFirstEventDelay of the traits (added by this mod): see doEvents
bool CvPlayer::isNoFirstEventDelay() const
{
	return hasTraitFlag(&CvTraitInfo::isNoFirstEventDelay);
}

// Deals of the traits (added by this mod): the terms of the deals between the AI and the player. See CvPlayerAI::AI_cityTrade,
// AI_cityTradeVal, AI_maxGoldTrade, AI_maxGoldPerTurnTrade and AI_goldTradeValuePercent
bool CvPlayer::isBuyCities() const
{
	return hasTraitFlag(&CvTraitInfo::isBuyCities);
}

int CvPlayer::getDealCityValueModifier() const
{
	return sumTraitValues(&CvTraitInfo::getDealCityValueModifier);
}

int CvPlayer::getDealGoldLimitModifier() const
{
	return sumTraitValues(&CvTraitInfo::getDealGoldLimitModifier);
}

int CvPlayer::getDealGoldValueModifier() const
{
	return sumTraitValues(&CvTraitInfo::getDealGoldValueModifier);
}

// LendLease of the traits (added by this mod): the action button Lend-Lease sends the selected units to the capital of a
// civilization at peace. See CvUnit::canLendLease, launchLendLeasePopup and lendLease
bool CvPlayer::isLendLease() const
{
	return hasTraitFlag(&CvTraitInfo::isLendLease);
}

int CvPlayer::getLendLeaseResearchPercent() const
{
	return sumTraitValues(&CvTraitInfo::getLendLeaseResearchPercent);
}

int CvPlayer::getLendLeaseGoldPercent() const
{
	return sumTraitValues(&CvTraitInfo::getLendLeaseGoldPercent);
}

int CvPlayer::getLendLeaseAttitudeChange() const
{
	return sumTraitValues(&CvTraitInfo::getLendLeaseAttitudeChange);
}

// Not written (0) or above 4: 4, the most of the trade attitude of BtS (CvPlayerAI::AI_getTradeAttitude)
int CvPlayer::getLendLeaseAttitudeLimit() const
{
	int iLimit = sumTraitValues(&CvTraitInfo::getLendLeaseAttitudeLimit);

	if (iLimit <= 0 || iLimit > 4)
	{
		iLimit = 4;
	}

	return iLimit;
}

// The civilizations that can receive lend-lease from this player: alive, met, not at war, with a capital. Team mates and
// vassals too, the barbarians not
bool CvPlayer::canLendLeaseTo(PlayerTypes eTarget) const
{
	if (eTarget < 0 || eTarget >= MAX_CIV_PLAYERS || eTarget == getID())
	{
		return false;
	}

	CvPlayer& kTarget = GET_PLAYER(eTarget);

	if (!kTarget.isAlive() || kTarget.getCapitalCity() == NULL)
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasMet(kTarget.getTeam()) || atWar(getTeam(), kTarget.getTeam()))
	{
		return false;
	}

	return true;
}

// The units of apUnits (the selection or a group) and their cargo that eTarget can receive now (CvUnit::getLendLeaseDenial),
// each transport before its cargo. iNumUnits: how many units and cargo there are. szDenial: the reason of the first unit that
// cannot go (empty if all can)
void CvPlayer::getLendLeaseUnits(const std::vector<CvUnit*>& apUnits, PlayerTypes eTarget, std::vector<CvUnit*>& apSent, int& iNumUnits, CvWString& szDenial) const
{
	std::vector<CvUnit*> apAll;
	std::vector<CvUnit*> apCargo;
	const char* szKey;
	uint i;
	uint j;

	for (i = 0; i < apUnits.size(); i++)
	{
		if (std::find(apAll.begin(), apAll.end(), apUnits[i]) == apAll.end())
		{
			apAll.push_back(apUnits[i]);
		}

		apCargo.clear();
		apUnits[i]->getCargoUnits(apCargo);
		for (j = 0; j < apCargo.size(); j++)
		{
			if (std::find(apAll.begin(), apAll.end(), apCargo[j]) == apAll.end())
			{
				apAll.push_back(apCargo[j]);
			}
		}
	}

	iNumUnits = (int)apAll.size();
	apSent.clear();
	szDenial.clear();

	for (i = 0; i < apAll.size(); i++)
	{
		szKey = apAll[i]->getLendLeaseDenial(eTarget);
		if (szKey == NULL)
		{
			apSent.push_back(apAll[i]);
		}
		else if (szDenial.empty())
		{
			szDenial = gDLL->getText(szKey);
		}
	}
}

// The production cost of the units for this player, summed before any of them leaves (the cost of a unit can depend on how
// many units of its class the player has). The percents are taken of the sum, not of each unit
static int getLendLeaseCost(const CvPlayer& kPlayer, const std::vector<CvUnit*>& apUnits)
{
	int iCost = 0;
	uint i;

	for (i = 0; i < apUnits.size(); i++)
	{
		iCost += kPlayer.getProductionNeeded(apUnits[i]->getUnitType());
	}

	return iCost;
}

// The popup of the action button Lend-Lease (CvGame::handleAction, on the computer of the active player only): a button for
// each civilization that can receive some of the selected units (leader, civilization, its wars, its attitude, research and
// gold), and greyed lines with the reason for the others. CvDLLButtonPopup sends the choice as MOD_NET_MESSAGE_LEND_LEASE
void CvPlayer::launchLendLeasePopup()
{
	std::vector<CvUnit*> apSelected;
	std::vector<CvUnit*> apSent;
	CLLNode<IDInfo>* pSelectedUnitNode;
	CvUnit* pLoopUnit;
	CvPopupInfo* pInfo;
	CvWString szBody;
	CvWString szName;
	CvWString szButton;
	CvWString szWars;
	CvWString szDenial;
	CvWString szTemp;
	int iFlags = 0;
	int iNumUnits;
	int iCost;
	int iI;
	int iJ;

	pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();
	while (pSelectedUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pSelectedUnitNode->m_data);
		pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getID())
		{
			apSelected.push_back(pLoopUnit);
		}
	}

	if (apSelected.empty())
	{
		return;
	}

	pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON);
	pInfo->setOnClickedPythonCallback("claudeLendLease");
	szBody = gDLL->getText("TXT_KEY_POPUP_LEND_LEASE");

	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (!canLendLeaseTo((PlayerTypes)iI))
		{
			continue;
		}

		CvPlayerAI& kTarget = GET_PLAYER((PlayerTypes)iI);

		getLendLeaseUnits(apSelected, (PlayerTypes)iI, apSent, iNumUnits, szDenial);
		szName = gDLL->getText("TXT_KEY_LEND_LEASE_CIV", kTarget.getNameKey(), kTarget.getCivilizationShortDescriptionKey());

		if (apSent.empty())
		{
			szTemp.Format(SETCOLR L"%s: %s" ENDCOLR, TEXT_COLOR("COLOR_LIGHT_GREY"), szName.GetCString(), szDenial.GetCString());
			szBody += NEWLINE;
			szBody += szTemp;
			continue;
		}

		szButton = szName;

		// The civilizations at war with it that this player has met
		szWars.clear();
		for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			CvPlayer& kEnemy = GET_PLAYER((PlayerTypes)iJ);
			if (kEnemy.isAlive() && atWar(kTarget.getTeam(), kEnemy.getTeam()) && GET_TEAM(getTeam()).isHasMet(kEnemy.getTeam()))
			{
				if (!szWars.empty())
				{
					szWars += L", ";
				}
				szWars += kEnemy.getCivilizationShortDescription();
			}
		}
		szButton += L" / ";
		if (szWars.empty())
		{
			szButton += gDLL->getText("TXT_KEY_LEND_LEASE_NO_WARS");
		}
		else
		{
			szButton += gDLL->getText("TXT_KEY_LEND_LEASE_WARS", szWars.GetCString());
		}

		if (!kTarget.isHuman())
		{
			szButton += L" / ";
			szButton += gDLL->getText("TXT_KEY_LEND_LEASE_ATTITUDE", GC.getAttitudeInfo(kTarget.AI_getAttitude(getID())).getDescription());
		}

		iCost = getLendLeaseCost(*this, apSent);
		szButton += L" / ";
		szButton += gDLL->getText("TXT_KEY_LEND_LEASE_REWARD", (iCost * getLendLeaseResearchPercent()) / 100, (iCost * getLendLeaseGoldPercent()) / 100);

		if ((int)apSent.size() < iNumUnits)
		{
			szButton += L" ";
			szButton += gDLL->getText("TXT_KEY_LEND_LEASE_PART", (int)apSent.size(), iNumUnits);
		}

		pInfo->addPythonButton(szButton, GC.getLeaderHeadInfo(kTarget.getLeaderType()).getButton());
		iFlags |= (1 << iI);
	}

	if (iFlags == 0)
	{
		szBody += NEWLINE;
		szBody += gDLL->getText("TXT_KEY_LEND_LEASE_NO_TARGET");
	}

	pInfo->addPythonButton(gDLL->getText("TXT_KEY_POPUP_CANCEL"), "");
	pInfo->setFlags(iFlags);
	pInfo->setText(szBody);
	gDLL->getInterfaceIFace()->addPopup(pInfo, getID(), true);
}

// The choice of the popup of Lend-Lease, sent to every computer as MOD_NET_MESSAGE_LEND_LEASE: the group of the unit iUnitID
// and its cargo go to the capital of eTarget (the units that it can receive). This player gets research and gold of the
// production cost, and the trade attitude of eTarget goes up once for the whole batch
void CvPlayer::lendLease(PlayerTypes eTarget, int iUnitID)
{
	CvUnit* pHeadUnit = getUnit(iUnitID);
	CvUnit* pLoopUnit;
	CvUnit* pNewUnit;
	CvUnit* pFirstNewUnit = NULL;
	CLLNode<IDInfo>* pUnitNode;
	std::vector<CvUnit*> apGroup;
	std::vector<CvUnit*> apSent;
	std::vector<int> aiCosts;
	CvWString szDenial;
	CvWString szBuffer;
	int iNumUnits;
	int iCost = 0;
	int iCount = 0;
	int iResearch;
	int iGold;
	uint i;

	if (pHeadUnit == NULL || !isLendLease() || !canLendLeaseTo(eTarget) || !pHeadUnit->canLendLease(true))
	{
		return;
	}

	pUnitNode = pHeadUnit->getGroup()->headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pHeadUnit->getGroup()->nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			apGroup.push_back(pLoopUnit);
		}
	}

	getLendLeaseUnits(apGroup, eTarget, apSent, iNumUnits, szDenial);

	// The costs before any unit leaves
	for (i = 0; i < apSent.size(); i++)
	{
		aiCosts.push_back(getProductionNeeded(apSent[i]->getUnitType()));
	}

	for (i = 0; i < apSent.size(); i++)
	{
		// Checked again: the units sent before count for the limits of the unit classes of the receiver
		if (apSent[i]->getLendLeaseDenial(eTarget) != NULL)
		{
			continue;
		}

		pNewUnit = apSent[i]->lendLease(eTarget);
		if (pNewUnit != NULL)
		{
			iCost += aiCosts[i];
			iCount++;
			if (pFirstNewUnit == NULL)
			{
				pFirstNewUnit = pNewUnit;
			}
		}
	}

	if (iCount == 0)
	{
		return;
	}

	iResearch = (iCost * getLendLeaseResearchPercent()) / 100;
	if (iResearch > 0)
	{
		receiveResearch(iResearch);
	}

	iGold = (iCost * getLendLeaseGoldPercent()) / 100;
	if (iGold != 0)
	{
		changeGold(iGold);
	}

	changeLendLeaseAttitude(eTarget);

	CvCity* pCapital = GET_PLAYER(eTarget).getCapitalCity();

	szBuffer = gDLL->getText("TXT_KEY_LEND_LEASE_SENT", iCount, GET_PLAYER(eTarget).getNameKey(), iResearch, iGold);
	gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, pFirstNewUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));

	szBuffer = gDLL->getText("TXT_KEY_LEND_LEASE_RECEIVED", getNameKey(), iCount, pCapital->getNameKey());
	gDLL->getInterfaceIFace()->addMessage(eTarget, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, pFirstNewUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pCapital->getX_INLINE(), pCapital->getY_INLINE(), true, true);
}

// iAttitudeChange of LendLease: raises the trade attitude of eTarget towards this player (CvPlayerAI::AI_getTradeAttitude,
// "(grant value + trade value) / ((turns since met + 1) * 5)", 0 to 4) by adding just enough peacetime grant value, up to
// iAttitudeLimit. It fades as in BtS: the divisor grows each turn. As AI_changePeacetimeGrantValue does, the enemies of
// eTarget like this player less
void CvPlayer::changeLendLeaseAttitude(PlayerTypes eTarget)
{
	CvPlayerAI& kTarget = GET_PLAYER(eTarget);
	int iCurrent = kTarget.AI_getTradeAttitude(getID());
	int iGoal = std::min(iCurrent + getLendLeaseAttitudeChange(), getLendLeaseAttitudeLimit());
	int iDivisor;
	int iValue;

	if (iGoal <= iCurrent)
	{
		return;
	}

	iDivisor = (GET_TEAM(kTarget.getTeam()).AI_getHasMetCounter(getTeam()) + 1) * 5;
	iValue = kTarget.AI_getPeacetimeGrantValue(getID()) + std::max(0, kTarget.AI_getPeacetimeTradeValue(getID()) - GET_PLAYER(getID()).AI_getPeacetimeTradeValue(eTarget));

	if (iGoal * iDivisor > iValue)
	{
		kTarget.AI_changePeacetimeGrantValue(getID(), iGoal * iDivisor - iValue);
	}
}

// Research for the current tech, or the overflow when there is none (added by this mod)
void CvPlayer::receiveResearch(int iResearch)
{
	TechTypes eCurrentTech = getCurrentResearch();
	int iModifier;

	if (eCurrentTech != NO_TECH)
	{
		GET_TEAM(getTeam()).changeResearchProgress(eCurrentTech, iResearch, getID());
	}
	else
	{
		// The overflow gets the research modifier when it is used (doResearch), so it is stored without it
		iModifier = std::max(1, calculateResearchModifier(NO_TECH));
		changeOverflowResearch((iResearch * 100) / iModifier);
	}
}

// bIgnoreEvents of the traits (added by this mod): the player chose to ignore the triggered event. No choice is applied.
// The trigger counts as fired, as when a choice is applied (a trigger that is not recurring does not come again, a global
// or team trigger is fired for the others too), without the world news. Called on every computer (ModNetMessage)
void CvPlayer::ignoreEvent(int iEventTriggeredId)
{
	EventTriggeredData* pTriggeredData = getEventTriggered(iEventTriggeredId);
	if (NULL == pTriggeredData || pTriggeredData->m_eTrigger == NO_EVENTTRIGGER || !isIgnoreEvents())
	{
		return;
	}

	setTriggerFired(*pTriggeredData, true, false);
	deleteEventTriggered(iEventTriggeredId);
}

// bNoCultureFlip of the traits (added by this mod): the cities of the player do not flip by culture. Also true with
// bNoRevoltAndNoCultureFlip. See CvPlot::doCulture
bool CvPlayer::isNoCultureFlip() const
{
	return hasTraitFlag(&CvTraitInfo::isNoCultureFlip) || isNoRevoltAndNoCultureFlip();
}

// bNoRevoltAndNoCultureFlip of the traits (added by this mod): see CvCity::getRevoltTestProbability
bool CvPlayer::isNoRevoltAndNoCultureFlip() const
{
	return hasTraitFlag(&CvTraitInfo::isNoRevoltAndNoCultureFlip);
}

// bInstantCultureFlip of the traits (added by this mod): see CvPlot::doCulture
bool CvPlayer::isInstantCultureFlip() const
{
	return hasTraitFlag(&CvTraitInfo::isInstantCultureFlip);
}

// iCultureRevoltChance of the traits (added by this mod): see CvCity::getRevoltTestProbability
int CvPlayer::getCultureRevoltChance() const
{
	return sumTraitValues(&CvTraitInfo::getCultureRevoltChance);
}

// iCultureRangeChange of the traits (added by this mod)
int CvPlayer::getCultureRangeChange() const
{
	return sumTraitValues(&CvTraitInfo::getCultureRangeChange);
}

// The anger of the traits, in 1/10 % of the population (added by this mod)
int CvPlayer::getPeaceAngerPercentAnger() const
{
	return m_iPeaceAngerTimes100 / 100;
}

// PeaceAnger/iPerTurn / PeaceAnger/iDecayPerTurn / PeaceAnger/iMax of the traits (added by this mod): the anger grows in the
// turns without a war against a full civilization and falls during such a war. The rates follow the game speed like the
// other things counted in turns, so the anger is kept x100 to keep small rates from rounding to 0 at slow speeds
void CvPlayer::doPeaceAnger()
{
	int iOldAnger = getPeaceAngerPercentAnger();
	int iPerTurn = sumTraitValues(&CvTraitInfo::getPeaceAngerPerTurn);
	int iDecay = sumTraitValues(&CvTraitInfo::getPeaceAngerDecayPerTurn);
	int iMax = sumTraitValues(&CvTraitInfo::getPeaceAngerMax);
	int iGrowthPercent;

	iGrowthPercent = std::max(1, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent());

	if (GET_TEAM(getTeam()).getAtWarCount(true) > 0)
	{
		m_iPeaceAngerTimes100 -= (iDecay * 10000) / iGrowthPercent;
	}
	else
	{
		m_iPeaceAngerTimes100 += (iPerTurn * 10000) / iGrowthPercent;
	}

	// Also drops the anger of a trait the player has lost
	m_iPeaceAngerTimes100 = range(m_iPeaceAngerTimes100, 0, iMax * 100);

	if (getPeaceAngerPercentAnger() != iOldAnger)
	{
		AI_makeAssignWorkDirty();
	}
}

// ForbiddenCivics of the traits (added by this mod): canDoCivics is false, and verifyCivics changes the civic at the start
// of the turn when the player gains such a trait
bool CvPlayer::isForbiddenCivic(CivicTypes eCivic) const
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI) && GC.getTraitInfo((TraitTypes)iI).isForbiddenCivic(eCivic))
		{
			return true;
		}
	}

	return false;
}

int CvPlayer::getMaxCities() const
{
	int iMaxCities = -1;
	int iLimit;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iLimit = GC.getTraitInfo((TraitTypes)iI).getMaxCitiesLimit(GC.getMapINLINE().getWorldSize());

			if (iLimit >= 0 && (iMaxCities < 0 || iLimit < iMaxCities))
			{
				iMaxCities = iLimit;
			}
		}
	}

	return iMaxCities;
}

// The player cannot found more cities (nor train settlers, nor get them from goody huts). Conquered, traded and
// flipped cities can go over the limit.
bool CvPlayer::isMaxCitiesReached() const
{
	int iMaxCities = getMaxCities();

	return (iMaxCities >= 0 && getNumCities() >= iMaxCities);
}

// TerritoryRoutes of the traits (added by this mod): the best route (iValue) for pPlot, NO_ROUTE if none. A peak counts as
// TERRAIN_PEAK (not its terrain), hills as their terrain and TERRAIN_HILL
RouteTypes CvPlayer::getTerritoryRouteType(const CvPlot* pPlot) const
{
	RouteTypes eBestRoute = NO_ROUTE;
	RouteTypes eRoute;
	TerrainTypes aeTerrains[2];
	int iNumTerrains;
	int iI;
	int iJ;

	if (pPlot == NULL || pPlot->isWater())
	{
		return NO_ROUTE;
	}

	iNumTerrains = 0;
	if (pPlot->isPeak())
	{
		aeTerrains[iNumTerrains++] = (TerrainTypes)GC.getInfoTypeForString("TERRAIN_PEAK", true);
	}
	else
	{
		aeTerrains[iNumTerrains++] = pPlot->getTerrainType();
		if (pPlot->isHills())
		{
			aeTerrains[iNumTerrains++] = (TerrainTypes)GC.getInfoTypeForString("TERRAIN_HILL", true);
		}
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (!hasTrait((TraitTypes)iI))
		{
			continue;
		}

		for (iJ = 0; iJ < iNumTerrains; iJ++)
		{
			eRoute = (RouteTypes)GC.getTraitInfo((TraitTypes)iI).getTerritoryRouteType(aeTerrains[iJ]);
			if (eRoute != NO_ROUTE && (eBestRoute == NO_ROUTE || GC.getRouteInfo(eRoute).getValue() > GC.getRouteInfo(eBestRoute).getValue()))
			{
				eBestRoute = eRoute;
			}
		}
	}

	return eBestRoute;
}

RouteTypes CvPlayer::getFeatureRouteType(FeatureTypes eFeature) const
{
	RouteTypes eBestRoute = NO_ROUTE;
	RouteTypes eRoute;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			eRoute = (RouteTypes)GC.getTraitInfo((TraitTypes)iI).getFeatureRouteType(eFeature);

			if (eRoute != NO_ROUTE && (eBestRoute == NO_ROUTE || GC.getRouteInfo(eRoute).getValue() > GC.getRouteInfo(eBestRoute).getValue()))
			{
				eBestRoute = eRoute;
			}
		}
	}

	return eBestRoute;
}

int CvPlayer::getGoldenAgeGreatPeopleRateModifier() const
{
	return sumTraitValues(&CvTraitInfo::getGoldenAgeGreatPeopleRateModifier);
}

int CvPlayer::getTraitBonusYieldChange(BonusTypes eBonus, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getBonusYieldChange, eBonus, eYield);
}

int CvPlayer::getCapitalBuildingProductionModifier() const
{
	return sumTraitValues(&CvTraitInfo::getCapitalBuildingProductionModifier);
}

int CvPlayer::getPillageGoldModifier() const
{
	return sumTraitValues(&CvTraitInfo::getPillageGoldModifier);
}

int CvPlayer::getCityCaptureGoldModifier() const
{
	return sumTraitValues(&CvTraitInfo::getCityCaptureGoldModifier);
}

// Production cost % of all units, the sum of the traits (iUnitCostModifier; added by this mod)
int CvPlayer::getUnitCostModifier() const
{
	return sumTraitValues(&CvTraitInfo::getUnitCostModifier);
}

// Production cost % of all buildings, the sum of the traits (iBuildingCostModifier; added by this mod)
int CvPlayer::getBuildingCostModifier() const
{
	return sumTraitValues(&CvTraitInfo::getBuildingCostModifier);
}

// The highest NewCityCultureLevel of the traits and of the buildings in any city of this player (not obsolete)
CultureLevelTypes CvPlayer::getNewCityCultureLevel() const
{
	int iLevel = NO_CULTURELEVEL;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iLevel = std::max(iLevel, GC.getTraitInfo((TraitTypes)iI).getNewCityCultureLevel());
		}
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getNewCityCultureLevel() > iLevel)
		{
			if (hasActiveBuilding((BuildingTypes)iI))
			{
				iLevel = GC.getBuildingInfo((BuildingTypes)iI).getNewCityCultureLevel();
			}
		}
	}

	return (CultureLevelTypes)iLevel;
}

bool CvPlayer::hasActiveBuilding(BuildingTypes eBuilding) const
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->getNumActiveBuilding(eBuilding) > 0)
		{
			return true;
		}
	}

	return false;
}

// Extra population of the cities this player founds (CvPlayer::found)
int CvPlayer::getNewCityPopulation() const
{
	int iPopulation = sumTraitValues(&CvTraitInfo::getNewCityPopulation);
	int iI;

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getNewCityPopulation() != 0)
		{
			if (hasActiveBuilding((BuildingTypes)iI))
			{
				iPopulation += GC.getBuildingInfo((BuildingTypes)iI).getNewCityPopulation();
			}
		}
	}

	return iPopulation;
}

// The state religion spreads to the cities this player founds (CvPlayer::found)
bool CvPlayer::isNewCityStateReligion() const
{
	int iI;

	if (hasTraitFlag(&CvTraitInfo::isNewCityStateReligion))
	{
		return true;
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).isNewCityStateReligion())
		{
			if (hasActiveBuilding((BuildingTypes)iI))
			{
				return true;
			}
		}
	}

	return false;
}

// Revolt time % of the cities this player conquers (CvPlayer::acquireCity)
int CvPlayer::getOccupationTimeModifier() const
{
	int iModifier = sumTraitValues(&CvTraitInfo::getOccupationTimeModifier);
	int iI;

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getOccupationTimeModifier() != 0)
		{
			if (hasActiveBuilding((BuildingTypes)iI))
			{
				iModifier += GC.getBuildingInfo((BuildingTypes)iI).getOccupationTimeModifier();
			}
		}
	}

	return iModifier;
}

// iConquestCulturePercent of the traits (added by this mod): the share of the culture of the others the player takes over in a
// conquered city, 0 to 100
int CvPlayer::getConquestCulturePercent() const
{
	return range(sumTraitValues(&CvTraitInfo::getConquestCulturePercent), 0, 100);
}

// iConquestBuildingSurviveModifier of the traits (added by this mod): % to the chance the buildings survive a conquest
int CvPlayer::getConquestBuildingSurviveModifier() const
{
	return sumTraitValues(&CvTraitInfo::getConquestBuildingSurviveModifier);
}

// bNoConquestPopulationLoss of the traits (added by this mod): a conquered city keeps its population
bool CvPlayer::isNoConquestPopulationLoss() const
{
	return hasTraitFlag(&CvTraitInfo::isNoConquestPopulationLoss);
}

// bNoAutoRaze of the traits (added by this mod): a conquered city that never grew beyond 1 is not razed automatically
bool CvPlayer::isNoAutoRaze() const
{
	return hasTraitFlag(&CvTraitInfo::isNoAutoRaze);
}

// iNewWorldCityPopulation of the traits (added by this mod): population of the cities the player founds in the New World
int CvPlayer::getNewWorldCityPopulation() const
{
	return sumTraitValues(&CvTraitInfo::getNewWorldCityPopulation);
}

// iOverseasCityPopulation of the traits (added by this mod): population of the cities the player founds overseas
int CvPlayer::getOverseasCityPopulation() const
{
	return sumTraitValues(&CvTraitInfo::getOverseasCityPopulation);
}

// iOverseasMaintenanceModifier of the traits (added by this mod): maintenance % of the overseas cities
int CvPlayer::getOverseasMaintenanceModifier() const
{
	return sumTraitValues(&CvTraitInfo::getOverseasMaintenanceModifier);
}

// iDistantTradeRouteModifier of the traits (added by this mod): trade route yield % with cities far from the capital
int CvPlayer::getDistantTradeRouteModifier() const
{
	return sumTraitValues(&CvTraitInfo::getDistantTradeRouteModifier);
}

// EnemyAttrition/iPercent of the traits (added by this mod): see doEnemyAttrition
int CvPlayer::getEnemyAttritionInBorders() const
{
	return sumTraitValues(&CvTraitInfo::getEnemyAttritionInBorders);
}

// iAnarchyGoldenAgePercent of the traits (added by this mod): see changeAnarchyTurns
int CvPlayer::getAnarchyGoldenAgePercent() const
{
	return sumTraitValues(&CvTraitInfo::getAnarchyGoldenAgePercent);
}

// iTechCostModifier of the traits (added by this mod): see CvTeam::getResearchCost
int CvPlayer::getTechCostModifier() const
{
	return sumTraitValues(&CvTraitInfo::getTechCostModifier);
}

// iAttitudeChange of the traits (added by this mod): see CvPlayerAI::AI_getAttitudeVal
int CvPlayer::getAttitudeChange() const
{
	return sumTraitValues(&CvTraitInfo::getAttitudeChange);
}

// iDenounceAttitudeChange of the traits (added by this mod): see CvPlayerAI::AI_getDenounceAttitude
int CvPlayer::getDenounceAttitudeChange() const
{
	return sumTraitValues(&CvTraitInfo::getDenounceAttitudeChange);
}

// iRouteMovementModifierInBorders of the traits (added by this mod): see CvPlot::movementCost
int CvPlayer::getRouteMovementModifierInBorders() const
{
	return sumTraitValues(&CvTraitInfo::getRouteMovementModifierInBorders);
}

// iCombatPercent of the traits (added by this mod): combat % of all the units of the player (CvUnit::getExtraCombatPercent)
int CvPlayer::getTraitCombatPercent() const
{
	return sumTraitValues(&CvTraitInfo::getCombatPercent);
}

// AttitudeCombatPercents of the traits (added by this mod): counts the AI players (alive, met, not barbarian, not on the team of
// this player) by their attitude toward this player (AI_getAttitude, as the diplomacy screen shows it). Called at the start of
// the turn of this player and after a war is declared or peace is made (CvTeam::declareWar, CvTeam::makePeace). The counts are
// saved, so that the combat % does not change by saving and loading in the middle of a turn
void CvPlayer::updateAttitudeCombatCounts()
{
	AttitudeTypes eAttitude;
	int iI;

	for (iI = 0; iI < NUM_ATTITUDE_TYPES; iI++)
	{
		m_aiAttitudeCombatCount[iI] = 0;
	}

	if (!isAlive() || getLeaderType() == NO_LEADER || !hasTraitFlag(&CvTraitInfo::isAttitudeCombat))
	{
		return;
	}

	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kLoopPlayer.isAlive() || kLoopPlayer.isHuman() || kLoopPlayer.isBarbarian() || kLoopPlayer.getLeaderType() == NO_LEADER)
		{
			continue;
		}

		if (kLoopPlayer.getTeam() == getTeam() || !GET_TEAM(getTeam()).isHasMet(kLoopPlayer.getTeam()))
		{
			continue;
		}

		eAttitude = kLoopPlayer.AI_getAttitude(getID());
		if (eAttitude >= 0 && eAttitude < NUM_ATTITUDE_TYPES)
		{
			m_aiAttitudeCombatCount[eAttitude]++;
		}
	}
}

int CvPlayer::getAttitudeCombatCount(AttitudeTypes eAttitude) const
{
	return (eAttitude >= 0 && eAttitude < NUM_ATTITUDE_TYPES) ? m_aiAttitudeCombatCount[eAttitude] : 0;
}

// AttitudeCombatPercents of the traits (added by this mod): the % of eAttitude of all the traits times the AI players with the
// attitude, before the caps (for the help texts)
int CvPlayer::getAttitudeCombatPercentOf(AttitudeTypes eAttitude) const
{
	if (getAttitudeCombatCount(eAttitude) == 0)
	{
		return 0;
	}

	return getAttitudeCombatCount(eAttitude) * sumTraitValues(&CvTraitInfo::getAttitudeCombatPercent, (int)eAttitude);
}

// AttitudeCombatPercents of the traits (added by this mod): combat % of all the units of the player
// (CvUnit::getExtraCombatPercent). Each trait is capped by its iMaxPercent before the traits are added up
int CvPlayer::getAttitudeCombatPercent() const
{
	int iTotal = 0;
	int iTraitPercent;
	int iI;
	int iJ;

	for (iI = 0; iI < NUM_ATTITUDE_TYPES; iI++)
	{
		if (m_aiAttitudeCombatCount[iI] != 0)
		{
			break;
		}
	}
	if (iI == NUM_ATTITUDE_TYPES)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (!kTrait.isAttitudeCombat() || !hasTrait((TraitTypes)iI))
		{
			continue;
		}

		iTraitPercent = 0;
		for (iJ = 0; iJ < NUM_ATTITUDE_TYPES; iJ++)
		{
			iTraitPercent += m_aiAttitudeCombatCount[iJ] * kTrait.getAttitudeCombatPercent(iJ);
		}
		if (kTrait.getAttitudeCombatMaxPercent() >= 0)
		{
			iTraitPercent = std::min(iTraitPercent, kTrait.getAttitudeCombatMaxPercent());
		}
		iTotal += iTraitPercent;
	}

	return iTotal;
}

// iHappinessPerReligion, CommercePerReligion and iDefenseCombatPercent of the traits (added by this mod)
int CvPlayer::getHappinessPerReligion() const
{
	return sumTraitValues(&CvTraitInfo::getHappinessPerReligion);
}

int CvPlayer::getCommercePerReligion(CommerceTypes eCommerce) const
{
	return sumTraitValues(&CvTraitInfo::getCommercePerReligion, eCommerce);
}

// CommercePercentPerReligion, iExperiencePerReligion, iGreatPeopleRatePerReligion, bOtherReligionsToCapital and
// bNoForeignCultureAnger of the traits (added by this mod)
int CvPlayer::getCommercePercentPerReligion(CommerceTypes eCommerce) const
{
	return sumTraitValues(&CvTraitInfo::getCommercePercentPerReligion, eCommerce);
}

int CvPlayer::getExperiencePerReligion() const
{
	return sumTraitValues(&CvTraitInfo::getExperiencePerReligion);
}

int CvPlayer::getGreatPeopleRatePerReligion() const
{
	return sumTraitValues(&CvTraitInfo::getGreatPeopleRatePerReligion);
}

bool CvPlayer::isOtherReligionsToCapital() const
{
	return hasTraitFlag(&CvTraitInfo::isOtherReligionsToCapital);
}

bool CvPlayer::isNoForeignCultureAnger() const
{
	return hasTraitFlag(&CvTraitInfo::isNoForeignCultureAnger);
}

// iWonderGreatPeopleTurns and iWonderProductionCulturePercent of the traits (added by this mod)
int CvPlayer::getWonderGreatPeopleTurns() const
{
	return sumTraitValues(&CvTraitInfo::getWonderGreatPeopleTurns);
}

int CvPlayer::getWonderProductionCulturePercent() const
{
	return sumTraitValues(&CvTraitInfo::getWonderProductionCulturePercent);
}

// FeatureConversions of the traits (added by this mod): the chance (1/10000) that a plot of eFrom in our territory becomes
// eTo each turn, added up over the traits
int CvPlayer::getFeatureConversionChance(FeatureTypes eFrom, FeatureTypes eTo) const
{
	int iChance = 0;
	int iI, iJ;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.getNumFeatureConversions() == 0 || !hasTrait((TraitTypes)iI))
		{
			continue;
		}

		for (iJ = 0; iJ < kTrait.getNumFeatureConversions(); iJ++)
		{
			if (kTrait.getFeatureConversionFrom(iJ) == eFrom && kTrait.getFeatureConversionTo(iJ) == eTo)
			{
				iChance += kTrait.getFeatureConversionChance(iJ);
			}
		}
	}

	return iChance;
}

int CvPlayer::getTraitDefenseCombatPercent() const
{
	return sumTraitValues(&CvTraitInfo::getDefenseCombatPercent);
}

// WorldWonderCommerceChanges and GlobalWorldWonderCommerceChanges of the traits (added by this mod): CvCity::getTraitWorldWonderCommerce
int CvPlayer::getTraitWorldWonderCommerce(CommerceTypes eCommerce) const
{
	return sumTraitValues(&CvTraitInfo::getWorldWonderCommerceChange, eCommerce);
}

int CvPlayer::getTraitGlobalWorldWonderCommerce(CommerceTypes eCommerce) const
{
	return sumTraitValues(&CvTraitInfo::getGlobalWorldWonderCommerceChange, eCommerce);
}

// NearbyCityCommerce of the traits (added by this mod): commerce of pCity for the other cities of the player within the
// range of each trait (CvCity::getNearbyCityCommerce). Each trait counts with its own range, and doubles its own part in
// the capital with bDoubleInCapital. Counted whenever the commerce is calculated, so nothing is saved; the cities are
// updated when one is added or removed (updateNearbyCityCommerce), and setCapitalCity updates the old and the new capital
int CvPlayer::getNearbyCityCommerce(const CvCity* pCity, CommerceTypes eCommerce) const
{
	int iCommerce = 0;
	int iI;

	if (pCity == NULL || sumTraitValues(&CvTraitInfo::getNearbyCityRange) <= 0)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (!hasTrait((TraitTypes)iI))
		{
			continue;
		}

		CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		int iChange = kTrait.getNearbyCityCommerce(eCommerce);

		if (iChange == 0 || kTrait.getNearbyCityRange() <= 0)
		{
			continue;
		}

		int iValue = iChange * countNearbyCities(pCity, kTrait.getNearbyCityRange());

		if (kTrait.isNearbyCityDoubleInCapital() && pCity->isCapital())
		{
			iValue *= 2;
		}

		iCommerce += iValue;
	}

	return iCommerce;
}

// The other cities of the player within iRange tiles (plotDistance) of pCity, in any area
int CvPlayer::countNearbyCities(const CvCity* pCity, int iRange) const
{
	CvCity* pLoopCity;
	int iCount = 0;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity != pCity && plotDistance(pCity->getX_INLINE(), pCity->getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE()) <= iRange)
		{
			iCount++;
		}
	}

	return iCount;
}

// Called by CvCity::init and CvCity::kill: the number of nearby cities may have changed for every city of the player
void CvPlayer::updateNearbyCityCommerce()
{
	CvCity* pLoopCity;
	int iLoop;

	if (sumTraitValues(&CvTraitInfo::getNearbyCityRange) <= 0)
	{
		return;
	}

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateCommerce();
	}
}

// The start bias of the player for a feature or a terrain (added by this mod): StartBiasFeatures / StartBiasTerrains of the
// civilization and of the traits of the player. The normalization of the starting plots keeps what the player likes
// (CvGame::normalizeRemovePeaks, normalizeRemoveBadFeatures, normalizeRemoveBadTerrain)
int CvPlayer::getStartBiasFeature(FeatureTypes eFeature) const
{
	if (eFeature == NO_FEATURE || getLeaderType() == NO_LEADER)
	{
		return 0;
	}

	int iBias = ((getCivilizationType() != NO_CIVILIZATION) ? GC.getCivilizationInfo(getCivilizationType()).getStartBiasFeature(eFeature) : 0);
	return iBias + sumTraitValues(&CvTraitInfo::getStartBiasFeature, eFeature);
}

int CvPlayer::getStartBiasTerrain(TerrainTypes eTerrain) const
{
	if (eTerrain == NO_TERRAIN || getLeaderType() == NO_LEADER)
	{
		return 0;
	}

	int iBias = ((getCivilizationType() != NO_CIVILIZATION) ? GC.getCivilizationInfo(getCivilizationType()).getStartBiasTerrain(eTerrain) : 0);
	return iBias + sumTraitValues(&CvTraitInfo::getStartBiasTerrain, eTerrain);
}

// The max number of national wonders in a city of the player, -1 for no limit (added by this mod; was in
// CvCity::isNationalWondersMaxed). MAX_NATIONAL_WONDERS_PER_CITY of GlobalDefines, or MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC for a
// human player in a One-City Challenge game. iMaxNationalWondersPerCity (ForOCC) of the traits replaces the define; with several
// traits, the loosest wins (-1 over any number). 0 in a trait: not set
int CvPlayer::getMaxNationalWondersPerCity() const
{
	bool bOCC = (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman());
	int iMax = 0;
	int iValue;
	int iI;

	if (getLeaderType() != NO_LEADER)
	{
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (hasTrait((TraitTypes)iI))
			{
				CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
				iValue = (bOCC ? kTrait.getMaxNationalWondersPerCityForOCC() : kTrait.getMaxNationalWondersPerCity());

				if (iValue == -1)
				{
					return -1;
				}

				iMax = std::max(iMax, iValue);
			}
		}
	}

	if (iMax > 0)
	{
		return iMax;
	}

	return (bOCC ? GC.getDefineINT("MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC") : GC.getDefineINT("MAX_NATIONAL_WONDERS_PER_CITY"));
}

// bHumanCanBeVassal of the traits (added by this mod): see canTradeItem
bool CvPlayer::isHumanCanBeVassal() const
{
	return hasTraitFlag(&CvTraitInfo::isHumanCanBeVassal);
}

// iMaxExperienceAfterUpgrade of the traits (added by this mod): the experience a unit keeps when upgraded (CvUnit::upgrade),
// MAX_EXPERIENCE_AFTER_UPGRADE replaced by the traits. -1 (keeps all) wins, then the largest value; 0 = not set
int CvPlayer::getMaxExperienceAfterUpgrade() const
{
	int iMax = 0;
	int iValue;
	int iI;

	if (getLeaderType() != NO_LEADER)
	{
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (hasTrait((TraitTypes)iI))
			{
				iValue = GC.getTraitInfo((TraitTypes)iI).getMaxExperienceAfterUpgrade();

				if (iValue < 0)
				{
					return -1;
				}

				iMax = std::max(iMax, iValue);
			}
		}
	}

	if (iMax > 0)
	{
		return iMax;
	}

	return GC.getDefineINT("MAX_EXPERIENCE_AFTER_UPGRADE");
}

// iExpOutsideBordersChange of the traits (added by this mod): see CvUnit::changeExperience
int CvPlayer::getExpOutsideBordersChange() const
{
	return sumTraitValues(&CvTraitInfo::getExpOutsideBordersChange);
}

// iWeLoveTheKingChance of the traits (added by this mod): see CvCity::doTurn
int CvPlayer::getWeLoveTheKingChance() const
{
	return range(sumTraitValues(&CvTraitInfo::getWeLoveTheKingChance), 0, 100);
}

// SpecialistSlotChanges of the traits (added by this mod): -1 if a trait makes the specialist unlimited, else the slots the
// traits add in every city (CvCity::getMaxSpecialistCount, isSpecialistValid)
int CvPlayer::getTraitSpecialistSlotChange(SpecialistTypes eSpecialist) const
{
	int iSlots = 0;
	int iValue;
	int iI;

	if (getLeaderType() != NO_LEADER)
	{
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (hasTrait((TraitTypes)iI))
			{
				iValue = GC.getTraitInfo((TraitTypes)iI).getSpecialistSlotChange(eSpecialist);

				if (iValue < 0)
				{
					return -1;
				}

				iSlots += iValue;
			}
		}
	}

	return iSlots;
}

// ImprovedBonusYieldChanges of the traits (added by this mod): see CvPlot::calculateImprovementYieldChange
int CvPlayer::getTraitImprovedBonusYieldChange(BonusTypes eBonus, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getImprovedBonusYieldChange, eBonus, eYield);
}

// RevealBonuses of the traits (added by this mod): see CvTeam::isTraitRevealedBonus
bool CvPlayer::isTraitRevealBonus(BonusTypes eBonus) const
{
	return hasTraitFlag(&CvTraitInfo::isRevealBonus, eBonus);
}

// LockedImprovements of the traits (added by this mod): see CvPlot::canBuild and CvUnit::canPillage
bool CvPlayer::isLockedImprovement(ImprovementTypes eImprovement) const
{
	return hasTraitFlag(&CvTraitInfo::isLockedImprovement, eImprovement);
}

// PyOnImprovementChanged of the traits (added by this mod): pPlot is a plot of this player, eOldImprovement and
// eNewImprovement may be NO_IMPROVEMENT. The plot goes to Python as its coordinates
void CvPlayer::doImprovementChangedPythonHook(const CvPlot* pPlot, ImprovementTypes eOldImprovement, ImprovementTypes eNewImprovement)
{
	std::vector<int> aiArgs;

	aiArgs.push_back(pPlot->getX_INLINE());
	aiArgs.push_back(pPlot->getY_INLINE());
	aiArgs.push_back((int)eOldImprovement);
	aiArgs.push_back((int)eNewImprovement);
	callTraitPythonHook(TRAIT_PYHOOK_IMPROVEMENT_CHANGED, NULL, NULL, aiArgs);
}

// NoTechProcesses of the traits (added by this mod): the cities can maintain the process without its tech
bool CvPlayer::isNoTechProcess(ProcessTypes eProcess) const
{
	return hasTraitFlag(&CvTraitInfo::isNoTechProcess, eProcess);
}

// bNoConscriptExperiencePenalty of the traits (added by this mod): see CvCity::addProductionExperience
bool CvPlayer::isNoConscriptExperiencePenalty() const
{
	return hasTraitFlag(&CvTraitInfo::isNoConscriptExperiencePenalty);
}

// FreeTechs of the traits (added by this mod): the techs the traits give at the start of the game. CvGame::setInitialItems
// calls this after ConditionalFreeTraits, so the traits given at the start also count. Like the free techs of the
// civilizations (CvGame::initFreeState), the prerequisite techs are not needed
void CvPlayer::initTraitFreeTechs()
{
	int iI;

	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)iI) && hasTraitFlag(&CvTraitInfo::isFreeTech, iI))
		{
			GET_TEAM(getTeam()).setHasTech((TechTypes)iI, true, NO_PLAYER, false, false);
			if (GC.getTechInfo((TechTypes)iI).isMapVisible())
			{
				GC.getMapINLINE().setRevealedPlots(getTeam(), true, true);
			}
		}
	}
}

// YieldChanges of the traits (added by this mod): yield of every city (CvCity::getTraitYield)
int CvPlayer::getTraitYieldChange(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getYieldChange, eYield);
}

// iHealthPerReligion and YieldPerReligion of the traits (added by this mod): for each religion in the city
int CvPlayer::getHealthPerReligion() const
{
	return sumTraitValues(&CvTraitInfo::getHealthPerReligion);
}

int CvPlayer::getYieldPerReligion(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getYieldPerReligion, eYield);
}

// WorldWonderYieldChanges and GlobalWorldWonderYieldChanges of the traits (added by this mod): CvCity::getTraitYield
int CvPlayer::getTraitWorldWonderYield(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getWorldWonderYieldChange, eYield);
}

int CvPlayer::getTraitGlobalWorldWonderYield(YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getGlobalWorldWonderYieldChange, eYield);
}

// BonusHappinessChanges and BonusHealthChanges of the traits (added by this mod): CvCity::getTraitBonusHappiness
int CvPlayer::getTraitBonusHappiness(BonusTypes eBonus) const
{
	return sumTraitValues(&CvTraitInfo::getBonusHappinessChange, eBonus);
}

int CvPlayer::getTraitBonusHealth(BonusTypes eBonus) const
{
	return sumTraitValues(&CvTraitInfo::getBonusHealthChange, eBonus);
}

bool CvPlayer::isAnyTraitBonusHappinessOrHealth() const
{
	return hasTraitFlag(&CvTraitInfo::isAnyBonusHappinessOrHealthChange);
}

// iTradeRouteModifier, iForeignTradeRouteModifier, iBombardDefense and iEspionageDefense of the traits (added by this mod):
// as the buildings, for every city (iCoastalTradeRoutes and iAllCityDefense: processTrait)
int CvPlayer::getTraitTradeRouteModifier() const
{
	return sumTraitValues(&CvTraitInfo::getTradeRouteModifier);
}

int CvPlayer::getTraitForeignTradeRouteModifier() const
{
	return sumTraitValues(&CvTraitInfo::getForeignTradeRouteModifier);
}

int CvPlayer::getTraitBombardDefense() const
{
	return sumTraitValues(&CvTraitInfo::getBombardDefense);
}

int CvPlayer::getTraitEspionageDefense() const
{
	return sumTraitValues(&CvTraitInfo::getEspionageDefense);
}

// iFoodKept of the traits (added by this mod): see CvCity::getMaxFoodKeptPercent
int CvPlayer::getTraitFoodKept() const
{
	return sumTraitValues(&CvTraitInfo::getFoodKept);
}

// iMaintenanceModifier of the traits (added by this mod): see CvCity::getMaintenanceModifier
int CvPlayer::getTraitMaintenanceModifier() const
{
	return sumTraitValues(&CvTraitInfo::getMaintenanceModifier);
}

// CapitalTerrainYieldPercents of the traits (added by this mod): see CvCity::getTraitCapitalTerrainYield
int CvPlayer::getTraitCapitalTerrainYieldPercent(TerrainTypes eTerrain, YieldTypes eYield) const
{
	return sumTraitValues(&CvTraitInfo::getCapitalTerrainYieldPercent, eTerrain, eYield);
}

bool CvPlayer::isAnyTraitCapitalTerrainYieldPercent() const
{
	return hasTraitFlag(&CvTraitInfo::isAnyCapitalTerrainYieldPercent);
}

// Hurrys of the traits (added by this mod): an item of a trait of the player for eHurry that applies to the unit or building
// lets the player hurry it this way without the civic (CvCity::canHurry...)
bool CvPlayer::isTraitHurry(HurryTypes eHurry, UnitTypes eUnit, BuildingTypes eBuilding) const
{
	int iI, iJ;

	if (eHurry == NO_HURRY || getLeaderType() == NO_LEADER)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.getNumHurrys() > 0 && hasTrait((TraitTypes)iI))
		{
			for (iJ = 0; iJ < kTrait.getNumHurrys(); iJ++)
			{
				if (kTrait.getHurry(iJ).iHurry == eHurry && kTrait.getHurry(iJ).isTarget(eUnit, eBuilding))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// The sum of iCostModifier (bIgnoreCostModifier: any of them, iExperience: the sum) of the items for eHurry that apply to
// the unit or building. They count whether the trait or a civic lets the player hurry it (added by this mod)
int CvPlayer::getTraitHurryCostModifier(HurryTypes eHurry, UnitTypes eUnit, BuildingTypes eBuilding) const
{
	int iModifier = 0;
	int iI, iJ;

	if (eHurry == NO_HURRY || getLeaderType() == NO_LEADER)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.getNumHurrys() > 0 && hasTrait((TraitTypes)iI))
		{
			for (iJ = 0; iJ < kTrait.getNumHurrys(); iJ++)
			{
				if (kTrait.getHurry(iJ).iHurry == eHurry && kTrait.getHurry(iJ).isTarget(eUnit, eBuilding))
				{
					iModifier += kTrait.getHurry(iJ).iCostModifier;
				}
			}
		}
	}

	return iModifier;
}

bool CvPlayer::isTraitHurryIgnoreCostModifier(HurryTypes eHurry, UnitTypes eUnit, BuildingTypes eBuilding) const
{
	int iI, iJ;

	if (eHurry == NO_HURRY || getLeaderType() == NO_LEADER)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.getNumHurrys() > 0 && hasTrait((TraitTypes)iI))
		{
			for (iJ = 0; iJ < kTrait.getNumHurrys(); iJ++)
			{
				if (kTrait.getHurry(iJ).iHurry == eHurry && kTrait.getHurry(iJ).bIgnoreCostModifier && kTrait.getHurry(iJ).isTarget(eUnit, eBuilding))
				{
					return true;
				}
			}
		}
	}

	return false;
}

int CvPlayer::getTraitHurryExperience(HurryTypes eHurry, UnitTypes eUnit) const
{
	int iExperience = 0;
	int iI, iJ;

	if (eHurry == NO_HURRY || eUnit == NO_UNIT || getLeaderType() == NO_LEADER)
	{
		return 0;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
		if (kTrait.getNumHurrys() > 0 && hasTrait((TraitTypes)iI))
		{
			for (iJ = 0; iJ < kTrait.getNumHurrys(); iJ++)
			{
				if (kTrait.getHurry(iJ).iHurry == eHurry && kTrait.getHurry(iJ).isTarget(eUnit, NO_BUILDING))
				{
					iExperience += kTrait.getHurry(iJ).iExperience;
				}
			}
		}
	}

	return iExperience;
}

// FreeBonuses of the traits (added by this mod): in the trade network of the capital (CvPlot::updatePlotGroupBonus)
int CvPlayer::getTraitFreeBonus(BonusTypes eBonus) const
{
	return sumTraitValues(&CvTraitInfo::getFreeBonus, eBonus);
}

// BonusProductionModifiers of the traits (added by this mod): production % in a city with eBonus. bWorldWonder: also the
// items with bWorldWondersOnly
int CvPlayer::getTraitBonusProductionModifier(BonusTypes eBonus, bool bWorldWonder) const
{
	int iModifier = sumTraitValues(&CvTraitInfo::getBonusProductionModifier, eBonus);

	if (bWorldWonder)
	{
		iModifier += sumTraitValues(&CvTraitInfo::getBonusWonderProductionModifier, eBonus);
	}

	return iModifier;
}

// CommerceHappinesses and CommerceHealths of the traits (added by this mod): per 100% of the commerce rate
int CvPlayer::getTraitCommerceHappiness(CommerceTypes eCommerce) const
{
	return sumTraitValues(&CvTraitInfo::getCommerceHappiness, eCommerce);
}

int CvPlayer::getTraitCommerceHealth(CommerceTypes eCommerce) const
{
	return sumTraitValues(&CvTraitInfo::getCommerceHealth, eCommerce);
}

// iHealRateChange, DomainFreeExperiences and DomainProductionModifiers of the traits (added by this mod): as the
// buildings, for every city (CvCity::getHealRate, getDomainFreeExperience, getDomainProductionModifier)
int CvPlayer::getTraitHealRateChange() const
{
	return sumTraitValues(&CvTraitInfo::getHealRateChange);
}

int CvPlayer::getTraitDomainFreeExperience(DomainTypes eDomain) const
{
	return sumTraitValues(&CvTraitInfo::getDomainFreeExperience, eDomain);
}

int CvPlayer::getTraitDomainProductionModifier(DomainTypes eDomain) const
{
	return sumTraitValues(&CvTraitInfo::getDomainProductionModifier, eDomain);
}

// bConquestStateReligion of the traits (added by this mod): see acquireCity
bool CvPlayer::isConquestStateReligion() const
{
	return hasTraitFlag(&CvTraitInfo::isConquestStateReligion);
}

// ConquestReligionBuildings of the traits (added by this mod): see acquireCity
bool CvPlayer::isConquestReligionBuilding(SpecialBuildingTypes eSpecialBuilding) const
{
	int iI;

	if (eSpecialBuilding == NO_SPECIALBUILDING)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).isConquestReligionBuilding(eSpecialBuilding) && hasTrait((TraitTypes)iI))
		{
			return true;
		}
	}

	return false;
}

// NoTechCivics of the traits (added by this mod): the player can adopt the civic without its tech
bool CvPlayer::isNoTechCivic(CivicTypes eCivic) const
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).isNoTechCivic(eCivic) && hasTrait((TraitTypes)iI))
		{
			return true;
		}
	}

	return false;
}

// EnemyAttrition/iLimit of the traits with EnemyAttrition/iPercent (added by this mod): the largest one, 0 to 100
int CvPlayer::getEnemyAttritionLimit() const
{
	int iLimit = 0;
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).getEnemyAttritionInBorders() != 0 && hasTrait((TraitTypes)iI))
		{
			iLimit = std::max(iLimit, GC.getTraitInfo((TraitTypes)iI).getEnemyAttritionLimit());
		}
	}

	return range(iLimit, 0, 100);
}

// EnemyAttrition/iPercent of the traits (added by this mod, as the Russian power of Rhye's and Fall of Civilization): at the
// start of the turn of this player, the units of the players at war with it in its territory lose that % of their max HP,
// down to EnemyAttrition/iLimit (a unit that reaches its max HP dies, killed by this player). All domains
void CvPlayer::doEnemyAttrition()
{
	std::vector<IDInfo> aUnits;
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iDamage = getEnemyAttritionInBorders();
	int iLimit;
	int iMaxDamage;
	int iNewDamage;
	int iI;

	if (iDamage <= 0)
	{
		return;
	}

	iLimit = getEnemyAttritionLimit();

	// The units first: a unit that dies can take others (its cargo) with it
	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if (pLoopPlot->getOwnerINLINE() != getID())
		{
			continue;
		}

		pUnitNode = pLoopPlot->headUnitNode();
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL && GET_TEAM(getTeam()).isAtWar(pLoopUnit->getTeam()))
			{
				aUnits.push_back(pLoopUnit->getIDInfo());
			}
		}
	}

	for (iI = 0; iI < (int)aUnits.size(); iI++)
	{
		pLoopUnit = ::getUnit(aUnits[iI]);
		if (pLoopUnit == NULL || pLoopUnit->maxHitPoints() <= 0)
		{
			continue;
		}

		iMaxDamage = (pLoopUnit->maxHitPoints() * iLimit) / 100;
		iNewDamage = std::min(iMaxDamage, pLoopUnit->getDamage() + ((pLoopUnit->maxHitPoints() * iDamage) / 100));
		if (iNewDamage > pLoopUnit->getDamage())
		{
			pLoopUnit->setDamage(iNewDamage, getID());
		}
	}
}

// ExplorationRewards of the traits (added by this mod): the kind of a plot for this player. Land is home (the region of the
// capital, CvPlot::isOverseas), else overseas in the New World or not (CvPlot::isNewWorld). Home goes first, so the land of
// a capital in the New World is home
ExplorePlotTypes CvPlayer::getExplorePlotType(const CvPlot* pPlot) const
{
	if (pPlot == NULL)
	{
		return NO_EXPLORE_PLOT;
	}

	if (pPlot->isWater())
	{
		return EXPLORE_PLOT_WATER;
	}

	if (!pPlot->isOverseas(getID()))
	{
		return EXPLORE_PLOT_HOME;
	}

	return (pPlot->isNewWorld() ? EXPLORE_PLOT_OVERSEAS_NEW_WORLD : EXPLORE_PLOT_OVERSEAS_OLD_WORLD);
}

int CvPlayer::getExploredPlotCount(ExplorePlotTypes eExplorePlot) const
{
	return (eExplorePlot >= 0 && eExplorePlot < NUM_EXPLORE_PLOT_TYPES) ? m_aiExploredPlotCount[eExplorePlot] : 0;
}

// A plot was revealed to the team of this player for the first time (CvPlot::setRevealed decides which reveals count). The
// kind is decided now, so moving the capital later in the turn does not change it. Every player counts, so a trait gained
// later in the turn gets the plots of the whole turn
void CvPlayer::onPlotExplored(const CvPlot* pPlot)
{
	ExplorePlotTypes eExplorePlot;

	// Not during the advanced start (the visibility bought there can be sold back)
	if (!isAlive() || getAdvancedStartPoints() >= 0)
	{
		return;
	}

	eExplorePlot = getExplorePlotType(pPlot);
	if (eExplorePlot != NO_EXPLORE_PLOT)
	{
		m_aiExploredPlotCount[eExplorePlot]++;
	}
}

// ExplorationRewards of the traits (added by this mod): at the end of the turn of this player (the start of doTurn), each
// plot revealed during the turn gives the sum of iPercent of its kind and the commerce in 1/100. What is less than 1 is
// carried to the next turn. Gold to the treasury, research to the current research (to the overflow without one), culture
// to the capital, espionage divided among the teams by the espionage weights. Then the counts start again from 0
void CvPlayer::doExplorationRewards()
{
	CvWString szRewards;
	CvWString szTemp;
	CvCity* pCapital;
	int aiReward[NUM_COMMERCE_TYPES];
	int iPercent;
	int iTotal;
	int iSpending;
	int iI;
	int iJ;

	for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		aiReward[iJ] = 0;
	}

	for (iI = 0; iI < NUM_EXPLORE_PLOT_TYPES; iI++)
	{
		for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			iPercent = sumTraitValues(&CvTraitInfo::getExplorationRewardPercentFor, iI, iJ);
			if (iPercent <= 0)
			{
				continue;
			}

			iTotal = m_aiExploredPlotCount[iI] * iPercent + m_aiExplorationRemainder[iI * NUM_COMMERCE_TYPES + iJ];
			aiReward[iJ] += iTotal / 100;
			m_aiExplorationRemainder[iI * NUM_COMMERCE_TYPES + iJ] = iTotal % 100;
		}
	}

	for (iI = 0; iI < NUM_EXPLORE_PLOT_TYPES; iI++)
	{
		m_aiExploredPlotCount[iI] = 0;
	}

	if (aiReward[COMMERCE_GOLD] > 0)
	{
		changeGold(aiReward[COMMERCE_GOLD]);
	}

	if (aiReward[COMMERCE_RESEARCH] > 0)
	{
		receiveResearch(aiReward[COMMERCE_RESEARCH]);
	}

	if (aiReward[COMMERCE_CULTURE] > 0)
	{
		pCapital = getCapitalCity();
		if (pCapital != NULL)
		{
			pCapital->changeCulture(getID(), aiReward[COMMERCE_CULTURE], true, true);
		}
		else
		{
			aiReward[COMMERCE_CULTURE] = 0;
		}
	}

	if (aiReward[COMMERCE_ESPIONAGE] > 0)
	{
		GET_TEAM(getTeam()).changeEspionagePointsEver(aiReward[COMMERCE_ESPIONAGE]);
		for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (iI != getTeam() && GET_TEAM((TeamTypes)iI).isAlive() && GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
			{
				iSpending = getEspionageSpending((TeamTypes)iI, aiReward[COMMERCE_ESPIONAGE]);
				if (iSpending > 0)
				{
					GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam((TeamTypes)iI, iSpending);
				}
			}
		}
	}

	// One message for the turn
	if (isHuman())
	{
		for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			if (aiReward[iJ] > 0)
			{
				szTemp.Format(L"%s+%d%c", (szRewards.empty() ? L"" : L", "), aiReward[iJ], GC.getCommerceInfo((CommerceTypes)iJ).getChar());
				szRewards += szTemp;
			}
		}

		if (!szRewards.empty())
		{
			gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_EXPLORATION_REWARD", szRewards.GetCString()), NULL, MESSAGE_TYPE_MINOR_EVENT);
		}
	}
}

// FeatureProductionChanges of the traits (added by this mod): production added when a worker removes the feature
int CvPlayer::getFeatureProductionChange(FeatureTypes eFeature) const
{
	return sumTraitValues(&CvTraitInfo::getFeatureProductionChange, (int)eFeature);
}

// FeatureGrowthChanges of the traits (added by this mod): growth chance (1/10000 per turn) for each adjacent plot with the feature
int CvPlayer::getFeatureGrowthChange(FeatureTypes eFeature) const
{
	return sumTraitValues(&CvTraitInfo::getFeatureGrowthChange, (int)eFeature);
}

// FeatureHealthChanges of the traits (added by this mod): added to iHealthPercent of the feature in the city radius
int CvPlayer::getFeatureHealthChange(FeatureTypes eFeature) const
{
	return sumTraitValues(&CvTraitInfo::getFeatureHealthChange, (int)eFeature);
}

// True if a trait, current civic or owned building lets eUnitCombat units upgrade outside the team's territory.
// bEnemyTerritory: the unit stands in the territory of a team we are at war with; sources with
// bExcludeEnemyTerritory do not count there.
bool CvPlayer::isUpgradeAnywhere(UnitCombatTypes eUnitCombat, bool bEnemyTerritory) const
{
	int iI;

	FAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat is expected to be within maximum bounds");

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);
			if (kTrait.isUpgradeAnywhereUnitCombat(eUnitCombat) && !(bEnemyTerritory && kTrait.isUpgradeAnywhereExcludeEnemy(eUnitCombat)))
			{
				return true;
			}
		}
	}

	for (iI = 0; iI < getNumCivicEntries(); iI++)	// every civic slot (changed by this mod)
	{
		if (getCivicByEntry(iI) != NO_CIVIC)
		{
			CvCivicInfo& kCivic = GC.getCivicInfo(getCivicByEntry(iI));
			if (kCivic.isUpgradeAnywhereUnitCombat(eUnitCombat) && !(bEnemyTerritory && kCivic.isUpgradeAnywhereExcludeEnemy(eUnitCombat)))
			{
				return true;
			}
		}
	}

	// Buildings count if any of our cities has one that is not obsolete.
	// Cities are only scanned for the (usually few) buildings that have the entry.
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);
		if (kBuilding.isUpgradeAnywhereUnitCombat(eUnitCombat) && !(bEnemyTerritory && kBuilding.isUpgradeAnywhereExcludeEnemy(eUnitCombat)))
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->getNumActiveBuilding((BuildingTypes)iI) > 0)
				{
					return true;
				}
			}
		}
	}

	return false;
}

// FoundingOverlord of the traits (added by this mod): the first trait of the player with FoundingOverlord, or NO_TRAIT
static TraitTypes getFoundingOverlordTrait(const CvPlayer& kPlayer)
{
	int iI;

	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (GC.getTraitInfo((TraitTypes)iI).isFoundingOverlord() && kPlayer.hasTrait((TraitTypes)iI))
		{
			return (TraitTypes)iI;
		}
	}

	return NO_TRAIT;
}

// FoundingOverlord of the traits (added by this mod): the starting plot of the master. Land that is passable, not owned,
// without units and goody huts. Plots on other areas (continents) than the one of pCity with at least
// FOUNDING_OVERLORD_MIN_AREA_TILES land tiles come first when there are some (no small islands). Among those plots, the
// ones at least half as far from pCity as the farthest one are far enough, and the best of them is chosen: the farthest plot
// alone was often a poor polar island. With eNewPlayer the value is its AI_foundValue as a starting location (plots without
// a value are left out); with NO_PLAYER every plot has the same value and the farthest wins (a check that a plot exists,
// and the fallback)
static const int FOUNDING_OVERLORD_MIN_AREA_TILES = 20;

CvPlot* CvPlayer::findFoundingOverlordPlot(const CvCity* pCity, PlayerTypes eNewPlayer) const
{
	std::vector<CvPlot*> apPlots;
	std::vector<int> aiValues;
	std::vector<int> aiDistances;
	std::vector<bool> abOtherArea;
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	bool bOtherArea;
	bool bUseOtherArea;
	int iValue;
	int iDistance;
	int iMaxDistance;
	int iBestValue;
	int iBestDistance;
	int iI;

	bUseOtherArea = false;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->isWater() || pLoopPlot->isImpassable() || pLoopPlot->isOwned() || pLoopPlot->getNumUnits() > 0 || pLoopPlot->isGoody())
		{
			continue;
		}

		if (eNewPlayer == NO_PLAYER)
		{
			iValue = 1;
		}
		else
		{
			iValue = GET_PLAYER(eNewPlayer).AI_foundValue(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), -1, true);
		}

		if (iValue <= 0)
		{
			continue;
		}

		bOtherArea = (pLoopPlot->area() != pCity->area() && pLoopPlot->area()->getNumTiles() >= FOUNDING_OVERLORD_MIN_AREA_TILES);
		bUseOtherArea = bUseOtherArea || bOtherArea;

		apPlots.push_back(pLoopPlot);
		aiValues.push_back(iValue);
		aiDistances.push_back(plotDistance(pCity->getX_INLINE(), pCity->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()));
		abOtherArea.push_back(bOtherArea);
	}

	// The farthest candidate of the chosen group (the big other areas, or all the plots when they have none)
	iMaxDistance = -1;
	for (iI = 0; iI < (int)apPlots.size(); iI++)
	{
		if (!bUseOtherArea || abOtherArea[iI])
		{
			iMaxDistance = std::max(iMaxDistance, aiDistances[iI]);
		}
	}

	if (iMaxDistance < 0)
	{
		return NULL;
	}

	pBestPlot = NULL;
	iBestValue = 0;
	iBestDistance = -1;

	for (iI = 0; iI < (int)apPlots.size(); iI++)
	{
		if ((bUseOtherArea && !abOtherArea[iI]) || aiDistances[iI] * 2 < iMaxDistance)
		{
			continue;
		}

		iValue = aiValues[iI];
		iDistance = aiDistances[iI];
		if (iValue > iBestValue || (iValue == iBestValue && iDistance > iBestDistance))
		{
			iBestValue = iValue;
			iBestDistance = iDistance;
			pBestPlot = apPlots[iI];
		}
	}

	return pBestPlot;
}

// FoundingOverlord of the traits (added by this mod): when the player founds its first city, an AI civilization joins the
// game far away and the player becomes its vassal by a vassal agreement (so the player can cancel it after the peace treaty
// length, as any vassal agreement). Once per player, with the first trait that has FoundingOverlord. Nothing happens with No
// Vassal States, when no player slot is free, when no civilization or leader is left, or when no plot is found
void CvPlayer::doFoundingOverlord(CvCity* pCity)
{
	CvPlot* pFallbackPlot;
	CvPlot* pStartPlot;
	PlayerTypes eNewPlayer;
	CivilizationTypes eCiv;
	LeaderHeadTypes eLeader;
	TraitTypes eTrait;
	TeamTypes eNewTeam;
	bool bValid;
	int iI, iJ;

	if (m_bFoundingOverlordDone || pCity == NULL || isBarbarian() || isMinorCiv())
	{
		return;
	}

	eTrait = getFoundingOverlordTrait(*this);
	if (eTrait == NO_TRAIT)
	{
		return;
	}

	m_bFoundingOverlordDone = true;

	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_VASSAL_STATES) || GET_TEAM(getTeam()).isAVassal())
	{
		return;
	}

	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

	pFallbackPlot = findFoundingOverlordPlot(pCity, NO_PLAYER);
	if (pFallbackPlot == NULL)
	{
		return;
	}

	eNewPlayer = findJoiningPlayerSlot();
	if (eNewPlayer == NO_PLAYER)
	{
		return;
	}

	if (!chooseJoiningCivAndLeader((CivilizationTypes)kTrait.getFoundingOverlordCivilization(), kTrait.getFoundingOverlordCivilizationsVector(), (LeaderHeadTypes)kTrait.getFoundingOverlordLeader(), eCiv, eLeader))
	{
		return;
	}

	initJoiningPlayer(eNewPlayer, eCiv, eLeader, (HandicapTypes)kTrait.getFoundingOverlordHandicap());

	CvPlayer& kNewPlayer = GET_PLAYER(eNewPlayer);
	eNewTeam = kNewPlayer.getTeam();

	// The free techs of the start of the game (as CvGame::initFreeState): of the difficulty, the eras before the start era and
	// the civilization
	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		bValid = (GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).isFreeTechs(iI) || GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).isAIFreeTechs(iI));
		bValid = bValid || (GC.getTechInfo((TechTypes)iI).getEra() < GC.getGameINLINE().getStartEra());
		bValid = bValid || GC.getCivilizationInfo(eCiv).isCivilizationFreeTechs(iI);
		if (bValid && !GET_TEAM(eNewTeam).isHasTech((TechTypes)iI))
		{
			GET_TEAM(eNewTeam).setHasTech((TechTypes)iI, true, NO_PLAYER, false, false);
		}
	}

	pStartPlot = findFoundingOverlordPlot(pCity, eNewPlayer);
	if (pStartPlot == NULL)
	{
		pStartPlot = pFallbackPlot;
	}
	kNewPlayer.setStartingPlot(pStartPlot, true);

	GET_TEAM(getTeam()).meet(eNewTeam, false);

	// The units: all the starting units of an AI at the difficulty of its AI modifiers, as for FoundingVassal
	kNewPlayer.addStartingUnits();
	kNewPlayer.doRandomBasicTraits();

	// The vassal agreement, as for a colony (CvTeam::assignVassal: the vassal state and a saved deal that the vassal can cancel
	// after the peace treaty length; a deal made through implementDeal would fail the trade checks for a human vassal)
	GET_TEAM(eNewTeam).assignVassal(getTeam(), false);

	if (GET_TEAM(getTeam()).isVassal(eNewTeam))
	{
		m_eFoundingOverlord = eNewPlayer;
		m_iIndependenceState = 1;
	}

	// The message, to the players that know the new team
	CvWString szMessage = gDLL->getText("TXT_KEY_MISC_FOUNDING_OVERLORD_JOINED", GC.getLeaderHeadInfo(eLeader).getTextKeyWide(), GC.getCivilizationInfo(eCiv).getShortDescriptionKey(), getNameKey());
	for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
	{
		if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).isHasMet(eNewTeam))
		{
			gDLL->getInterfaceIFace()->addMessage((PlayerTypes)iJ, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_CITY_BAR_CAPITAL_TEXTURE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), pStartPlot->getX_INLINE(), pStartPlot->getY_INLINE(), true, true);
		}
	}
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szMessage, pStartPlot->getX_INLINE(), pStartPlot->getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

	GC.getGameINLINE().updateScore(true);
}

PlayerTypes CvPlayer::getFoundingOverlord() const
{
	return m_eFoundingOverlord;
}

int CvPlayer::getIndependenceState() const
{
	return m_iIndependenceState;
}

// FoundingOverlord of the traits (added by this mod): any end of the vassal state (a cancelled agreement, a refused demand,
// a freed vassal) means the war of independence. The master declares war unless the war is already on. Called after a deal
// is cancelled and at the start of the turn of the player
void CvPlayer::checkIndependenceWar()
{
	if (m_iIndependenceState != 1 || m_eFoundingOverlord == NO_PLAYER || !isAlive())
	{
		return;
	}

	CvPlayer& kOverlord = GET_PLAYER(m_eFoundingOverlord);
	if (!kOverlord.isAlive())
	{
		m_iIndependenceState = 3;
		return;
	}

	TeamTypes eOverlordTeam = kOverlord.getTeam();
	if (GET_TEAM(getTeam()).isVassal(eOverlordTeam))
	{
		return;
	}

	if (GET_TEAM(getTeam()).isAtWar(eOverlordTeam))
	{
		startIndependenceWar(eOverlordTeam);
		return;
	}

	GET_TEAM(eOverlordTeam).declareWar(getTeam(), false, WARPLAN_TOTAL);
}

// FoundingOverlord of the traits (added by this mod): the player, still the vassal of its master, may declare war on it (the
// war of independence, CvTeam::canChangeWarPeace) once it may cancel the vassal agreement: after the peace treaty length
// (CvDeal::isCancelable of the deal with TRADE_VASSAL). Without that deal (cancelled in another way) it may at once
bool CvPlayer::canDeclareIndependenceWar() const
{
	CvDeal* pLoopDeal;
	CLLNode<TradeData>* pNode;
	int iLoop;

	if (m_iIndependenceState != 1 || m_eFoundingOverlord == NO_PLAYER || !GET_PLAYER(m_eFoundingOverlord).isAlive())
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isVassal(GET_PLAYER(m_eFoundingOverlord).getTeam()))
	{
		return false;
	}

	for (pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (!((pLoopDeal->getFirstPlayer() == getID() && pLoopDeal->getSecondPlayer() == m_eFoundingOverlord) || (pLoopDeal->getFirstPlayer() == m_eFoundingOverlord && pLoopDeal->getSecondPlayer() == getID())))
		{
			continue;
		}

		bool bVassalDeal = false;
		for (pNode = pLoopDeal->headFirstTradesNode(); pNode != NULL; pNode = pLoopDeal->nextFirstTradesNode(pNode))
		{
			bVassalDeal = bVassalDeal || (pNode->m_data.m_eItemType == TRADE_VASSAL);
		}
		for (pNode = pLoopDeal->headSecondTradesNode(); pNode != NULL; pNode = pLoopDeal->nextSecondTradesNode(pNode))
		{
			bVassalDeal = bVassalDeal || (pNode->m_data.m_eItemType == TRADE_VASSAL);
		}

		if (bVassalDeal)
		{
			return pLoopDeal->isCancelable(getID());
		}
	}

	return true;
}

// FoundingOverlord of the traits (added by this mod): a war between the team of the player and eOtherTeam began. When it is
// the master of FoundingOverlord and the player was its vassal, the war of independence begins
void CvPlayer::startIndependenceWar(TeamTypes eOtherTeam)
{
	if (m_iIndependenceState != 1 || m_eFoundingOverlord == NO_PLAYER || GET_PLAYER(m_eFoundingOverlord).getTeam() != eOtherTeam)
	{
		return;
	}

	m_iIndependenceState = 2;
	m_iIndependenceWarTurn = GC.getGameINLINE().getGameTurn();

	CvWString szMessage = gDLL->getText("TXT_KEY_MISC_INDEPENDENCE_WAR_BEGINS", GET_PLAYER(m_eFoundingOverlord).getCivilizationShortDescriptionKey());
	gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
}

// FoundingOverlord of the traits (added by this mod): the war with eOtherTeam ends, by peace (CvTeam::makePeace, before the
// war success is reset) or because the master is destroyed (bEliminated, CvPlayer::setAlive). Once per player: the golden
// age of the result, in percent of the normal length (the values of the first trait with FoundingOverlord)
// - the master destroyed while at war: iEliminateGoldenAgePercent
// - peace while the player holds a city it took from the master in this war (not one it founded): iCityGoldenAgePercent
// - peace while the war success of the team (CvTeamAI::AI_getWarSuccess) is higher than the one of the master: iPeaceGoldenAgePercent
// - otherwise no golden age
void CvPlayer::endIndependenceWar(TeamTypes eOtherTeam, bool bEliminated)
{
	CvCity* pLoopCity;
	TraitTypes eTrait;
	int iPercent;
	int iLoop;
	const char* szKey;

	if (m_eFoundingOverlord == NO_PLAYER || GET_PLAYER(m_eFoundingOverlord).getTeam() != eOtherTeam)
	{
		return;
	}

	if (m_iIndependenceState == 1 && bEliminated)
	{
		m_iIndependenceState = 3;	// the master is gone before any war of independence
		return;
	}

	if (m_iIndependenceState != 2)
	{
		return;
	}

	m_iIndependenceState = 3;

	eTrait = getFoundingOverlordTrait(*this);
	iPercent = 0;
	szKey = "TXT_KEY_MISC_INDEPENDENCE_NO_GOLDEN_AGE";

	if (bEliminated)
	{
		if (GET_TEAM(getTeam()).isAtWar(eOtherTeam))
		{
			iPercent = (eTrait != NO_TRAIT) ? GC.getTraitInfo(eTrait).getIndependenceEliminateGoldenAgePercent() : 0;
			szKey = "TXT_KEY_MISC_INDEPENDENCE_ELIMINATE";
		}
	}
	else
	{
		bool bHoldsCity = false;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->getPreviousOwner() == m_eFoundingOverlord && pLoopCity->getGameTurnAcquired() >= m_iIndependenceWarTurn && GET_PLAYER(pLoopCity->getOriginalOwner()).getTeam() != getTeam())
			{
				bHoldsCity = true;
				break;
			}
		}

		if (bHoldsCity)
		{
			iPercent = (eTrait != NO_TRAIT) ? GC.getTraitInfo(eTrait).getIndependenceCityGoldenAgePercent() : 0;
			szKey = "TXT_KEY_MISC_INDEPENDENCE_CITY";
		}
		else if (GET_TEAM(getTeam()).AI_getWarSuccess(eOtherTeam) > GET_TEAM(eOtherTeam).AI_getWarSuccess(getTeam()))
		{
			iPercent = (eTrait != NO_TRAIT) ? GC.getTraitInfo(eTrait).getIndependencePeaceGoldenAgePercent() : 0;
			szKey = "TXT_KEY_MISC_INDEPENDENCE_PEACE";
		}
	}

	if (iPercent <= 0)
	{
		szKey = "TXT_KEY_MISC_INDEPENDENCE_NO_GOLDEN_AGE";
	}

	CvWString szMessage = gDLL->getText(szKey, iPercent);
	gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

	if (iPercent > 0)
	{
		changeGoldenAgeTurns(std::max(1, (getGoldenAgeLength() * iPercent) / 100));
	}
}

// iTributeDemandTurns of the traits (added by this mod): the turns between two demands of resources by an AI master, scaled
// by the growth percent of the game speed (0 = no limit)
int CvPlayer::getTributeDemandTurns() const
{
	int iTurns = sumTraitValues(&CvTraitInfo::getTributeDemandTurns);

	if (iTurns > 0)
	{
		iTurns = std::max(1, (iTurns * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent()) / 100);
	}

	return iTurns;
}

// An AI master does not demand a resource from this player now (CvPlayerAI::AI_doDiplo)
bool CvPlayer::isTributeDemandWaiting() const
{
	int iTurns = getTributeDemandTurns();

	return (iTurns > 0 && m_iLastTributeDemandTurn >= 0 && GC.getGameINLINE().getGameTurn() < m_iLastTributeDemandTurn + iTurns);
}

void CvPlayer::setTributeDemanded()
{
	m_iLastTributeDemandTurn = GC.getGameINLINE().getGameTurn();
}
