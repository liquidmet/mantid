﻿import unittest
from mantid.api import AlgorithmManager
from SANSStatePrototype import SANSStatePrototype


class SANSStatePrototypeTest(unittest.TestCase):
    def test_prototype(self):
        state = SANSStatePrototype()
        state.parameter1 = "test_value"

        alg = AlgorithmManager.createUnmanaged("SANSAlgorithmPrototype")
        alg.setChild(True)
        alg.initialize()
        # This should accept a PropertyManager
        import time
        time.sleep(10)
        property_manager = state.property_manager
        alg.setProperty("Factor", 1)
        alg.setProperty("SANSStatePrototype", property_manager)
        alg.execute()


if __name__ == '__main__':
    unittest.main()