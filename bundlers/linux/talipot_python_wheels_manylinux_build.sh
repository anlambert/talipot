#!/bin/bash

# This script is only intended to be run in a pypa/manylinux2014
# docker image (based on CentOS 7)

yum -y install epel-release ccache

# install talipot-core wheel deps
yum -y install zlib-devel libzstd-devel yajl-devel qhull-devel graphviz-devel libgit2-devel

# required to build upstream python cffi from pip
yum -y install libffi-devel


JSON=$(curl -s 'https://test.pypi.org/pypi/talipot/json')
LAST_VERSION=$(echo $JSON | python3 -c "
import sys, json
print(json.load(sys.stdin)['info']['version'])" 2>/dev/null)
if [ $? -ne 0 ]
then
  DEV_VERSION=1
else
  DEV_VERSION=$(echo $LAST_VERSION | cut -f4 -d '.' | sed 's/dev//')
  VERSION_INCREMENT=$(echo $JSON | python -c "
import sys, json
releases = json.load(sys.stdin)['releases']['$LAST_VERSION']
print(any(['manylinux' in r['filename'] for r in releases]))")
  if [ "$VERSION_INCREMENT" == "True" ]
  then
    let DEV_VERSION+=1
  fi
fi

echo current wheel dev version = $DEV_VERSION

# abort script on first error
set -e

# get talipot source dir
if [ -d /talipot ]
then
  TALIPOT_SRC=/talipot
else
  # talipot sources install
  cd /tmp
  git clone https://github.com/anlambert/talipot.git
  TALIPOT_SRC=/tmp/talipot
fi

cat > ~/.pypirc << EOF
[distutils]
index-servers=
    testpypi
[testpypi]
repository: https://test.pypi.org/legacy/
username: __token__
password: $1
EOF

# build talipot
if [ -d /talipot_build ]
then
  cd /talipot_build; rm -rf *;
else
  mkdir /tmp/talipot_build; cd /tmp/talipot_build
fi
# iterate on available Python versions
for CPYBIN in /opt/python/cp*/bin
do
  if [[ $CPYBIN == *"cp27"* ]] || [[ $CPYBIN == *"cp34"* ]] \
    || [[ $CPYBIN == *"cp35"* ]] || [[ $CPYBIN == *"cp36"* ]] \
    || [[ $CPYBIN == *"cp312"* ]]
  then
    continue
  fi
  ${CPYBIN}/pip install twine sip
  # configure and build python wheel with specific Python version
  cmake ${TALIPOT_SRC} \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/tmp/talipot_install \
        -DPython3_EXECUTABLE=${CPYBIN}/python \
        -DTALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET=ON \
        -DTALIPOT_PYTHON_TEST_WHEEL_SUFFIX=a2.dev$DEV_VERSION \
        -DTALIPOT_BUILD_DOC=OFF \
        -DTALIPOT_USE_CCACHE=ON
  make -j4
  if [ -n "$DEV_VERSION" ]
  then
    make test-wheel
  else
    make wheel
  fi
  if [ $? -ne 0 ]
  then
    break
  fi

  # check the talipot wheel
  pushd ./library/talipot-python/bindings/talipot-core/talipot_module/dist
  ${CPYBIN}/pip install $(ls -t | head -1)
  ${CPYBIN}/python -c "
from talipot import tlp
from platform import python_version
print('Talipot %s successfully imported in Python %s' %
      (tlp.getRelease(), python_version()))
"
  if [ $? -ne 0 ]
  then
     break
  fi
  popd
done

if [ -n "$DEV_VERSION" ]
then
  if [ "$2" == "refs/tags/dev-latest" ]
  then
    make test-wheel-upload
  fi
fi
