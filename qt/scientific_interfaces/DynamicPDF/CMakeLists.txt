set ( SRC_FILES
	DPDFBackgroundRemover.cpp
	DPDFDisplayControl.cpp
	DPDFFitControl.cpp
	DPDFFitOptionsBrowser.cpp
	DPDFFourierTransform.cpp
	DPDFInputDataControl.cpp
	DisplayCurveFitTest.cpp
	SliceSelector.cpp
)

# Include files aren't required, but this makes them appear in Visual Studio
# IMPORTANT: Include files are required in the MOC_FILES set. Scroll down to find it.
set ( INC_FILES
	DllConfig.h
	DPDFBackgroundRemover.h
	DPDFDisplayControl.h
	DPDFFitControl.h
	DPDFFitOptionsBrowser.h
	DPDFFourierTransform.h
	DPDFInputDataControl.h
	DisplayCurveFitTest.h
	SliceSelector.h
)

set ( MOC_FILES 
    DisplayCurveFitTest.h
    DPDFBackgroundRemover.h
    DPDFDisplayControl.h
    DPDFFitControl.h
    DPDFFitOptionsBrowser.h
    DPDFFourierTransform.h
    DPDFInputDataControl.h
    SliceSelector.h
)

set ( UI_FILES 
    DisplayCurveFitTest.ui
    DPDFBackgroundRemover.ui
    DPDFFitControl.ui
    DPDFFourierTransform.ui
    SliceSelector.ui
)

qt4_wrap_cpp ( MOCCED_FILES ${MOC_FILES} OPTIONS ${extra_options} )

set ( ALL_SRC ${SRC_FILES} ${MOCCED_FILES} )

qt4_wrap_ui ( UI_HDRS ${UI_FILES} )
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

# Use a precompiled header where they are supported
enable_precompiled_headers( PrecompiledHeader.h  ALL_SRC )
add_library ( MantidScientificInterfacesDynamicPDF ${ALL_SRC} ${INC_FILES} ${UI_HDRS} )

# Set the name of the generated library
set_target_properties ( MantidScientificInterfacesDynamicPDF PROPERTIES COMPILE_DEFINITIONS IN_MANTIDQT_DYNAMICPDF )
if ( MSVC_IDE )
#  # Add to the 'ScientificInterfaces' group in VS
  set_property ( TARGET MantidScientificInterfacesDynamicPDF PROPERTY FOLDER "ScientificInterfaces" )
endif()

if (OSX_VERSION VERSION_GREATER 10.8)
  set_target_properties ( MantidScientificInterfacesDynamicPDF PROPERTIES INSTALL_RPATH "@loader_path/../../../Contents/MacOS")
endif ()

find_Package(Threads)

target_link_libraries ( MantidScientificInterfacesDynamicPDF LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME}
            MantidQtWidgetsCommon
            ${CORE_MANTIDLIBS}
            ${QT_LIBRARIES}
            ${QWT_LIBRARIES}
            ${POCO_LIBRARIES}
            ${Boost_LIBRARIES}
            ${JSONCPP_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

###########################################################################
# Installation settings
###########################################################################

install ( TARGETS MantidScientificInterfacesDynamicPDF ${SYSTEM_PACKAGE_TARGET} DESTINATION ${PLUGINS_DIR}/qtplugins/mantid )
