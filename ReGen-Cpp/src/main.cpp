#include "CommandsRegistry.h"
#include "CommandsDeclaration.h"
#include "DataManager.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();

	const std::hash<std::string> hasher;
	std::vector<std::any> arg;
	arg.emplace_back(std::string("toto"));
	CommandRegistry::getInstance()->executeCommand(hasher("removeVictimRelations"), arg);
	
	return 0;
}
