#!/bin/bash

cd

# clean packages cache
yum -y clean all
yum -y clean expire-cache
yum -y update

# install base build system
yum -y install epel-release
yum -y install xz cmake3 tar gzip make wget ccache

# install GCC 8
yum -y install centos-release-scl
yum -y install devtoolset-8-gcc devtoolset-8-gcc-c++

# install talipot deps
yum -y install zlib-devel qhull-devel yajl-devel binutils-devel
yum -y install freetype-devel glew-devel
yum -y install python36-devel python36-pip
yum -y install qt5-qtbase-devel qt5-qtimageformats qt5-qtsvg quazip-qt5-devel qt5-qtwebkit-devel

# needed for qt5 gtk3 platform theme
yum -y install gtk3-devel

# needed for generating the AppImage
yum -y install fuse fuse-libs file

# needed for generating the doc
yum -y install doxygen graphviz
pip3.6 install sphinx

# needed to build tests
yum -y install cppunit-devel

# build and install talipot
if [ -d /talipot/build ]; then
  rm -rf /talipot/build
fi
mkdir /talipot/build
cd /talipot/build
if [ "$1" == "NO_CCACHE" ]; then
  CCACHE=OFF
else
  CCACHE=ON
fi
if [ "$2" == "RUN_TESTS" ]; then
  RUN_TESTS=ON
else
  RUN_TESTS=OFF
fi

cmake3 -DCMAKE_BUILD_TYPE=Release \
       -DCMAKE_C_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gcc \
       -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/g++ \
       -DCMAKE_INSTALL_PREFIX=$PWD/install \
       -DPYTHON_EXECUTABLE=/usr/bin/python3.6 \
       -DTALIPOT_USE_CCACHE=$CCACHE \
       -DTALIPOT_BUILD_FOR_APPIMAGE=ON \
       -DTALIPOT_BUILD_TESTS=$RUN_TESTS \
       -DOpenMP_C_FLAGS=-fopenmp \
       -DOpenMP_CXX_FLAGS=-fopenmp ..

make -j4 install

# run unit tests
if [ "$2" == "RUN_TESTS" ]; then
  make runTests
fi

# build a bundle dir suitable for AppImageKit
sh bundlers/linux/make_appimage_bundle.sh --appdir $PWD

# get appimagetool
wget "https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-$(uname -p).AppImage"
chmod a+x appimagetool-$(uname -p).AppImage

# finally build the portable app
TALIPOT_APPIMAGE=Talipot-$(sh talipot-config --version)-$(uname -p).AppImage
./appimagetool-$(uname -p).AppImage Talipot.AppDir Talipot-$(sh talipot-config --version)-$(uname -p).AppImage $TALIPOT_APPIMAGE
chmod +x $TALIPOT_APPIMAGE