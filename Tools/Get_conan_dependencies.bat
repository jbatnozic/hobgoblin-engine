@ECHO OFF 

SET SFML_COMMITHASH=8ec114832c0069125f51a87bd5c5adf4632742e5
SET SFML_VERSION=2.5.1

SET LIBZT_COMMITHASH=03034d93fdd9450257013e699103431e2e68ba18
SET LIBZT_VERSION=2.1.0

SET ZTCPP_COMMITHASH=24123dec64640e6eca965aa5cdc593edca313cd6
SET ZTCPP_VERSION=2.1.0

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

ECHO "Getting SFML..."
git clone https://github.com/bincrafters/conan-sfml.git
CD conan-sfml
git checkout %SFML_COMMITHASH%
conan export . %SFML_VERSION%@jbatnozic/stable
CD ..

ECHO "Getting libzt..."
git clone https://github.com/jbatnozic/libzt-conan
CD libzt-conan
git checkout %LIBZT_COMMITHASH%
conan export . %LIBZT_VERSION%@jbatnozic/stable
CD ..

ECHO "Getting ZTCpp..."
git clone https://github.com/jbatnozic/ztcpp
CD ztcpp
git checkout %ZTCPP_COMMITHASH%
conan export . %ZTCPP_VERSION%@jbatnozic/stable
CD ..

ECHO "All required Conan recipes exported!"

REM Clean up
CD ..
DEL /f /s /q Build.Temp 1>NUL
RMDIR /s /q Build.Temp