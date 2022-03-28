CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/make_appimage_bundle.sh.in"
               "${CMAKE_CURRENT_BINARY_DIR}/make_appimage_bundle.sh" @ONLY)

ADD_CUSTOM_TARGET(
  bundle-legacy
  COMMAND
    docker run --rm=true -it -v ${CMAKE_SOURCE_DIR}:/talipot:rw -v
    ${CMAKE_BINARY_DIR}:/talipot_host_build:rw -v /tmp/.X11-unix:/tmp/.X11-unix
    -e DISPLAY=$DISPLAY centos:7 /bin/bash -c
    "bash -xe /talipot/bundlers/linux/talipot_appimage_centos_build.sh"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

ADD_CUSTOM_TARGET(
  bundle
  COMMAND
    docker run --rm=true -it -v ${CMAKE_SOURCE_DIR}:/talipot:rw -v
    ${CMAKE_BINARY_DIR}:/talipot_host_build:rw -v /tmp/.X11-unix:/tmp/.X11-unix
    -e DISPLAY=$DISPLAY tgagor/centos-stream:8 /bin/bash -c
    "bash -xe /talipot/bundlers/linux/talipot_appimage_centos_build.sh"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
