#include "CommandsDeclaration.h"
#include "DataManager.h"
#include "Scheduler.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();

	for(int i = 0; i < DataManager::getInstance()->getTestLayout().numStoryToGenerate; ++i)
	{
		Scheduler scheduler("narrative" + std::to_string(i));
		scheduler.run();
	}

	return 0;
}
