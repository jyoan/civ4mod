#include "CvGameCoreDLL.h"
#include "CvInfos.h"

//
// Python interface for info classes, continued (added by this mod): CyInfoInterface3.cpp is at the limit of MSVC 7.1
// (fatal error C1204: compiler limit : internal structure overflow), so the getters added later go here
//

// The getters of CvTraitInfo added on 2026-11-18 and later. Called from CyInfoPythonInterface3 with the class
void CyInfoPythonInterfaceTrait2(python::class_<CvTraitInfo, python::bases<CvInfoBase> >& kTraitInfo)
{
	kTraitInfo
		.def("getMaxExperienceAfterUpgrade", &CvTraitInfo::getMaxExperienceAfterUpgrade, "int () - 0 = not set, -1 = keeps all")
		.def("getExpOutsideBordersChange", &CvTraitInfo::getExpOutsideBordersChange, "int ()")
		.def("getWeLoveTheKingChance", &CvTraitInfo::getWeLoveTheKingChance, "int ()")
		.def("getSpecialistSlotChange", &CvTraitInfo::getSpecialistSlotChange, "int (int iSpecialist) - -1 = no limit")
		.def("isUnitHurry", &CvTraitInfo::isUnitHurry, "bool (int iHurry)")
		.def("getHurryUnitExperience", &CvTraitInfo::getHurryUnitExperience, "int (int iHurry)")
		.def("getImprovedBonusYieldChange", &CvTraitInfo::getImprovedBonusYieldChange, "int (int iBonus, int iYield)")
		.def("getHappyYieldPercent", &CvTraitInfo::getHappyYieldPercent, "int (int i)")
		.def("getHappyCommercePercent", &CvTraitInfo::getHappyCommercePercent, "int (int i)")
		.def("getHealthyYieldPercent", &CvTraitInfo::getHealthyYieldPercent, "int (int i)")
		.def("getHealthyCommercePercent", &CvTraitInfo::getHealthyCommercePercent, "int (int i)")
		;
}
