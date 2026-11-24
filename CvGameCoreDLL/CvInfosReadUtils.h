#pragma once

// CvInfosReadUtils.h (added by this mod)
//
// XML reading helpers shared by CvInfos.cpp and CvTraitInfo.cpp. They are defined in CvInfos.cpp.

#ifndef CV_INFOS_READ_UTILS_H
#define CV_INFOS_READ_UTILS_H

class CvXMLLoadUtility;

void readTypeList(CvXMLLoadUtility* pXML, const TCHAR* szListTag, std::vector<int>& aiTypes);
void readTypeList(CvXMLLoadUtility* pXML, const TCHAR* szListTag, std::vector<CvString>& aszTypes);
void readTypeValueList(CvXMLLoadUtility* pXML, const TCHAR* szListTag, const TCHAR* szItemTag, const TCHAR* szTypeTag, const TCHAR* szValueTag, int iNumInfos, std::vector<int>& aiValues);
void readTypeStringValueList(CvXMLLoadUtility* pXML, const TCHAR* szListTag, const TCHAR* szItemTag, const TCHAR* szTypeTag, const TCHAR* szValueTag, std::vector<CvString>& aszTypes, std::vector<int>& aiValues);
void readYieldChanges(CvXMLLoadUtility* pXML, const TCHAR* szTag, int** ppiChanges);
void readYieldChanges(CvXMLLoadUtility* pXML, const TCHAR* szListTag, const TCHAR* szItemTag, const TCHAR* szTypeTag, const TCHAR* szYieldsTag, std::vector<CvString>& aszTypes, std::vector< std::vector<int> >& aaiYields, bool bCommerce = false);
void resolveYieldChanges(const TCHAR* szListTag, const std::vector<CvString>& aszTypes, const std::vector< std::vector<int> >& aaiYields, int iNumInfos, std::vector< std::vector<int> >& aaiYieldChanges, int iNumValues = NUM_YIELD_TYPES, const TCHAR* szFile = "CIV4TraitInfos.xml");
void readCommerceChanges(CvXMLLoadUtility* pXML, const TCHAR* szTag, int** ppiChanges);
void readUpgradeAnywhereUnitCombats(CvXMLLoadUtility* pXML, bool** ppabUpgradeAnywhere, bool** ppabExcludeEnemy);

#endif
