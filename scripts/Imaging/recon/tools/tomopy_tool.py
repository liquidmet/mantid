from __future__ import (absolute_import, division, print_function)
from recon.tools.abstract_tool import AbstractTool


class TomoPyTool(AbstractTool):

    def __init__(self):
        AbstractTool.__init__(self)
        self._tomopy = self.import_self()
        self._tool_supported_methods = ['art', 'bart', 'fbp', 'gridrec', 'mlem',
                                        'osem', 'ospml_hybrid', 'ospml_quad', 'pml_hybrid', 'pml_quad', 'sirt']

    def check_algorithm_compatibility(self, config):
        algorithm = config.func.algorithm
        if algorithm not in self._tool_supported_methods:
            raise ValueError("The selected algorithm {0} is not supported by TomoPy.".format(algorithm))

    def find_center(self, **kwargs):
        # just forward to tomopy
        return self._tomopy.find_center(**kwargs)

    def import_self(self):
        try:
            import tomopy
            import tomopy.prep
            import tomopy.recon
            import tomopy.misc
            import tomopy.io

        except ImportError as exc:
            raise ImportError("Could not import the tomopy package and its subpackages. Details: {0}".
                              format(exc))

        return tomopy

    def run_reconstruct(self, data, config):
        import numpy as np
        from recon.helper import Helper

        h = Helper(config)

        h.check_data_stack(data)

        num_proj = data.shape[0]
        inc = float(config.func.max_angle) / num_proj

        proj_angles = np.arange(0, num_proj * inc, inc)

        proj_angles = np.radians(proj_angles)

        alg = config.func.algorithm
        cor = config.func.cor
        num_iter = config.func.num_iter

        h.tomo_print(" * Using center of rotation: {0}".format(cor))

        iterative_algorithm = False if alg in ['gridrec', 'fbp'] else True

        # run the iterative algorithms
        if iterative_algorithm:
            h.pstart(
                " * Starting iterative method with TomoPy. Algorithm: {0}, "
                "number of iterations: {1}...".format(alg, num_iter))

            recon = self._tomopy.recon(tomo=data, theta=proj_angles, center=cor,
                                       algorithm=alg, num_iter=num_iter)  # , filter_name='parzen')

        else:  # run the non-iterative algorithms
            h.pstart(
                " * Starting non-iterative reconstruction algorithm with TomoPy. "
                "Algorithm: {0}...".format(alg))
            recon = self._tomopy.recon(
                tomo=data, theta=proj_angles, center=cor, algorithm=alg)

        h.pstop(
            " * Reconstructed 3D volume. Shape: {0}, and pixel data type: {1}.".
            format(recon.shape, recon.dtype))

        return recon
