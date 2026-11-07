# Install script for directory: E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/microtex")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/Debug/microtex.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/Release/microtex.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/MinSizeRel/microtex.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/RelWithDebInfo/microtex.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex/microtexTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex/microtexTargets.cmake"
         "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/CMakeFiles/Export/fc572e78c32fa3ed3912c9df9d509f66/microtexTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex/microtexTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex/microtexTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/CMakeFiles/Export/fc572e78c32fa3ed3912c9df9d509f66/microtexTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/CMakeFiles/Export/fc572e78c32fa3ed3912c9df9d509f66/microtexTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/CMakeFiles/Export/fc572e78c32fa3ed3912c9df9d509f66/microtexTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/CMakeFiles/Export/fc572e78c32fa3ed3912c9df9d509f66/microtexTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/CMakeFiles/Export/fc572e78c32fa3ed3912c9df9d509f66/microtexTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/microtex" TYPE FILE FILES
    "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/microtexConfig.cmake"
    "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/microtexConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/microtexconfig.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/microtexexport.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/microtex.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/microtexapi.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/utils" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/utils/types.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/render" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/render/render.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/unimath" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/unimath/font_src.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/unimath" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/unimath/font_meta.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/graphic" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/graphic/graphic.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/graphic" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/graphic/font_style.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/microtex/graphic" TYPE FILE FILES "E:/CODE/C/PJ/SCUT Word Exercise/.deps/MicroTeX_richmd/lib/graphic/graphic_basic.h")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "E:/CODE/C/PJ/SCUT Word Exercise/.deps/microtex-static-build/lib/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
