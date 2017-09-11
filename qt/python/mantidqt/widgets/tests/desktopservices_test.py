from __future__ import (absolute_import, division,
                        print_function, unicode_literals)

import inspect
import unittest

class DesktopServicesTest(unittest.TestCase):

    def test_class_exists_with_expected_name(self):
        from mantidqt.widgets import DesktopServices # noqa
        self.assertTrue(inspect.isclass(DesktopServices))

    def test_method_exists_on_class(self):
        from mantidqt.widgets import DesktopServices # noqa
        self.assertTrue(hasattr(DesktopServices, 'openUrl'))


if __name__ == '__main__':
    unittest.main()