#    This file is part of the mantid workbench.
#
#    Copyright (C) 2017 mantidproject
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""
workbench.config.main
---------------------

Creates an application-wide config instance. This module is imported early
in the startup of the mainwindow. See workbench.app.maindwindow.
"""

from __future__ import (absolute_import, unicode_literals)

from workbench.config.user import UserConfig

# -----------------------------------------------------------------------------
# Constants
# -----------------------------------------------------------------------------
ORG = 'mantidproject'
APP = 'workbench'

# Iterable containing defaults for each configurable section of the code
# General application settings are in the main section
DEFAULTS = [
    ('main',
     {
      'high_dpi_scaling': True,
     })
]

# -----------------------------------------------------------------------------
# 'Singleton' instance
# -----------------------------------------------------------------------------
# IMPORTANT NOTES:
# 1. If you want to *change* the default value of a current option, you need to
#    do a MINOR update in config version, e.g. from 1.0.0 to 1.1.0
# 2. If you want to *remove* options that are no longer needed,
#    or if you want to *rename* options, then you need to do a MAJOR update in
#    version, e.g. from 1.0.0 to 2.0.0
# 3. You don't need to touch this value if you're just adding a new option
CONF_VERSION = '1.0.0'

# Main instance - this object should be used across the application to
# load/store any user setting
CONF = UserConfig(ORG, APP, defaults=DEFAULTS)