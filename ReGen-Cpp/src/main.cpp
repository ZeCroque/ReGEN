#include "CommandsDeclaration.h"
#include "DataManager.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();

	//DataManager::getInstance()->saveDataAsDotFormat(true, true);
	DataManager::getInstance()->printTestLayout();
	DataManager::getInstance()->printStoryModifications();
	
	return 0;
}
