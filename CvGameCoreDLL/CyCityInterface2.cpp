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
		.def("getMiseryYieldByBuilding", &CyCity::getMiseryYieldByBuilding, "int (int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) - the changes per angry citizen and per unhealthiness of the building (added by this mod)")
		.def("getMiseryCommerceByBuilding", &CyCity::getMiseryCommerceByBuilding, "int (int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) - the changes per angry citizen and per unhealthiness of the building (added by this mod)")
		;
}
