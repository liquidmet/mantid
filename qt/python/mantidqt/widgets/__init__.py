""" Defines the api for the common subpackage of mantidqt.widgets
"""
from __future__ import (absolute_import, unicode_literals)

from . import _common

# Define import all API
__all__ = ["DesktopServices"]

# Handle renaming to string the silly namespace->class translation that sip does
DesktopServices = _common.MantidQt.API.MantidDesktopServices