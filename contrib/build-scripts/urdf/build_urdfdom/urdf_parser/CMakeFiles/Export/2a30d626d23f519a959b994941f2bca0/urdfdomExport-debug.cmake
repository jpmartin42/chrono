#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "urdfdom::urdfdom_model" for configuration "Debug"
set_property(TARGET urdfdom::urdfdom_model APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(urdfdom::urdfdom_model PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "console_bridge::console_bridge"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liburdfdom_model_d.so.4.0"
  IMPORTED_SONAME_DEBUG "liburdfdom_model_d.so.4.0"
  )

list(APPEND _cmake_import_check_targets urdfdom::urdfdom_model )
list(APPEND _cmake_import_check_files_for_urdfdom::urdfdom_model "${_IMPORT_PREFIX}/lib/liburdfdom_model_d.so.4.0" )

# Import target "urdfdom::urdfdom_world" for configuration "Debug"
set_property(TARGET urdfdom::urdfdom_world APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(urdfdom::urdfdom_world PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "console_bridge::console_bridge"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liburdfdom_world_d.so.4.0"
  IMPORTED_SONAME_DEBUG "liburdfdom_world_d.so.4.0"
  )

list(APPEND _cmake_import_check_targets urdfdom::urdfdom_world )
list(APPEND _cmake_import_check_files_for_urdfdom::urdfdom_world "${_IMPORT_PREFIX}/lib/liburdfdom_world_d.so.4.0" )

# Import target "urdfdom::urdfdom_sensor" for configuration "Debug"
set_property(TARGET urdfdom::urdfdom_sensor APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(urdfdom::urdfdom_sensor PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "console_bridge::console_bridge"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liburdfdom_sensor_d.so.4.0"
  IMPORTED_SONAME_DEBUG "liburdfdom_sensor_d.so.4.0"
  )

list(APPEND _cmake_import_check_targets urdfdom::urdfdom_sensor )
list(APPEND _cmake_import_check_files_for_urdfdom::urdfdom_sensor "${_IMPORT_PREFIX}/lib/liburdfdom_sensor_d.so.4.0" )

# Import target "urdfdom::urdfdom_model_state" for configuration "Debug"
set_property(TARGET urdfdom::urdfdom_model_state APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(urdfdom::urdfdom_model_state PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "console_bridge::console_bridge"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liburdfdom_model_state_d.so.4.0"
  IMPORTED_SONAME_DEBUG "liburdfdom_model_state_d.so.4.0"
  )

list(APPEND _cmake_import_check_targets urdfdom::urdfdom_model_state )
list(APPEND _cmake_import_check_files_for_urdfdom::urdfdom_model_state "${_IMPORT_PREFIX}/lib/liburdfdom_model_state_d.so.4.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
