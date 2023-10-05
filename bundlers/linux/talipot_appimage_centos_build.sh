#!/bin/bash

# this script should only be run in a CentOS Stream 8.x docker image

cd

# clean packages cache
yum -y clean all
yum -y clean expire-cache
yum -y update

# install base build system
yum -y install epel-release
yum -y install xz tar gzip make wget ccache


# add extra CentOS 8 repositories to get some build dependencies
yum -y install dnf-plugins-core
yum config-manager --set-enabled powertools
yum -y install https://pkgs.dyn.su/el8/base/x86_64/raven-release-1.0-2.el8.noarch.rpm
yum -y install cmake


# install talipot deps
yum -y install zlib-devel libzstd-devel qhull-devel yajl-devel \
  graphviz-devel libgit2-devel binutils-devel
yum -y install freetype-devel fontconfig-devel glew-devel fribidi-devel
yum -y install qt5-qtbase-devel qt5-qtimageformats qt5-qtsvg \
  quazip-qt5-devel qt5-qtwebkit-devel --enablerepo=epel-testing --nobest
yum -y install openssl3-devel


# install Python 3, Sphinx and SIP
yum -y install python3.11-devel python3.11-pip
pip3.11 install sphinx sip

# needed for generating the AppImage
yum -y install fuse fuse-libs file

# needed for generating the doc
yum -y install doxygen graphviz

# needed to build and run tests
yum -y install cppunit-devel xorg-x11-server-Xvfb

# build and install talipot
if [ -d /talipot/build ]; then
  rm -rf /talipot/build
fi
mkdir /talipot/build
cd /talipot/build

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DPython3_EXECUTABLE=/usr/bin/python3.11 \
      -DTALIPOT_USE_CCACHE=ON \
      -DTALIPOT_BUILD_FOR_APPIMAGE=ON \
      -DTALIPOT_BUILD_TESTS=ON \
      -DOpenMP_C_FLAGS=-fopenmp \
      -DOpenMP_CXX_FLAGS=-fopenmp ..

xvfb-run make -j4 install

# run unit tests
xvfb-run make tests

# build a bundle dir suitable for AppImageKit
bash bundlers/linux/make_appimage_bundle.sh --appdir $PWD

# get appimagetool
wget "https://github.com/probonopd/AppImageKit/releases/download/\
continuous/appimagetool-$(uname -p).AppImage"
chmod a+x appimagetool-$(uname -p).AppImage

# finally build the portable app
TALIPOT_APPIMAGE=Talipot-$(sh talipot-config --version)-$(uname -p).AppImage
./appimagetool-$(uname -p).AppImage --appimage-extract-and-run Talipot.AppDir $TALIPOT_APPIMAGE
chmod +x $TALIPOT_APPIMAGE

if [ -d /talipot_host_build ]; then
  cp $TALIPOT_APPIMAGE /talipot_host_build/
fi
