#
# Build rules for the sip bindings of the C++ WidgetsCommon library.
#
include ( SipTargets )

# The main specification file is configured automatically from the following
# information
# The following files are included within the generated definition file
set ( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set ( SIP_SRCS
  MantidDesktopServices.sip
)

# The build rule must depend on the C++ header files included by the above
# .sip files so that any change in the header causes a rebuild of the export.
set ( COMMON_HEADER_DIR ../../../../../widgets/common/inc/MantidQtWidgets/Common )
set ( HEADER_DEPENDS
  ${COMMON_HEADER_DIR}/MantidDesktopServices.h
)

mtd_add_sip_module (
  MODULE_NAME _common
  TARGET_NAME mantidqt_widgets_common
  MODULE_OUTPUT_DIR ${MANTIDQT_OUTPUT_DIR}/mantidqt/widgets/common
  SIP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  SIP_SRCS ${SIP_SRCS}
  HEADER_DEPS ${HEADER_DEPENDS}
  PYQT_VERSION 4
  LINK_LIBS MantidQtWidgetsCommon
  FOLDER mantidqt
)



