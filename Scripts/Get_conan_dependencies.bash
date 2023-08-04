#! /bin/bash

LIBZT_COMMITHASH=057bf33da19c37ec8aaf36607b222ff315db13a7
LIBZT_VERSION=3.0.0

ZTCPP_COMMITHASH=11dd5f6aa1d14f15c5070173edba779e35bff769
ZTCPP_VERSION=.0.0

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
