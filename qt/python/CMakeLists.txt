# This file manages building/installation of the   mantidqt and   mantidqtpython
# Python wrappers.
#

# Legacy wrappers for MantidPlot
add_subdirectory ( mantidqtpython )

# Define build location of installed files
# In the future we should move to push all other
# python modules to this directory
set ( BUILD_PYTHON_MODULE_DIR
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/lib/site-packages )

# All source files
set ( PYTHON_SOURCE_FILES
  mantidqt/__init__.py
  mantidqt/qtpy/QtMultimedia.py
  mantidqt/qtpy/QtOpenGL.py
  mantidqt/qtpy/__init__.py
  mantidqt/qtpy/_version.py
  mantidqt/qtpy/QtTest.py
  mantidqt/qtpy/QtCore.py
  mantidqt/qtpy/QtNetwork.py
  mantidqt/qtpy/py3compat.py
  mantidqt/qtpy/QtGui.py
  mantidqt/qtpy/QtDesigner.py
  mantidqt/qtpy/QtPrintSupport.py
  mantidqt/qtpy/QtWidgets.py
  mantidqt/qtpy/uic.py
  mantidqt/qtpy/QtWebEngineWidgets.py
  mantidqt/qtpy/compat.py
  mantidqt/qtpy/QtSvg.py
  mantidqt/qtpy/_patch/__init__.py
  mantidqt/qtpy/_patch/qcombobox.py
  mantidqt/qtpy/_patch/qheaderview.py
)

# Copy to build directory and compile
# Inspired by ParaView/Wrapping/Python/CMakeLists.txt
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/copy_and_compile_py_files_complete"
  # copy
  COMMAND ${CMAKE_COMMAND} ARGS -E echo "copying mantidqt/*.py to '.../site-packages/mantidqt'"
  COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/mantidqt"
          "${BUILD_PYTHON_MODULE_DIR}/mantidqt"

  # compile
  COMMAND ${CMAKE_COMMAND} ARGS -E echo "compiling '.../site-packages/mantidqt'"
  COMMAND ${PYTHON_EXECUTABLE} -m compileall -q
          ${BUILD_PYTHON_MODULE_DIR}/mantidqt

  # touch the marker file
  COMMAND ${CMAKE_COMMAND} ARGS -E touch "${CMAKE_CURRENT_BINARY_DIR}/copy_and_compile_py_files_complete"

  DEPENDS
    ${PYTHON_SOURCE_FILES}
)

# Target
add_custom_target(mantidqt_pyc DEPENDS copy_and_compile_py_files_complete)

# Not installed yet...