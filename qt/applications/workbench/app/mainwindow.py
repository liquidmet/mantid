"""
Defines the QMainWindow of the application and the main() entry point.
"""
from __future__ import (absolute_import, division,
                        print_function, unicode_literals)

# -----------------------------------------------------------------------------
# Requirements
# -----------------------------------------------------------------------------
from workbench import requirements
requirements.check_qt()

# -----------------------------------------------------------------------------
# Qt
# -----------------------------------------------------------------------------
from mantidqt.qtpy.QtCore import QCoreApplication, Qt
from mantidqt.qtpy.QtWidgets import QApplication, QMainWindow

# -----------------------------------------------------------------------------
# High-dpi scaling (if available). Must be set before the QApplication
# instance is created
# -----------------------------------------------------------------------------
if hasattr(Qt, 'AA_EnableHighDpiScaling'):
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)