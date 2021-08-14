from src.ReGEN.Test.Test import Test
import src.ReGEN.Graph.Globals as globals

globals.initGlobals()

#Test = Test('Skyrim_Test', False)
Test = Test('one_story', False)
Test.run()
