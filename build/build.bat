:: run from developer command prompt
@cls
@echo off
set build_version=%~1
set arch=%~2
set user_mode=%~3
if "%build_version%"=="" set build_version="major"
if "%arch%"=="" set arch="x64"
if "%user_mode%"=="" set user_mode="true"


set vc2022p="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
set vc2022c="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
set vc2019p="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
set vc2019c="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
set vc2017p="C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
set vc2017c="C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
set vc2015p="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
set vc2013p="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
set vc2012p="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
set vc2010p="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"


if exist %vc2010p% (
	set vccmd=%vc2010p%
	set x32_flag=x86
	set x64_flag=x64
)
if exist %vc2012p% (
	set vccmd=%vc2012p%
	set x32_flag=x86_amd64
	set x64_flag=x64_amd64
)
if exist %vc2013p% (
	set vccmd=%vc2013p%
	set x32_flag=x86_amd64
	set x64_flag=x64_amd64
)
if exist %vc2015p% (
	set vccmd=%vc2015p%
	set x32_flag=x86_amd64
	set x64_flag=x64_amd64
)
if exist %vc2017c% (
	set vccmd=%vc2017c%
	set x32_flag=x86_amd64
	set x64_flag=x64_amd64
)
if exist %vc2017p% (
	set vccmd=%vc2017p%
	set x32_flag=x86_amd64
	set x64_flag=x64_amd64
)
if exist %vc2019c% (
	set vccmd=%vc2019c%
	set x32_flag=x86
	set x64_flag=x64
)
if exist %vc2019p% (
	set vccmd=%vc2019p%
	set x32_flag=x86
	set x64_flag=x64
)
if exist %vc2022c% (
	set vccmd=%vc2022c%
	set x32_flag=x86
	set x64_flag=x64
)
if exist %vc2022p% (
	set vccmd=%vc2022p%
	set x32_flag=x86
	set x64_flag=x64
)

if %vccmd%=="" (
	echo Cannot find a valid Visual Studio Environment"
	exit 0/B
)



if "%arch%"=="x32" (
	call %vccmd% %x32_flag%
) else (
	call %vccmd% %x64_flag%
)

::rd /S /Q ..\src\build\obj\%arch%
pushd ..\src
set BASE_NAME=WinFontInstall
echo "building %arch% bit version"
if "%user_mode%"=="true" (
	copy /y WinFontInstall.user.manifest WinFontInstall.manifest
) else (
	copy /y WinFontInstall.admin.manifest WinFontInstall.manifest
)
NMAKE /f "%BASE_NAME%.mak" ARCH=%arch% USER=%user_mode%
popd
