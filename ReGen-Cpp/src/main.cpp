#include "CommandsRegistry.h"
#include "CommandsDeclaration.h"
#include "DataManager.h"
#include "Graph.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	//DataManager::getInstance()->saveDataAsDotFormat(true, true);
	DataManager::getInstance()->printTestLayout();
	
	declareCommands();

	std::vector<std::any> arg;
	arg.emplace_back(std::string("toto"));
	CommandRegistry::getInstance()->executeCommand("removeVictimRelations", arg);
	
	return 0;
}
