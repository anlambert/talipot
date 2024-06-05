#!/bin/bash

# this script should only be run in a CentOS Stream [8|9].x docker image

centos8=false
centos9=true
if grep -q "CentOS Stream release 8" /etc/centos-release
then
  centos8=true
  centos9=false
  # CentOS 8 packages have been moved to the vault
  sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*
  sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' \
    /etc/yum.repos.d/CentOS-*
fi

cd

# clean packages cache
yum -y clean all
yum -y clean expire-cache
yum -y update

# install base build system
yum -y install epel-release
yum -y install xz tar gzip make wget ccache
yum -y install dnf-plugins-core

if [ "$centos8" = true ]
then
  # add extra CentOS 8 repositories to get some build dependencies
  yum config-manager --set-enabled powertools
  yum -y install https://pkgs.dyn.su/el8/base/x86_64/raven-release-1.0-2.el8.noarch.rpm
  talipot_use_qt6=OFF
  qmake=qmake-qt5
else
  # add extra CentOS 9 repositories to get some build dependencies
  yum config-manager --set-enabled crb
  talipot_use_qt6=ON
  qmake=qmake
fi

yum -y install cmake

# install talipot deps
yum -y install zlib-devel libzstd-devel qhull-devel yajl-devel \
  graphviz-devel libgit2-devel binutils-devel
yum -y install freetype-devel fontconfig-devel glew-devel fribidi-devel

if [ "$centos8" = true ]
then
  yum -y install qt5-qtbase-devel qt5-qtimageformats qt5-qtsvg \
    quazip-qt5-devel qt5-qtwebkit-devel --enablerepo=epel-testing --nobest
else
  yum -y install qt6-qtbase-devel qt6-qtimageformats qt6-qtsvg \
    qt6-qt5compat-devel qt6-qtwebengine-devel
fi

# install Python 3, Sphinx and SIP
yum -y install python3.11-devel python3.11-pip
pip3.11 install sphinx sip

# needed for generating the AppImage
yum -y install fuse fuse-libs file

# needed for generating the doc
yum -y install doxygen graphviz

# install recent GCC
yum -y install gcc-toolset-13

# needed to build and run tests
yum -y install cppunit-devel xorg-x11-server-Xvfb

# build and install talipot
if [ -d /talipot/build ]; then
  rm -rf /talipot/build
fi
mkdir /talipot/build
cd /talipot/build

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=/opt/rh/gcc-toolset-13/root/bin/gcc \
      -DCMAKE_CXX_COMPILER=/opt/rh/gcc-toolset-13/root/bin/g++ \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DPython3_EXECUTABLE=/usr/bin/python3.11 \
      -DTALIPOT_USE_CCACHE=ON \
      -DTALIPOT_BUILD_FOR_APPIMAGE=ON \
      -DTALIPOT_BUILD_TESTS=ON \
      -DTALIPOT_USE_QT6=$talipot_use_qt6 \
      -DOpenMP_C_FLAGS=-fopenmp \
      -DOpenMP_CXX_FLAGS=-fopenmp ..

xvfb-run make -j4 install

# run unit tests
xvfb-run make tests

# build a bundle dir suitable for AppImageKit
bash bundlers/linux/make_appimage_bundle.sh --appdir $PWD

APP_DIR=Talipot.AppDir

# ensure QtWebEngine is functional when bundled in AppImage
if [ "$centos9" = true ]
then
  yum -y install patchelf cpio
  # for some reasons, qt6-qtwebengine translations files are not installed
  # by yum but those are still available in the rpm archive so we hack a bit
  # to extract and copy them in the AppImage AppDir
  yum -y remove --noautoremove qt6-qtwebengine
  yum -y install --downloadonly --downloaddir=$PWD qt6-qtwebengine
  rpm2cpio qt6-qtwebengine*.rpm | cpio -idmv --directory=/opt/qtwebengine
  cp -r /opt/qtwebengine/usr/share/qt6/translations/qtwebengine_locales/ $APP_DIR/usr/translations/
  # this file is also required to be bundled in AppImage or V8 crashes on startup
  cp /opt/qtwebengine/usr/share/qt6/resources/v8_context_snapshot.bin $APP_DIR/usr/resources/
  # rpath of QtWebEngineProcess also needs to be patched to work in AppImage
  patchelf --set-rpath '$ORIGIN/../lib' $APP_DIR/usr/libexec/QtWebEngineProcess
fi

# get appimagetool
wget "https://github.com/probonopd/AppImageKit/releases/download/\
continuous/appimagetool-$(uname -p).AppImage"
chmod a+x appimagetool-$(uname -p).AppImage

# finally build the portable app
TALIPOT_APPIMAGE=Talipot-$(sh talipot-config --version)-$(uname -p)-qt$($qmake -query QT_VERSION).AppImage
./appimagetool-$(uname -p).AppImage $APP_DIR $TALIPOT_APPIMAGE
chmod +x $TALIPOT_APPIMAGE

if [ -d /talipot_host_build ]; then
  cp $TALIPOT_APPIMAGE /talipot_host_build/
fi
