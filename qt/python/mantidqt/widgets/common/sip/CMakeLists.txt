# Build rules for the sip bindings of the C++ WidgetsCommon library.
#

# The main specification file is configured automatically from the following
# information
set ( MODULE_NAME _common )
set ( MODULE_OUTPUT_DIR ${MANTIDQT_OUTPUT_DIR}/mantidqt/widgets/common )
# The following files are included within the generated definition file
set ( SIP_SRCS_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set ( SIP_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/MantidDesktopServices.sip
)

# The build rule must depend on the C++ header files included by the above
# .sip files so that any change in the header causes a rebuild of the export.
set ( COMMON_HEADER_DIR ../../../../../widgets/common/inc/MantidQtWidgets/Common )
set ( HEADER_DEPENDS
  ${COMMON_HEADER_DIR}/MantidDesktopServices.h
)






# Create the module spec file
set ( SIP_INCLUDES )
foreach ( SIP_FILE ${SIP_SRCS})
  get_filename_component (_sip_filename ${SIP_FILE} NAME)
  set ( SIP_INCLUDES "${SIP_INCLUDES}%Include ${_sip_filename}\n" )
endforeach ()

set ( MODULE_SPEC ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.sip )
configure_file ( sipmodule_template.sip.in ${MODULE_SPEC} )

# Run sip code generator
set ( PYQT_SIP_DIR ${PYQT4_SIP_DIR} )
set ( PYQT_SIP_FLAGS ${PYQT4_SIP_FLAGS} )
set ( SIP_GENERATED_CPP ${CMAKE_CURRENT_BINARY_DIR}/sip${MODULE_NAME}part0.cpp )
add_custom_command ( OUTPUT ${SIP_GENERATED_CPP}

  COMMAND ${SIP_EXECUTABLE}
    -I ${SIP_SRCS_DIR} -I ${PYQT_SIP_DIR}
    ${PYQT_SIP_FLAGS}
    -c ${CMAKE_CURRENT_BINARY_DIR} -j1 -w -e ${MODULE_SPEC}
  DEPENDS ${MODULE_SPEC} ${SIP_SRCS} ${HEADER_DEPENDS}
  COMMENT "Generating ${MODULE_NAME} python bindings with sip"
)

set ( TARGET_NAME mantidqtwidgetscommoncpp )
add_library ( ${TARGET_NAME} MODULE ${SIP_GENERATED_CPP} )
target_include_directories ( ${TARGET_NAME} SYSTEM PRIVATE ${SIP_INCLUDE_DIR} )
target_link_libraries ( ${TARGET_NAME} PRIVATE
  Qt4::QtGui
)
set_target_properties ( ${TARGET_NAME} PROPERTIES
  LIBRARY_OUTPUT_NAME ${MODULE_NAME}
  LIBRARY_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR}
)
if ( WIN32 )
  set_target_properties( ${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".pyd" )
elseif ( APPLE )
  set_target_properties( ${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".so" )
  if (OSX_VERSION VERSION_GREATER 10.8)
    set_target_properties ( ${TARGET_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../MacOS")
  endif ()
else ()
  set_target_properties( ${TARGET_NAME} PROPERTIES PREFIX "" )
endif ()