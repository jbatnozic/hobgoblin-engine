@ECHO OFF 

SET LIBZT_COMMITHASH=5eb65b8aab761b6937be64736eff310cfa5d51e5
SET LIBZT_VERSION=3.0.1

SET ZTCPP_COMMITHASH=a7031bf268ac008f997aced92e13450f7297db3e
SET ZTCPP_VERSION=3.0.1

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
