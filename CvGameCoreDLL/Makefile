# Makefile for CvGameCoreDLL.dll (Civilization IV: Beyond the Sword, Japanese edition)
#
# Reproduces the "Final Release" configuration of CvGameCoreDLL.vcproj, which is
# the configuration that produces the shipped CvGameCoreDLL.dll.
#
# The DLL must be built with MSVC 7.1 (Visual C++ Toolkit 2003) so that it uses
# the same runtime as Civ4 (MSVCR71.dll / MSVCP71.dll). Do not call nmake directly;
# run ..\build.bat, which sets TOOLKIT / PSDK and a clean PATH / INCLUDE / LIB.
#
# Targets: all (default), clean
#
# Not built:
#   CvTextScreens.cpp  - not part of CvGameCoreDLL.vcproj
#   CvGameCoreDLL.rc   - version info only; needs MFC's afxres.h

!IFNDEF TOOLKIT
!ERROR TOOLKIT is not set. Run ..\build.bat instead of calling nmake directly.
!ENDIF
!IFNDEF PSDK
!ERROR PSDK is not set. Run ..\build.bat instead of calling nmake directly.
!ENDIF

INTDIR = Release
TARGET = $(INTDIR)\CvGameCoreDLL.dll
PCH    = $(INTDIR)\CvGameCoreDLL.pch

CC   = "$(TOOLKIT)\bin\cl.exe"
LINK = "$(TOOLKIT)\bin\link.exe"

# /X ignores the INCLUDE environment variable: only the paths below are searched.
INCLUDES = /X \
	/I "Boost-1.32.0\include" \
	/I "Python24\include" \
	/I "$(TOOLKIT)\include" \
	/I "$(PSDK)\Include"

DEFINES = /D WIN32 /D NDEBUG /D _WINDOWS /D _USRDLL /D _MBCS \
	/D CVGAMECOREDLL_EXPORTS /D FINAL_RELEASE /D CYBERFRONT

CFLAGS = /nologo /c /MD /O2 /Og /Oi /Oy /G7 /GR /EHsc /Gy /W3 /Zi \
	/Fd"$(INTDIR)\vc70.pdb" $(INCLUDES) $(DEFINES)

# Toolkit lib must come before the Platform SDK lib: both contain kernel32.lib and libcp.lib.
LIBPATHS = \
	/LIBPATH:"Python24\libs" \
	/LIBPATH:"Boost-1.32.0\libs" \
	/LIBPATH:"$(TOOLKIT)\lib" \
	/LIBPATH:"$(PSDK)\Lib"

# boost_python + winmm from the .vcproj, plus the Visual Studio 2003 default libraries.
LIBS = boost_python-vc71-mt-1_32.lib winmm.lib \
	kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
	shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

LDFLAGS = /nologo /DLL /INCREMENTAL:NO /DEBUG /PDB:"$(INTDIR)\CvGameCoreDLL.pdb" \
	/SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /OPT:REF /OPT:ICF \
	/IMPLIB:"$(INTDIR)\CvGameCoreDLL.lib" /MACHINE:X86 $(LIBPATHS)

