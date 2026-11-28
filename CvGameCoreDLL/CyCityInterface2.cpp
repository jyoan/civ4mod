#include "CvGameCoreDLL.h"
#include "CyCity.h"

//
// published python interface for CyCity (added by this mod: CyCityInterface1.cpp is at the limit of the debug information
// of MSVC 7.1)
//

void CyCityPythonInterface2(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface2\n");

	x
		.def("getCultureRange", &CyCity::getCultureRange, "int () - how far the borders reach, with iCultureRangeChange of the traits (added by this mod)")
		.def("getMiseryYieldByBuilding", &CyCity::getMiseryYieldByBuilding, "int (int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) - the changes per angry citizen and per unhealthiness of the building (added by this mod)")
		.def("getMiseryCommerceByBuilding", &CyCity::getMiseryCommerceByBuilding, "int (int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) - the changes per angry citizen and per unhealthiness of the building (added by this mod)")
		.def("canUnsettleGreatPerson", &CyCity::canUnsettleGreatPerson, "bool (int /*SpecialistTypes*/ eSpecialist) - a settled great person can become a unit (bUnsettleGreatPeople of the traits, added by this mod)")
		.def("isNoAnarchyDisorder", &CyCity::isNoAnarchyDisorder, "bool () - a specialist of the city keeps it working during the anarchy (bNoAnarchyDisorder, added by this mod)")
		.def("unsettleGreatPerson", &CyCity::unsettleGreatPerson, "bool (int /*SpecialistTypes*/ eSpecialist) - the settled great person becomes a unit in the city; call it on every computer (added by this mod)")
		.def("getTotalGreatPeopleUnitProgress", &CyCity::getTotalGreatPeopleUnitProgress, "int () - the great people points stored for each great person unit, added up (added by this mod)")
		.def("getGreatPeopleHurryPoints", &CyCity::getGreatPeopleHurryPoints, "int () - the great people points missing until the next great person (GreatPeopleHurry of the traits, added by this mod)")
		.def("getGreatPeopleHurryCost", &CyCity::getGreatPeopleHurryCost, "int () - the gold to buy the missing great people points (GreatPeopleHurry of the traits, added by this mod)")
		.def("canHurryGreatPeople", &CyCity::canHurryGreatPeople, "bool () - the owner can buy the great person of the city now (GreatPeopleHurry of the traits, added by this mod)")
		.def("hurryGreatPeople", &CyCity::hurryGreatPeople, "bool () - the owner buys the missing points and the great person is born; call it on every computer (GreatPeopleHurry of the traits, added by this mod)")
		.def("getImprovementProduction", &CyCity::getImprovementProduction, "int () - production from replacing improvements, spent with the next production like chopping (added by this mod)")
		.def("changeImprovementProduction", &CyCity::changeImprovementProduction, "void (int iChange) - adds production from replacing improvements (shown in its own line of the production help; added by this mod)")
		.def("countCityPlots", &CyCity::countCityPlots, "int (int eTerrain, int eFeature) - plots of the city radius in our territory, not the city plot, of the terrain and/or feature (-1: any; TERRAIN_HILL for hills, TERRAIN_PEAK for peaks; added by this mod)")
		.def("getCityPlotBuildingProductionModifier", &CyCity::getCityPlotBuildingProductionModifier, "int (int eBuilding) - production % of the building from CityPlotBuildingProductionModifiers of the traits (added by this mod)")
		;
}
