#!/bin/bash

# Script to build and upload Talipot Python wheels for MacOS on AppVeyor

# get last uploaded dev wheel version
JSON=$(curl -s 'https://test.pypi.org/pypi/talipot/json')
LAST_VERSION=$(echo $JSON | python -c "
import sys, json
print(json.load(sys.stdin)['info']['version'])" 2>/dev/null)

if [ $? -ne 0 ]
then
  DEV_VERSION=1
else
  echo last wheel dev version = $LAST_VERSION

  # check if dev wheel version needs to be incremented
  VERSION_INCREMENT=$(echo $JSON | python -c "
import sys, json
releases = json.load(sys.stdin)['releases']['$LAST_VERSION']
print(any(['macosx' in r['filename'] for r in releases]))")
  DEV_VERSION=$(echo $LAST_VERSION | cut -f4 -d '.' | sed 's/dev//')

  if [ "$VERSION_INCREMENT" == "True" ]
  then
    let DEV_VERSION+=1
  fi
fi
echo current wheel dev version = $DEV_VERSION

# abort script on first error
set -e

# install MacPorts
curl -LO https://raw.githubusercontent.com/GiovanniBussi/macports-ci/master/macports-ci
source ./macports-ci install
source ./macports-ci ccache

CLANG_VERSION=13

# install build tools and dependencies
sudo port -N install \
  cmake \
  clang-${CLANG_VERSION} \
  ccache \
  qhull \
  yajl \
  zlib \
  zstd \
  graphviz \
  libgit2

  source ./macports-ci ccache --save

# install Python versions from which to build wheels against
curl -LO https://www.python.org/ftp/python/3.7.6/python-3.7.6-macosx10.9.pkg
sudo installer -pkg python-3.7.6-macosx10.9.pkg -target /
sudo /Library/Frameworks/Python.framework/Versions/3.7/bin/pip3 install wheel
curl -LO https://www.python.org/ftp/python/3.8.1/python-3.8.1-macosx10.9.pkg
sudo installer -pkg python-3.8.1-macosx10.9.pkg -target /
sudo /Library/Frameworks/Python.framework/Versions/3.8/bin/pip3 install wheel
curl -LO https://www.python.org/ftp/python/3.9.0/python-3.9.0-macosx10.9.pkg
sudo installer -pkg python-3.9.0-macosx10.9.pkg -target /
curl -LO https://www.python.org/ftp/python/3.10.0/python-3.10.0-macosx10.9.pkg
sudo installer -pkg python-3.10.0-macosx10.9.pkg -target /
sudo /Library/Frameworks/Python.framework/Versions/3.10/bin/pip3 install wheel
sudo /Library/Frameworks/Python.framework/Versions/3.10/bin/pip3 install twine

# configure and build wheels for all supported Python versions
cd $APPVEYOR_BUILD_FOLDER
mkdir build && cd build

for py3Version in 3.7 3.8 3.9 3.10
do
  cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$PWD/install \
    -DCMAKE_C_COMPILER=/opt/local/bin/clang-mp-${CLANG_VERSION} \
    -DCMAKE_CXX_COMPILER=/opt/local/bin/clang++-mp-${CLANG_VERSION} \
    -DTALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET=ON \
    -DTALIPOT_PYTHON_TEST_WHEEL_SUFFIX=a2.dev$DEV_VERSION \
    -DPython3_EXECUTABLE=/Library/Frameworks/Python.framework/Versions/$py3Version/bin/python3 \
    -DTALIPOT_USE_CCACHE=ON \
    -DTALIPOT_BUILD_CORE_ONLY=ON \
    -DTALIPOT_BUILD_DOC=OFF
  make -j4 test-wheel
done

# upload wheels to test PyPI
if [ "$APPVEYOR_REPO_TAG_NAME" == "dev-latest" ]
then
  make test-wheel-upload
fi

# test uploaded wheels in clean environment
sudo port -N -f uninstall \
  cmake \
  clang-${CLANG_VERSION} \
  ccache \
  qhull \
  yajl

sudo pip3.7 install --index-url https://test.pypi.org/simple/ talipot
python3.7 -c "from talipot import tlp; print(tlp.getLayoutAlgorithmPluginsList())"

sudo port -N install \
  python39 \
  py39-pip \
  python310 \
  py310-pip

sudo pip-3.9 install --index-url https://test.pypi.org/simple/ talipot
python3.9 -c "from talipot import tlp; print(tlp.getLayoutAlgorithmPluginsList())"

sudo pip-3.10 install --index-url https://test.pypi.org/simple/ talipot
python3.10 -c "from talipot import tlp; print(tlp.getLayoutAlgorithmPluginsList())"


