#! /bin/bash

SFML_COMMITHASH=8ec114832c0069125f51a87bd5c5adf4632742e5
SFML_VERSION=2.5.1

LIBZT_COMMITHASH=03034d93fdd9450257013e699103431e2e68ba18
LIBZT_VERSION=2.1.0

ZTCPP_COMMITHASH=24123dec64640e6eca965aa5cdc593edca313cd6
ZTCPP_VERSION=2.1.0

echo "Looking for git..."
if ! command -v git &> /dev/null
then
    echo "git could not be found!"
    exit
fi

echo "Looking for conan..."
if ! command -v git &> /dev/null
then
    echo "conan could not be found!"
    exit
fi

echo "All required tools found!"

mkdir Build.Temp
pushd Build.Temp

echo "Getting SFML..."
git clone https://github.com/bincrafters/conan-sfml.git
pushd conan-sfml
git checkout ${SFML_COMMITHASH}
conan export . ${SFML_VERSION}@jbatnozic/stable
popd

echo "Getting libzt..."
git clone https://github.com/jbatnozic/libzt-conan
pushd libzt-conan
git checkout ${LIBZT_COMMITHASH}
conan export . ${LIBZT_VERSION}@jbatnozic/stable
popd

echo "Getting ZTCpp..."
git clone https://github.com/jbatnozic/ztcpp
pushd ztcpp
git checkout ${ZTCPP_COMMITHASH}
conan export . ${ZTCPP_VERSION}@jbatnozic/stable
popd

echo "All required Conan recipes exported!"

# Clean up
popd
rm -rf Build.Temp
