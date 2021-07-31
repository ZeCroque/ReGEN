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

	const std::hash<std::string> hasher;
	std::vector<std::any> arg;
	arg.emplace_back(std::string("toto"));
	CommandRegistry::getInstance()->executeCommand(hasher("removeVictimRelations"), arg);
	
	return 0;
}
