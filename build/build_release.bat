@cls
@echo off && SETLOCAL 

set build_type=%~1
if "%build_type%"=="" set build_type=patch

set /p old_build_num=<ver.txt

for /F "tokens=1,2,3,4 delims=." %%a in ("%old_build_num%") do (
   set sMajor=%%a
   set sMinor=%%b
   set sPatch=%%c
   set sBuild=%%d
)

set /a Major=%sMajor%
set /a Minor=%sMinor%
set /a Patch=%sPatch%
set /a Build=%sBuild%
   
if %build_type%==major (
	set /a Major=%Major%+1
	set /a Minor=0
	set /a Patch=0
	set /a Build=0
)
if %build_type%==minor (
	set /a Minor=%Minor%+1
	set /a Patch=0
	set /a Build=0
)
if %build_type%==patch (
	set /a Patch=%Patch%+1
	set /a Build=0
)
if %build_type%==build (
	set /a Build=%Build%+1
)

echo Build Type: %build_type%
echo Previous Version: %sMajor%.%sMinor%.%sPatch%.%sBuild%
echo New Version Type: %Major%.%Minor%.%Patch%.%Build%

:: update our ver.txt file with new version
echo %Major%.%Minor%.%Patch%.%Build%>ver.txt


:: update our version.h file with new version
del /Q ..\src\version.new.h

type ..\src\version.h | findstr /v version: | findstr /v WINFONTINSTALLER_VERSION_FULL | findstr /v WINFONTINSTALLER_VERSION_STR | findstr /v #pragma > ..\src\version.new.h

del /Q ..\src\version.h
copy /Y ..\src\version.new.h ..\src\version.h 

echo // Full version: MUST be in the form of major,minor,revision,build >> ..\src\version.h 
echo #define WINFONTINSTALLER_VERSION_FULL %Major%,%Minor%,%Patch%,%Build% >> ..\src\version.h 

echo // String version: May be any suitable string >> ..\src\version.h 
echo #define WINFONTINSTALLER_VERSION_STR "%Major%.%Minor%.%Patch%.%Build%" >> ..\src\version.h 

echo // PE version: MUST be in the form of major.minor >> ..\src\version.h 
echo #pragma comment(linker, "/version:%Major%.%Minor%") >> ..\src\version.h 

del /Q ..\src\version.new.h


cmake -G "Visual Studio 17 2022" -A Win32 -S ../src -B "build32"
cmake -G "Visual Studio 17 2022" -A x64 -S ../src -B "build64"
cmake --build build32 --config Release
cmake --build build64 --config Release



mkdir ..\releases\release-%Major%.%Minor%.%Patch%
copy build32\Release\WinFontInstaller-x32.exe ..\releases\release-%Major%.%Minor%.%Patch%\
copy build64\Release\WinFontInstaller-x64.exe ..\releases\release-%Major%.%Minor%.%Patch%\

pushd ..\releases\release-%Major%.%Minor%.%Patch%\
for /f "usebackq delims=|" %%f in (`dir *.exe /b`) do (
 certutil -hashfile %%f > %%~nf.md5
)
popd

rd /S /Q build32
rd /S /Q build64

if not "%build_type%"=="build" (
 git add ver.txt
 git add ../src/version.h
 git commit -m "prepare for v%Major%.%Minor%.%Patch% release"
 git push
 git tag v%Major%.%Minor%.%Patch%
 git push --tags
)
