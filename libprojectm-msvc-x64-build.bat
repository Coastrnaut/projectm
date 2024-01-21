REM @echo off

Echo LIB Windows Build NuGet

REM # Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set INITDIR=%_SCRIPT_FOLDER%
set SRC=%INITDIR%\
set BUILDTREE=%SRC%\build-win\
SET tbs_arch=x64
SET vcvar_arg=x86_amd64
SET cmake_platform="Visual Studio 17 2022"
SET cmake_arch="x64"

REM # VC Vars #
SET VCVAR="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%
SET VCVAR="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%

REM # Clean Build Tree #
rd /s /q %BUILDTREE%
mkdir %BUILDTREE%
mkdir %BUILDTREE%\deps

REM # Change to Build Tree drive #
%_SCRIPT_DRIVE%
REM # Change to Build Tree directory #
cd %BUILDTREE%

set BINDIR=%SRC%\build-bin\
rd /s /q %BINDIR%
mkdir %BINDIR%
echo %BINDIR%

ECHO %cmake_platform%

rd /s /q %BUILDTREE%
mkdir %BUILDTREE%\projectm
cd %BUILDTREE%\projectm
cmake -G %cmake_platform% ^
-A %cmake_arch% ^
-DBUILD_SHARED_LIBS:BOOL=OFF ^
-DCMAKE_TOOLCHAIN_FILE=D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake ^
-DBUILD_TESTING:BOOL=OFF ^
-DCMAKE_CXX_FLAGS_RELEASE="/P7" ^
-DCMAKE_CXX_FLAGS_DEBUG="/P7" ^
-DCMAKE_C_FLAGS_RELEASE="/P7" ^
-DCMAKE_C_FLAGS_DEBUG="/P7" ^
-DCMAKE_INSTALL_PREFIX=%BINDIR%\ ^
-DCMAKE_BUILD_TYPE="Debug" %SRC%
cmake --build . --config Debug --target install


cd %INITDIR%
REM --- exit ----
GOTO:eof
