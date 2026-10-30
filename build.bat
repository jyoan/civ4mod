@echo off
rem Build CvGameCoreDLL.dll with MSVC 7.1 and install it as Assets\CvGameCoreDLL.dll
rem
rem   build.bat           incremental Release build, then copy to Assets\
rem   build.bat rebuild   clean + full Release build, then copy to Assets\
rem   build.bat clean     delete CvGameCoreDLL\Release\

setlocal
set "TOOLKIT=C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003"
set "PSDK=C:\Program Files\Microsoft Platform SDK"

rem Only the 2003-era toolchain may be visible. A newer cl.exe / link.exe on PATH,
rem or newer headers / libs in INCLUDE / LIB, would produce a DLL Civ4 cannot use.
set "PATH=%TOOLKIT%\bin;%PSDK%\Bin;%SystemRoot%\system32;%SystemRoot%"
set "INCLUDE="
set "LIB="

set "SRCDIR=%~dp0CvGameCoreDLL"
set "DLL=%SRCDIR%\Release\CvGameCoreDLL.dll"
set "ASSETS=%~dp0Assets"

cd /d "%SRCDIR%" || goto fail

if /i "%~1"=="clean" (
	nmake /nologo clean || goto fail
	goto done
)
if /i "%~1"=="rebuild" (
	nmake /nologo clean || goto fail
)
nmake /nologo || goto fail

rem Civ4 ships with the MSVC 7.1 runtime; reject a DLL linked against anything else.
"%TOOLKIT%\bin\link.exe" /dump /dependents "%DLL%" | findstr /i /c:"MSVCR71.dll" >nul || (
	echo ERROR: CvGameCoreDLL.dll does not depend on MSVCR71.dll
	goto fail
)
"%TOOLKIT%\bin\link.exe" /dump /dependents "%DLL%" | findstr /i /r /c:"MSVC[RP][89]" /c:"MSVC[RP]1[0-9]" /c:"VCRUNTIME" >nul && (
	echo ERROR: CvGameCoreDLL.dll depends on a newer MSVC runtime
	goto fail
)

if not exist "%ASSETS%" mkdir "%ASSETS%"
copy /y "%DLL%" "%ASSETS%\CvGameCoreDLL.dll" >nul || goto fail
echo.
echo Build succeeded: %ASSETS%\CvGameCoreDLL.dll

:done
endlocal
exit /b 0

:fail
echo.
echo BUILD FAILED
endlocal
exit /b 1
