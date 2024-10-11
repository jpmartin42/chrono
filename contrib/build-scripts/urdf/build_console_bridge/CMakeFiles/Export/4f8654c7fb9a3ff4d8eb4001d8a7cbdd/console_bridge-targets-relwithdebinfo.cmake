#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "console_bridge::console_bridge" for configuration "RelWithDebInfo"
set_property(TARGET console_bridge::console_bridge APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(console_bridge::console_bridge PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libconsole_bridge_rd.so.1.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libconsole_bridge_rd.so.1.0"
  )

list(APPEND _cmake_import_check_targets console_bridge::console_bridge )
list(APPEND _cmake_import_check_files_for_console_bridge::console_bridge "${_IMPORT_PREFIX}/lib/libconsole_bridge_rd.so.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
