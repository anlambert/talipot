#!/bin/bash

# this script should only be run in a CentOS Stream [8|9].x docker image

centos9=false
centos10=true
if grep -q "CentOS Stream release 9" /etc/centos-release
then
  centos9=true
  centos10=false
fi

cd

# clean packages cache
yum -y clean all
yum -y clean expire-cache
yum -y update

# install base build system
yum -y install epel-release
yum -y install xz tar gzip make wget ccache git
yum -y install dnf-plugins-core

if [ "$centos9" = true ]
then
  # add extra CentOS 9 repository to get quazip-qt5 dependency
  yum -y install https://pkgs.sysadmins.ws/el9/base/x86_64/raven-release.el9.noarch.rpm
  talipot_use_qt6=OFF
  qmake=qmake-qt5
else
  # add extra CentOS 10 repository to get yajl-devel dependency
  yum -y install https://repo.almalinux.org/almalinux/10/BaseOS/x86_64/os/Packages/almalinux-gpg-keys-10.1-16.el10.x86_64.rpm
  yum -y install https://repo.almalinux.org/almalinux/10/devel/x86_64/os/Packages/almalinux-release-devel-10-1.el10.x86_64.rpm
  talipot_use_qt6=ON
  qmake=qmake
fi

# add extra CentOS 9/10 repositories to get some build dependencies
yum config-manager --set-enabled crb

yum -y install cmake

# install talipot deps
yum -y install zlib-devel libzstd-devel qhull-devel yajl-devel \
  graphviz-devel libgit2-devel binutils-devel
yum -y install freetype-devel fontconfig-devel glew-devel fribidi-devel

if [ "$centos9" = true ]
then
  yum -y install qt5-qtbase-devel qt5-qtimageformats qt5-qtsvg \
    quazip-qt5-devel --enablerepo=epel-testing --nobest
else
  yum -y install qt6-qtbase-devel qt6-qtimageformats qt6-qtsvg \
    qt6-qt5compat-devel
fi

# install Python 3, Sphinx and SIP
yum -y install python3.13-devel python3.13-pip
pip3.13 install sphinx 'sip != 6.13.0'

# needed for generating the AppImage
yum -y install fuse fuse-libs file

# needed for generating the doc
yum -y install doxygen graphviz

# install recent GCC
yum -y install gcc-toolset-15

# needed to build and run tests
yum -y install cppunit-devel xorg-x11-server-Xvfb

# build and install talipot
if [ -d /talipot/build ]; then
  rm -rf /talipot/build
fi
mkdir /talipot/build
cd /talipot/build

# ensure talipot version can be fetched from git when
# source folder on host is mounted as a docker volume
git config --global --add safe.directory /talipot

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=/opt/rh/gcc-toolset-15/root/usr/bin/gcc \
      -DCMAKE_CXX_COMPILER=/opt/rh/gcc-toolset-15/root/usr/bin/g++ \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DPython3_EXECUTABLE=/usr/bin/python3.13 \
      -DTALIPOT_USE_CCACHE=ON \
      -DTALIPOT_BUILD_FOR_APPIMAGE=ON \
      -DTALIPOT_BUILD_TESTS=ON \
      -DTALIPOT_USE_QT6=$talipot_use_qt6 \
      -DOpenMP_C_FLAGS=-fopenmp \
      -DOpenMP_CXX_FLAGS=-fopenmp ..

xvfb-run make -j$(nproc) install

# run unit tests
xvfb-run make tests

# build a bundle dir suitable for AppImageKit
bash bundlers/linux/make_appimage_bundle.sh --appdir $PWD

APP_DIR=Talipot.AppDir

# get appimagetool
wget "https://github.com/AppImage/appimagetool/releases/download/\
continuous/appimagetool-x86_64.AppImage"
chmod a+x appimagetool-x86_64.AppImage

# finally build the portable app
TALIPOT_APPIMAGE=Talipot-$(sh talipot-config --version)-x86_64-qt$($qmake -query QT_VERSION).AppImage
./appimagetool-x86_64.AppImage $APP_DIR $TALIPOT_APPIMAGE
chmod +x $TALIPOT_APPIMAGE

if [ -d /talipot_host_build ]; then
  cp $TALIPOT_APPIMAGE /talipot_host_build/
fi
