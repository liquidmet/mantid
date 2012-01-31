import unittest
from mantid import ReferenceFrame

class ReferenceFrameTest(object):
    def test_ReferenceFrame_cannot_be_instantiated(self):
        self.assertFalse(can_be_instantiated(Instrument))

    def test_ReferenceFrame_has_expected_attrs(self):
        expected_attrs = ["pointingAlongBeam", "pointingUp"]
        for att in expected_attrs:
            self.assertTrue(att in attrs)
if __name__ == '__main__':
    unittest.main()
