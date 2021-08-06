#include "CommandsDeclaration.h"
#include "DataManager.h"
#include "Scheduler.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();
	Scheduler sched(DataManager::getInstance()->getWorldGraph());
	sched.run();
	
	//std::list<std::list<std::string> > result;
	//DataManager::getInstance()->getWorldGraph().getIsomorphicSubGraphs(DataManager::getInstance()->getInitializationRules().begin()->socialConditions, result);
	/*DataManager::getInstance()->saveDataAsDotFormat(true, true);
	DataManager::getInstance()->printTestLayout();
	DataManager::getInstance()->printStoryModifications();*/
	
	return 0;
}
