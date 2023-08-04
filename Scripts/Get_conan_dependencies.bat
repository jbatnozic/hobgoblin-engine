@ECHO OFF 

SET LIBZT_COMMITHASH=057bf33da19c37ec8aaf36607b222ff315db13a7
SET LIBZT_VERSION=3.0.0

SET ZTCPP_COMMITHASH=11dd5f6aa1d14f15c5070173edba779e35bff769
SET ZTCPP_VERSION=3.0.0

ECHO "Looking for git..."
where git
IF %ERRORLEVEL% EQU 0 GOTO foundgit
EXIT /b 1
:foundgit

ECHO "Looking for conan..."
where conan
IF %ERRORLEVEL% EQU 0 GOTO foundconan
EXIT /b 1
:foundconan

ECHO "All required tools found!"

MKDIR Build.Temp
CD Build.Temp

ECHO "Getting libzt..."
git clone https://github.com/jbatnozic/libzt-conan
CD libzt-conan\Conan2.x
git checkout %LIBZT_COMMITHASH%
conan export . --version %LIBZT_VERSION% --user jbatnozic --channel stable
IF %ERRORLEVEL% EQU 0 GOTO exportedlibzt
CD ..\..
EXIT /b 1
:exportedlibzt
CD ..\..

ECHO "Getting ZTCpp..."
git clone https://github.com/jbatnozic/ztcpp
CD ztcpp
git checkout %ZTCPP_COMMITHASH%
conan export . --version %ZTCPP_VERSION% --user jbatnozic --channel stable
IF %ERRORLEVEL% EQU 0 GOTO exportedztcpp
CD ..
EXIT /b 1
:exportedztcpp
CD ..

ECHO "All required Conan recipes exported!"

REM Clean up
CD ..
DEL /f /s /q Build.Temp 1>NUL
RMDIR /s /q Build.Temp
