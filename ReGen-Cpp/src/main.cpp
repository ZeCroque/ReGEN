#include "CommandsRegistry.h"
#include "CommandsDeclaration.h"
#include "DataManager.h"
#include "Graph.h"

int main()
{
	DataManager::getInstance()->init("one_story");
	declareCommands();

	const std::hash<std::string> hasher;
	std::vector<std::any> arg;
	arg.emplace_back(std::string("toto"));
	CommandRegistry::getInstance()->executeCommand(hasher("removeVictimRelations"), arg);

	pugi::xml_document testLayoutDocument;
	testLayoutDocument.load_file("./Data/SocialGraphs/Skyrim.xml");
	const Graph b(testLayoutDocument.document_element());
	b.saveAsDotFile("./Output", false);
	
	return 0;
}
