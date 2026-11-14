@echo off
rem Build CvGameCoreDLL.dll with MSVC 7.1, install it as Assets\CvGameCoreDLL.dll,
rem then deploy the mod (claude_sample.ini + Assets\) to the Civ4 Mods folder.
rem
rem   build.bat           incremental Release build, then deploy
rem   build.bat rebuild   clean + full Release build, then deploy
rem   build.bat deploy    deploy only (no build)
rem   build.bat clean     delete CvGameCoreDLL\Release\
rem
rem Exit code: 0 = done, 1 = build or deploy failed,
rem            2 = built but not deployed because Civ4 is running (quit it, then "build.bat deploy")
rem
rem Mods\claude_sample\Assets\ is mirrored from Assets\: files that are not in
rem Assets\ are deleted there. Edit files in this repository, not in Mods\.

setlocal
set "TOOLKIT=C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003"
set "PSDK=C:\Program Files\Microsoft Platform SDK"
set "BTS=C:\Program Files (x86)\CYBERFRONT\Sid Meier's Civilization 4(J)\Beyond the Sword(J)"
set "MODNAME=claude_sample"
set "CIV4EXE=Civ4BeyondSword.exe"

rem Only the 2003-era toolchain may be visible. A newer cl.exe / link.exe on PATH,
rem or newer headers / libs in INCLUDE / LIB, would produce a DLL Civ4 cannot use.
set "PATH=%TOOLKIT%\bin;%PSDK%\Bin;%SystemRoot%\system32;%SystemRoot%"
set "INCLUDE="
set "LIB="

set "SRCDIR=%~dp0CvGameCoreDLL"
set "DLL=%SRCDIR%\Release\CvGameCoreDLL.dll"
set "ASSETS=%~dp0Assets"
set "MODDIR=%BTS%\Mods\%MODNAME%"

cd /d "%SRCDIR%" || goto fail

if /i "%~1"=="clean" (
	nmake /nologo clean || goto fail
	goto done
)
if /i "%~1"=="deploy" goto deploy
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

rem Check the names the XML and Python refer to, and the format of the sources.
rem The errors are reported but do not stop the deploy.
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\check_refs.ps1" -Bts "%BTS%" || echo WARNING: tools\check_refs.ps1 found errors, see above. Deploying anyway.

:deploy
if not exist "%BTS%\Mods" goto nodeploy
rem A running Civ4 keeps CvGameCoreDLL.dll open, so do not even try to copy.
tasklist /NH /FI "IMAGENAME eq %CIV4EXE%" | findstr /i /c:"%CIV4EXE%" >nul && goto civ4running
rem /R:2 /W:1 fails fast instead of retrying for a long time if a file is locked anyway.
robocopy "%ASSETS%" "%MODDIR%\Assets" /MIR /R:2 /W:1 /NJH /NJS /NP /NDL
if errorlevel 8 goto deployfail
copy /y "%~dp0%MODNAME%.ini" "%MODDIR%\%MODNAME%.ini" >nul || goto deployfail
echo Deployed to:     %MODDIR%
goto done

:nodeploy
echo WARNING: Civ4 Mods folder not found; skipped deploy
goto done

:civ4running
echo.
echo SKIPPED DEPLOY: Civ4 is running. Quit Civ4, then run "build.bat deploy".
endlocal
exit /b 2

:deployfail
echo.
echo DEPLOY FAILED: could not update the Mods folder. Quit Civ4, then run "build.bat deploy".
endlocal
exit /b 1

:done
endlocal
exit /b 0

:fail
echo.
echo BUILD FAILED
endlocal
exit /b 1
