@cls
@echo off && SETLOCAL 

set build_type=%~1
if "%build_type%"=="" set build_type=build

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
   
if %build_type%==major set /a Major=%Major%+1
if %build_type%==patch set /a Minor=%Minor%+1
if %build_type%==minor set /a Patch=%Patch%+1
if %build_type%==build set /a Build=%Build%+1

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

call build.bat "%Major%.%Minor%.%Patch%" "x32" "false"
call build.bat "%Major%.%Minor%.%Patch%" "x32" "true"
call build.bat "%Major%.%Minor%.%Patch%" "x64" "false"
call build.bat "%Major%.%Minor%.%Patch%" "x64" "true"

mkdir ..\releases\release-%build_version%
copy ..\src\build\bin\*.* ..\releases\release-%build_version%\

pushd ..\releases\release-%build_version%\
for /f "usebackq delims=|" %%f in (`dir *.exe /b`) do (
 certutil -hashfile %%f > %%~nf.md5
)
popd

if not "%build_type%"=="build" (
 git add ver.txt
 git add ../src/version.h
 git commit -m "prepare for v%Major%.%Minor%.%Patch% release"
 git tag v%Major%.%Minor%.%Patch%
 git push --tags
)