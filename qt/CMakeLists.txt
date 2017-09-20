###########################################################################
# Now add the packages one-by-one, building up the dependencies as we go
###########################################################################
find_package ( QScintilla REQUIRED )

include ( UseQt4 )
include_directories ( SYSTEM ${QWT_INCLUDE_DIR} )

# Utilities for defining targets
include (QtTargetFunctions)

add_subdirectory ( widgets )
add_subdirectory ( python )
add_subdirectory ( scientific_interfaces )
if ( MAKE_VATES )
  add_subdirectory ( paraview_ext )
endif ( MAKE_VATES )

if(ENABLE_WORKBENCH)
  add_subdirectory (applications/workbench)
endif()

###########################################################################
# Add a custom target to build all of the Qt-related functionality
###########################################################################

# MantidQt4 is the folder/target for all legacy functionality related to MantidPlot
set ( MANTID_QT4_TARGETS
  MantidQtWidgetsCommonQt4
  MantidQtWidgetsSliceViewerQt4
  MantidQtWidgetsSpectrumViewerQt4
  MantidQtWidgetsRefDetectorViewQt4
  MantidQtWidgetsFactoryQt4
  MantidQtWidgetsPluginsAlgorithmDialogsQt4
  mantidqtpython
)
add_custom_target (MantidQt4 DEPENDS ${MANTID_QT4_TARGETS})
if ( MSVC_IDE )
  set_target_properties ( ${MANTID_QT4_TARGETS} PROPERTIES FOLDER "MantidQt4" )
endif ()
