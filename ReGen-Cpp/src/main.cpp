#include "CommandsDeclaration.h"
#include "DataManager.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();

	std::list<Graph> result;
	DataManager::getInstance()->worldGraph.getIsomorphicSubGraphs(DataManager::getInstance()->initializationRules.begin()->socialConditions, result);
	/*DataManager::getInstance()->saveDataAsDotFormat(true, true);
	DataManager::getInstance()->printTestLayout();
	DataManager::getInstance()->printStoryModifications();*/
	
	return 0;
}
