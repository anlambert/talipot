#!/bin/bash

# this script should only be run in a centos:[7|8] docker image

centos7=true
centos8=false
if grep -q "CentOS Linux release 8" /etc/centos-release
then
  centos7=false
  centos8=true
fi

cd

if [ "$centos8" = true ]
then
  # Centos 8 reached EOL so we need to update the package repository URLs
  sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-Linux-*
  sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' \
    /etc/yum.repos.d/CentOS-Linux-*
  yum -y upgrade
fi

# clean packages cache
yum -y clean all
yum -y clean expire-cache
yum -y update

# install base build system
yum -y install epel-release
yum -y install xz tar gzip make wget ccache

if [ "$centos7" = true ]
then
  # install GCC 8 on CentOS 7
  yum -y install centos-release-scl
  yum -y install devtoolset-8-gcc devtoolset-8-gcc-c++
  yum -y install cmake3
  # needed for qt5 gtk3 platform theme
  yum -y install gtk3-devel
else
  # add extra CentOS 8 repositories to get some build dependencies
  yum -y install dnf-plugins-core
  yum config-manager --set-enabled powertools
  yum -y install https://pkgs.dyn.su/el8/base/x86_64/raven-release-1.0-2.el8.noarch.rpm
  yum -y install cmake
fi


# install talipot deps
yum -y install zlib-devel libzstd-devel qhull-devel yajl-devel \
  graphviz-devel libgit2-devel binutils-devel
yum -y install freetype-devel fontconfig-devel glew-devel fribidi-devel
yum -y install qt5-qtbase-devel qt5-qtimageformats qt5-qtsvg \
  quazip-qt5-devel qt5-qtwebkit-devel --enablerepo=epel-testing

# install recent Python
yum -y groupinstall "Development Tools"
yum -y install openssl-devel libffi-devel bzip2-devel

if [ "$centos7" = true ]
then
  wget https://www.python.org/ftp/python/3.9.9/Python-3.9.9.tgz
  tar xzf Python-3.9.9.tgz
  cd Python-3.9.9
  PYTHON_VERSION=3.9
else
  wget https://www.python.org/ftp/python/3.10.0/Python-3.10.0.tgz
  tar xzf Python-3.10.0.tgz
  cd Python-3.10.0
  PYTHON_VERSION=3.10
fi

./configure --enable-optimizations \
  --with-system-ffi --with-computed-gotos \
  --enable-loadable-sqlite-extensions \
  --enable-shared LDFLAGS="-Wl,-rpath /usr/local/lib" \
  CC="ccache gcc"
make -j4 altinstall
cd ..

# needed for generating the AppImage
yum -y install fuse fuse-libs file

# needed for generating the doc
yum -y install doxygen graphviz
pip$PYTHON_VERSION install sphinx

# needed to build and run tests
yum -y install cppunit-devel xorg-x11-server-Xvfb

# build and install talipot
if [ -d /talipot/build ]; then
  rm -rf /talipot/build
fi
mkdir /talipot/build
cd /talipot/build

if [ "$centos7" = true ]
then
  cmake3 -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_C_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gcc \
         -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/g++ \
         -DCMAKE_INSTALL_PREFIX=$PWD/install \
         -DPYTHON_EXECUTABLE=/usr/local/bin/python$PYTHON_VERSION \
         -DTALIPOT_USE_CCACHE=ON \
         -DTALIPOT_BUILD_FOR_APPIMAGE=ON \
         -DTALIPOT_BUILD_TESTS=ON \
         -DOpenMP_C_FLAGS=-fopenmp \
         -DOpenMP_CXX_FLAGS=-fopenmp ..
else
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$PWD/install \
        -DPYTHON_EXECUTABLE=/usr/local/bin/python$PYTHON_VERSION \
        -DTALIPOT_USE_CCACHE=ON \
        -DTALIPOT_BUILD_FOR_APPIMAGE=ON \
        -DTALIPOT_BUILD_TESTS=ON \
        -DOpenMP_C_FLAGS=-fopenmp \
        -DOpenMP_CXX_FLAGS=-fopenmp ..
fi

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
TALIPOT_APPIMAGE=Talipot-$(sh talipot-config --version)-$(uname -p)-qt$(qmake-qt5 -query QT_VERSION).AppImage
./appimagetool-$(uname -p).AppImage --appimage-extract-and-run Talipot.AppDir $TALIPOT_APPIMAGE
chmod +x $TALIPOT_APPIMAGE

if [ -d /talipot_host_build ]; then
  cp $TALIPOT_APPIMAGE /talipot_host_build/
fi