# Every .cpp listed in CvGameCoreDLL.vcproj except _precompile.cpp (built separately below).
# New source files must be added here.
OBJS = \
	$(INTDIR)\CvArea.obj \
	$(INTDIR)\CvArtFileMgr.obj \
	$(INTDIR)\CvCity.obj \
	$(INTDIR)\CvCityAI.obj \
	$(INTDIR)\CvDeal.obj \
	$(INTDIR)\CvDiploParameters.obj \
	$(INTDIR)\CvDLLButtonPopup.obj \
	$(INTDIR)\CvDLLEntity.obj \
	$(INTDIR)\CvDLLPython.obj \
	$(INTDIR)\CvDllPythonEvents.obj \
	$(INTDIR)\CvDllTranslator.obj \
	$(INTDIR)\CvDLLWidgetData.obj \
	$(INTDIR)\CvEventReporter.obj \
	$(INTDIR)\CvFractal.obj \
	$(INTDIR)\CvGame.obj \
	$(INTDIR)\CvGameAI.obj \
	$(INTDIR)\CvGameCoreDLL.obj \
	$(INTDIR)\CvGameCoreUtils.obj \
	$(INTDIR)\CvGameInterface.obj \
	$(INTDIR)\CvGameTextMgr.obj \
	$(INTDIR)\CvGlobals.obj \
	$(INTDIR)\CvHallOfFameInfo.obj \
	$(INTDIR)\CvInfos.obj \
	$(INTDIR)\CvInfoWater.obj \
	$(INTDIR)\CvInitCore.obj \
	$(INTDIR)\CvMap.obj \
	$(INTDIR)\CvMapGenerator.obj \
	$(INTDIR)\CvMessageControl.obj \
	$(INTDIR)\CvMessageData.obj \
	$(INTDIR)\CvPlayer.obj \
	$(INTDIR)\CvPlayerAI.obj \
	$(INTDIR)\CvPlot.obj \
	$(INTDIR)\CvPlotGroup.obj \
	$(INTDIR)\CvPopupInfo.obj \
	$(INTDIR)\CvPopupReturn.obj \
	$(INTDIR)\CvRandom.obj \
	$(INTDIR)\CvReplayInfo.obj \
	$(INTDIR)\CvReplayMessage.obj \
	$(INTDIR)\CvSelectionGroup.obj \
	$(INTDIR)\CvSelectionGroupAI.obj \
	$(INTDIR)\CvStatistics.obj \
	$(INTDIR)\CvStructs.obj \
	$(INTDIR)\CvTalkingHeadMessage.obj \
	$(INTDIR)\CvTeam.obj \
	$(INTDIR)\CvTeamAI.obj \
	$(INTDIR)\CvUnit.obj \
	$(INTDIR)\CvUnitAI.obj \
	$(INTDIR)\CvXMLLoadUtility.obj \
	$(INTDIR)\CvXMLLoadUtilityGet.obj \
	$(INTDIR)\CvXMLLoadUtilityInit.obj \
	$(INTDIR)\CvXMLLoadUtilitySet.obj \
	$(INTDIR)\CyArea.obj \
	$(INTDIR)\CyAreaInterface.obj \
	$(INTDIR)\CyArgsList.obj \
	$(INTDIR)\CyArtFileMgr.obj \
	$(INTDIR)\CyArtFileMgrInterface.obj \
	$(INTDIR)\CyCity.obj \
	$(INTDIR)\CyCityInterface1.obj \
	$(INTDIR)\CyDeal.obj \
	$(INTDIR)\CyEnumsInterface.obj \
	$(INTDIR)\CyGame.obj \
	$(INTDIR)\CyGameCoreUtils.obj \
	$(INTDIR)\CyGameCoreUtilsInterface.obj \
	$(INTDIR)\CyGameInterface.obj \
	$(INTDIR)\CyGameTextMgr.obj \
	$(INTDIR)\CyGameTextMgrInterface.obj \
	$(INTDIR)\CyGlobalContext.obj \
	$(INTDIR)\CyGlobalContextInterface1.obj \
	$(INTDIR)\CyGlobalContextInterface2.obj \
	$(INTDIR)\CyGlobalContextInterface3.obj \
	$(INTDIR)\CyGlobalContextInterface4.obj \
	$(INTDIR)\CyHallOfFameInfo.obj \
	$(INTDIR)\CyHallOfFameInterface.obj \
	$(INTDIR)\CyInfoInterface1.obj \
	$(INTDIR)\CyInfoInterface2.obj \
	$(INTDIR)\CyInfoInterface3.obj \
	$(INTDIR)\CyMap.obj \
	$(INTDIR)\CyMapGenerator.obj \
	$(INTDIR)\CyMapGeneratorInterface.obj \
	$(INTDIR)\CyMapInterface.obj \
	$(INTDIR)\CyMessageControl.obj \
	$(INTDIR)\CyMessageControlInterface.obj \
	$(INTDIR)\CyPlayer.obj \
	$(INTDIR)\CyPlayerInterface1.obj \
	$(INTDIR)\CyPlayerInterface2.obj \
	$(INTDIR)\CyPlot.obj \
	$(INTDIR)\CyPlotInterface1.obj \
	$(INTDIR)\CyRandomInterface.obj \
	$(INTDIR)\CyReplayInfo.obj \
	$(INTDIR)\CySelectionGroup.obj \
	$(INTDIR)\CySelectionGroupInterface.obj \
	$(INTDIR)\CyStructsInterface1.obj \
	$(INTDIR)\CyTeam.obj \
	$(INTDIR)\CyTeamInterface.obj \
	$(INTDIR)\CyUnit.obj \
	$(INTDIR)\CyUnitInterface1.obj \
	$(INTDIR)\FAssert.obj \
	$(INTDIR)\FDialogTemplate.obj

all: $(TARGET)

$(TARGET): $(INTDIR)\_precompile.obj $(OBJS)
	$(LINK) @<<
$(LDFLAGS) /OUT:"$(TARGET)" $(INTDIR)\_precompile.obj $(OBJS) $(LIBS)
<<

# CvGameCoreDLL.h pulls in nearly every header, so any header change rebuilds
# the precompiled header and, through it, every object file.
$(INTDIR)\_precompile.obj: _precompile.cpp *.h *.inl
	@if not exist "$(INTDIR)" mkdir "$(INTDIR)"
	$(CC) @<<
$(CFLAGS) /Yc"CvGameCoreDLL.h" /Fp"$(PCH)" /Fo"$(INTDIR)\_precompile.obj" _precompile.cpp
<<

$(OBJS): $(INTDIR)\_precompile.obj

# Batch mode (::) compiles all out-of-date sources in a single cl.exe run.
{.}.cpp{$(INTDIR)}.obj::
	$(CC) @<<
$(CFLAGS) /Yu"CvGameCoreDLL.h" /Fp"$(PCH)" /Fo"$(INTDIR)\\" $<
<<

clean:
	@if exist "$(INTDIR)" rmdir /s /q "$(INTDIR)"
