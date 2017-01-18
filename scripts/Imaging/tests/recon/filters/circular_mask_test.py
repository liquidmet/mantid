from __future__ import (absolute_import, division, print_function)
import unittest
import numpy.testing as npt


class CircularMaskTest(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(CircularMaskTest, self).__init__(*args, **kwargs)

        # force silent outputs
        from recon.configs.recon_config import ReconstructionConfig
        r = ReconstructionConfig.empty_init()
        r.func.verbosity = 0
        from recon.helper import Helper

        self.h = Helper(r)

    def test_import(self):
        """
        Only check that the filter imports successfully
        """
        from recon.filters import circular_mask
        pass

    def test_not_executed(self):
        """
        Check that the filter is not executed when:
            - no ratio is provided
            - 0 < ratio < 1 is false
        """
        # images that will be put through testing
        images = self.generate_images()

        # control images
        control = self.generate_images()
        err_msg = "TEST NOT EXECUTED :: Running circular mask with ratio {0} changed the data!"
        from recon.filters import circular_mask

        ratio = 0
        result = circular_mask.execute(images, ratio, self.h)
        npt.assert_equal(result, control, err_msg=err_msg.format(ratio))

        ratio = 1
        result = circular_mask.execute(images, ratio, self.h)
        npt.assert_equal(result, control, err_msg=err_msg.format(ratio))

        ratio = -1
        result = circular_mask.execute(images, ratio, self.h)
        npt.assert_equal(result, control, err_msg=err_msg.format(ratio))

        ratio = None
        result = circular_mask.execute(images, ratio, self.h)
        npt.assert_equal(result, control, err_msg=err_msg.format(ratio))

    def test_executed(self):
        """
        Check that the filter changed the data.
        """
        # images that will be put through testing
        images = self.generate_images()

        # control images
        control = self.generate_images()

        from recon.filters import circular_mask

        ratio = 0.001
        result = circular_mask.execute(images, ratio, self.h)
        npt.assert_raises(
            AssertionError, npt.assert_array_equal, result, control)

        # reset the input images
        images = self.generate_images()
        ratio = 0.994
        result = circular_mask.execute(images, ratio, self.h)
        npt.assert_raises(
            AssertionError, npt.assert_array_equal, result, control)

    @staticmethod
    def generate_images():
        import numpy as np
        # generate 10 images with dimensions 10x10, all values 1. float32
        return np.full((10, 10, 10), 1., dtype=np.float32)

if __name__ == '__main__':
    unittest.main()