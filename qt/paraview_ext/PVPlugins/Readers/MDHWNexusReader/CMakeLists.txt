project( MantidParaViewMDHWNexusReader )
include_directories( SYSTEM ${PARAVIEW_INCLUDE_DIRS} )
add_paraview_plugin( MantidParaViewMDHWNexusReaderSMPlugin "1.0"
	SERVER_MANAGER_XML MDHWNexusReader.xml
	SERVER_MANAGER_SOURCES vtkMDHWNexusReader.cxx
)

# Add to the 'VatesParaViewPlugins' group in VS
set_property( TARGET MantidParaViewMDHWNexusReaderSMPlugin PROPERTY FOLDER "MantidVatesParaViewPlugins" )

include_directories ( SYSTEM ${QWT_INCLUDE_DIR} )

target_link_libraries( MantidParaViewMDHWNexusReaderSMPlugin LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME} 
${CORE_MANTIDLIBS}
DataObjects
VatesAPI
${POCO_LIBRARIES}
${Boost_LIBRARIES}
${vtkjsoncpp_LIBRARIES}
${QWT_LIBRARIES}
Qt4::QtCore
)

if (OSX_VERSION VERSION_GREATER 10.8)
  set_target_properties ( MantidParaViewMDHWNexusReaderSMPlugin PROPERTIES INSTALL_RPATH "@loader_path/../../Contents/Libraries;@loader_path/../../Contents/MacOS")
endif ()

# Put library into subfolder.
SET_TARGET_OUTPUT_DIRECTORY(MantidParaViewMDHWNexusReaderSMPlugin ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${PVPLUGINS_DIR}/${PVPLUGINS_SUBDIR})

install( TARGETS MantidParaViewMDHWNexusReaderSMPlugin ${SYSTEM_PACKAGE_TARGET} DESTINATION ${PVPLUGINS_DIR}/${PVPLUGINS_SUBDIR} )


