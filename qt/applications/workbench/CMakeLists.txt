# This file manages building/installation of the workbench application
# It uses the setuptools rules as much as possible

# Destination
set ( WORKBENCH_OUTPUT_DIR ${MTD_SITE_PACKAGES}/workbench )

# Configure setup.py
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/setup.py @ONLY )

# Configure the workbench/version.py. This has to be done in two steps so that the
# multi-config generators put the file in the build location
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/workbench/version.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/workbench/version.py.generated @ONLY )
# Build into the appropriate place
add_custom_command (
  OUTPUT "${MTD_SITE_PACKAGES}/workbench/version.py"

  COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/workbench/version.py.generated ${WORKBENCH_OUTPUT_DIR}/version.py

  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/workbench/version.py.generated
)

# "Build" the package
add_custom_command (
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/workbench.stamp"
  # build with setuptools
  COMMAND ${CMAKE_COMMAND} ARGS -E echo "building workbench to 'bin/../lib/site-packages/workbench'"
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup.py build
    --build-lib=${MTD_SITE_PACKAGES}

  # compile
#  COMMAND ${CMAKE_COMMAND} ARGS -E echo "compiling '.../site-packages/workbench'"
#  COMMAND ${PYTHON_EXECUTABLE} -m compileall -q
#          "${WORKBENCH_OUTPUT_DIR}"

  # touch the marker file
  COMMAND ${CMAKE_COMMAND} ARGS -E touch "${CMAKE_CURRENT_BINARY_DIR}/workbench.stamp"

  DEPENDS ${MTD_SITE_PACKAGES}/workbench/version.py
)

add_custom_target ( workbench ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/workbench.stamp )

## Tests
#set ( PYTHON_TEST_FILES
#  workbench/config/tests/userconfig_test.py
#)

#pyunittest_add_test ( ${CMAKE_CURRENT_SOURCE_DIR} workbench ${PYTHON_TEST_FILES} )
