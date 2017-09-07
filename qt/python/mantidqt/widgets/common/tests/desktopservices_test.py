from __future__ import (absolute_import, division,
                        print_function, unicode_literals)

import unittest

class DesktopServicesTest(unittest.TestCase):

    def test_class_exists_with_expected_name(self):
        from mantidqt.widgets.common import DesktopServices # noqa


if __name__ == '__main__':
    unittest.main()