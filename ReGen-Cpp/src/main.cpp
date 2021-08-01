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

	CommandData cmd;
	cmd.arguments.emplace_back(std::string("toto"));
	CommandRegistry::getInstance()->executeCommand("removeVictimRelations", cmd);
	
	return 0;
}
