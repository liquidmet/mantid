from __future__ import (absolute_import, division, print_function, unicode_literals)

import unittest

class ImportTest(unittest.TestCase):

    def test_import(self):
        import mantidqt # noqa

    def test_import_qtpy(self):
        import mantidqt.qtpy # noqa
        from mantidqt.qtpy import QtCore# noqa
        from mantidqt.qtpy import QtGui# noqa
        from mantidqt.qtpy import QtWidgets# noqa

if __name__ == "__main__":
    unittest.main()