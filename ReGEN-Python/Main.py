from src.ReGEN.Test.Test import Test
import numpy as np

#Test = Test('Skyrim_Test', False)
np.random.seed(42)
Test = Test('one_story', True)
Test.run()
