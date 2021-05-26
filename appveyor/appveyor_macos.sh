#!/bin/bash

let TALIPOT_BUILD_CORE_ONLY=APPVEYOR_JOB_NUMBER%2
echo TALIPOT_BUILD_CORE_ONLY=$TALIPOT_BUILD_CORE_ONLY

set -e

# install MacPorts
curl -LO https://raw.githubusercontent.com/GiovanniBussi/macports-ci/master/macports-ci
source ./macports-ci install
source ./macports-ci ccache

CLANG_VERSION=11
PYTHON_VERSION=3.9
PYTHON_VERSION_NO_DOT=$(echo $PYTHON_VERSION | sed 's/\.//')

# install Talipot core build dependencies
sudo port -N install \
  cmake \
  clang-${CLANG_VERSION} \
  ccache \
  zlib \
  qhull \
  yajl \
  zstd \
  graphviz \
  libgit2 \
  python${PYTHON_VERSION_NO_DOT} \
  pkgconfig \
  cppunit

export TALIPOT_BUILD_DOC=OFF

# install Talipot complete build dependencies
if [[ "$TALIPOT_BUILD_CORE_ONLY" == "0" ]]
then
  sudo port -N install \
    freetype \
    fontconfig \
    fribidi \
    glew \
    qt5-qtbase \
    qt5-qttools \
    qt5-qtwebkit \
    quazip \
    py${PYTHON_VERSION_NO_DOT}-pip
  pip-${PYTHON_VERSION} install sphinx
  export TALIPOT_BUILD_DOC=ON
fi

source ./macports-ci ccache --save

cd $APPVEYOR_BUILD_FOLDER

# create build directory
mkdir build && cd build

# configure Talipot core build using cmake
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PWD/install \
  -DCMAKE_C_COMPILER=/opt/local/bin/clang-mp-${CLANG_VERSION} \
  -DCMAKE_CXX_COMPILER=/opt/local/bin/clang++-mp-${CLANG_VERSION} \
  -DPYTHON_EXECUTABLE=/opt/local/bin/python${PYTHON_VERSION} \
  -DTALIPOT_BUILD_TESTS=ON \
  -DTALIPOT_USE_CCACHE=ON \
  -DTALIPOT_BUILD_DOC=$TALIPOT_BUILD_DOC \
  -DTALIPOT_BUILD_CORE_ONLY=$TALIPOT_BUILD_CORE_ONLY

# build Talipot
make -j4

# install Talipot
make -j4 install

# execute Talipot unit tests
make tests

# create Talipot bundle
if [[ "$TALIPOT_BUILD_CORE_ONLY" == "0" ]]
then
  make bundle
fi
