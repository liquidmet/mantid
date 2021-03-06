set ( SRC_FILES
	src/Communicator.cpp
	src/ExecutionMode.cpp
	src/Request.cpp
	src/StorageMode.cpp
	src/ThreadingBackend.cpp
)

set ( INC_FILES
	inc/MantidParallel/Collectives.h
	inc/MantidParallel/Communicator.h
	inc/MantidParallel/ExecutionMode.h
	inc/MantidParallel/Request.h
	inc/MantidParallel/StorageMode.h
	inc/MantidParallel/ThreadingBackend.h
)

set ( TEST_FILES
	CollectivesTest.h
	CommunicatorTest.h
	ExecutionModeTest.h
	ParallelRunnerTest.h
	RequestTest.h
	StorageModeTest.h
	ThreadingBackendTest.h
)

if (COVERALLS)
  foreach( loop_var ${SRC_FILES} ${INC_FILES})
    set_property(GLOBAL APPEND PROPERTY COVERAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
  endforeach(loop_var)
endif()

# Add the target for this directory
add_library ( Parallel ${SRC_FILES} ${INC_FILES} )
# Set the name of the generated library
set_target_properties ( Parallel PROPERTIES OUTPUT_NAME MantidParallel
  COMPILE_DEFINITIONS IN_MANTID_PARALLEL )

if (OSX_VERSION VERSION_GREATER 10.8)
  set_target_properties ( Parallel PROPERTIES INSTALL_RPATH "@loader_path/../MacOS")
endif ()

# Add to the 'Framework' group in VS
set_property ( TARGET Parallel PROPERTY FOLDER "MantidFramework" )

target_link_libraries ( Parallel LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME}
                        ${GSL_LIBRARIES} ${MANTIDLIBS} )

# Add the unit tests directory
add_subdirectory ( test )

###########################################################################
# Installation settings
###########################################################################

install ( TARGETS Parallel ${SYSTEM_PACKAGE_TARGET} DESTINATION ${LIB_DIR} )
