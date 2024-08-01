#! /bin/bash

LIBZT_COMMITHASH=3ca14e77842b08cc9c45e58f56ffb6869c6b7ad3
LIBZT_VERSION=3.0.2

ZTCPP_COMMITHASH=bb4c8d25ba8dbcea783f5e742066ce943344f762
ZTCPP_VERSION=3.0.2

RMLUI_COMMITHASH=33dc5517c883edf9bf54f9339bf223333e8f79c4
RMLUI_VERSION=5.1

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

echo "Getting RmlUi..."
git clone https://github.com/jbatnozic/rmlui-conan
pushd rmlui-conan
git checkout ${RMLUI_COMMITHASH}
conan export . --version ${RMLUI_VERSION} --user jbatnozic --channel stable
# TODO: exit on failure of conan export
popd

echo "All required Conan recipes exported!"

# Clean up
popd
rm -rf Build.Temp
