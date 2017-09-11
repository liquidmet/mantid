"""
mantidqt.py3compat
-------------------

Transitional module providing compatibility functions intended to help
migrating from Python 2 to Python 3. Mostly just wraps six but allowing
for additional functionality of our own.

This module should be fully compatible with:
    * Python >=v2.7
    * Python 3
"""

from __future__ import absolute_import, print_function

import six
from six import * # noqa

# -----------------------------------------------------------------------------
# Globals and constants
# -----------------------------------------------------------------------------
__all__ = dir(six)


# -----------------------------------------------------------------------------
# Strings
# -----------------------------------------------------------------------------
def is_text_string(obj):
    """Return True if `obj` is a text string, False if it is anything else,
    like binary data (Python 3) or QString (Python 2, PyQt API #1)"""
    if PY2:
        # Python 2
        return isinstance(obj, basestring)
    else:
        # Python 3
        return isinstance(obj, str)


def to_text_string(obj, encoding=None):
    """Convert `obj` to (unicode) text string"""
    if PY2:
        # Python 2
        if encoding is None:
            return unicode(obj)
        else:
            return unicode(obj, encoding)
    else:
        # Python 3
        if encoding is None:
            return str(obj)
        elif isinstance(obj, str):
            # In case this function is not used properly, this could happen
            return obj
        else:
            return str(obj, encoding)