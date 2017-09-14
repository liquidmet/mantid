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
Mantid Workbench
================
"""

from __future__ import (absolute_import, unicode_literals)

from setuptools import find_packages, setup

from workbench import __license__, __project_url__, __version__

# ==============================================================================
# Metadata
# ==============================================================================
NAME = 'workbench'
AUTHOR = 'mantidproject'
EMAIL = 'mantid-help@mantidproject.org'

# ==============================================================================
# Setup
# ==============================================================================
setup_args = dict(
    name=NAME,
    version=__version__,
    packages=find_packages(),
    url=__project_url__,
    license=__license__,
    author=AUTHOR,
    author_email=EMAIL,
    # Install this as a directory
    zip_safe=False,
)

setup(**setup_args)
