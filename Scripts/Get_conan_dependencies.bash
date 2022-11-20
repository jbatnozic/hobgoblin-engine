#! /bin/bash

SFML_COMMITHASH=8ec114832c0069125f51a87bd5c5adf4632742e5
SFML_VERSION=2.5.1

LIBZT_COMMITHASH=e083e4daaf78d0c7a72d8ba009a5d35359769c5d
LIBZT_VERSION=2.2.0

ZTCPP_COMMITHASH=af8f1bda92fcc121d36ab16d68676f461fc24404
ZTCPP_VERSION=2.2.0

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
