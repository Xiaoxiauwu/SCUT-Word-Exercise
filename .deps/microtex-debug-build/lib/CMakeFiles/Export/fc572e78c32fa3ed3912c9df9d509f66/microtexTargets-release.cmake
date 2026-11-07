#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "microtex::microtex" for configuration "Release"
set_property(TARGET microtex::microtex APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(microtex::microtex PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/microtex.lib"
  )

list(APPEND _cmake_import_check_targets microtex::microtex )
list(APPEND _cmake_import_check_files_for_microtex::microtex "${_IMPORT_PREFIX}/lib/microtex.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
