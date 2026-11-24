#pragma once

// CvGameTextMgrUtils.h (added by this mod)
//
// Help text helpers shared by CvGameTextMgr.cpp and CvGameTextMgrTrait.cpp. getBuildResultDescription is defined in
// CvGameTextMgr.cpp, getLinkText and getConditionalFreeTraitConditions in CvGameTextMgrTrait.cpp.

#ifndef CV_GAME_TEXT_MGR_UTILS_H
#define CV_GAME_TEXT_MGR_UTILS_H

struct ConditionalFreeTrait;

// What the build makes: the improvement, the route, or the build itself
const wchar* getBuildResultDescription(const CvBuildInfo& kBuild);
// A name that links to its Civilopedia page
CvWString getLinkText(const wchar* szName);
// The conditions of an item of ConditionalFreeTraits as text (see CvGameTextMgrTrait.cpp)
CvWString getConditionalFreeTraitConditions(const ConditionalFreeTrait& kCondition, bool bShowRecord, PlayerTypes eActivePlayer, int iSkipCivic);

// The start bias of a civilization or a trait as a list: "Coast +200%, Desert +40% per tile" (added by this mod)
template <class T>
CvWString getStartBiasText(const T& kInfo)
{
	CvWString szList;
	int iI;

	if (kInfo.getStartBiasCoastal() != 0)
	{
		szList += gDLL->getText("TXT_KEY_START_BIAS_COASTAL", kInfo.getStartBiasCoastal());
	}
	if (kInfo.getStartBiasRiver() != 0)
	{
		szList += (szList.empty() ? L"" : L", ");
		szList += gDLL->getText("TXT_KEY_START_BIAS_RIVER", kInfo.getStartBiasRiver());
	}
	for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (kInfo.getStartBiasTerrain(iI) != 0)
		{
			szList += (szList.empty() ? L"" : L", ");
			szList += gDLL->getText("TXT_KEY_START_BIAS_PLOT", GC.getTerrainInfo((TerrainTypes)iI).getTextKeyWide(), kInfo.getStartBiasTerrain(iI));
		}
	}
	for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		if (kInfo.getStartBiasFeature(iI) != 0)
		{
			szList += (szList.empty() ? L"" : L", ");
			szList += gDLL->getText("TXT_KEY_START_BIAS_PLOT", GC.getFeatureInfo((FeatureTypes)iI).getTextKeyWide(), kInfo.getStartBiasFeature(iI));
		}
	}
	for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (kInfo.getStartBiasBonus(iI) != 0)
		{
			szList += (szList.empty() ? L"" : L", ");
			szList += gDLL->getText("TXT_KEY_START_BIAS_PLOT", GC.getBonusInfo((BonusTypes)iI).getTextKeyWide(), kInfo.getStartBiasBonus(iI));
		}
	}

	return szList;
}

#endif
