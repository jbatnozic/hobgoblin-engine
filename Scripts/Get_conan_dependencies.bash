#! /bin/bash

LIBZT_COMMITHASH=5eb65b8aab761b6937be64736eff310cfa5d51e5
LIBZT_VERSION=3.0.1

ZTCPP_COMMITHASH=a7031bf268ac008f997aced92e13450f7297db3e
ZTCPP_VERSION=3.0.1

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

echo "Getting libzt..."
git clone https://github.com/jbatnozic/libzt-conan
pushd libzt-conan/Conan2.x
git checkout ${LIBZT_COMMITHASH}
conan export . --version ${LIBZT_VERSION} --user jbatnozic --channel stable
# TODO: exit on failure of conan export
popd

echo "Getting ZTCpp..."
git clone https://github.com/jbatnozic/ztcpp
pushd ztcpp
git checkout ${ZTCPP_COMMITHASH}
conan export . --version ${ZTCPP_VERSION} --user jbatnozic --channel stable
# TODO: exit on failure of conan export
popd

echo "All required Conan recipes exported!"

# Clean up
popd
rm -rf Build.Temp
