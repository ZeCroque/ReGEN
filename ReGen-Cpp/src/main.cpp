#include "pugixml.hpp"
#include <iostream>
#include <filesystem>

#include "CommandsRegistry.h"
#include "CommandsDeclaration.h"

int main()
{
	declareCommands();
	
	const std::hash<std::string> hasher;
	std::vector<std::any> arg;
	arg.emplace_back(std::string("toto"));
	CommandManager::getInstance()->executeCommand(hasher("removeVictimRelations"), arg);
	
    //assert(std::filesystem::exists(prefabsPath) && std::filesystem::is_directory(prefabsPath));
	for (const auto& entry : std::filesystem::recursive_directory_iterator("./Data/Rules/InitializationRules/"))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".xml")
		{
			pugi::xml_document doc;

			doc.load_file(entry.path().c_str());

		    for (const pugi::xml_node& node : doc.child("graph").child("nodes").children("node"))
		    {
		        std::cout << "Name : \"" << node.attribute("name").as_string() << "\"" << std::endl;

    			for(const pugi::xml_node& condition : node.children("attr"))
    			{
    				std::cout << "Condition : \"" << condition.attribute("name").as_string() << "\" of type \"" << condition.attribute("type").as_string() << "\" equals \"" << condition.child("value").text().as_string() << "\"" << std::endl;
    			}
		        std::cout << "================================" << std::endl;
		    }
		}
	}

	pugi::xml_document doc2;
	auto quest = doc2.append_child("graph");
	auto nodes = quest.append_child("nodes");
	auto questNode1 = nodes.append_child("node");
	auto questName = questNode1.append_attribute("name");
	questName.set_value("GO_TO_TRUC");

	for (const pugi::xml_node& node : doc2.child("graph").child("nodes").children("node"))
    {
        std::cout << "Name : \"" << node.attribute("name").as_string() << "\"" << std::endl;

    	for(const pugi::xml_node& condition : node.children("attr"))
    	{
    		std::cout << "Condition : \"" << condition.attribute("name").as_string() << "\" of type \"" << condition.attribute("type").as_string() << "\" equals \"" << condition.child("value").text().as_string() << "\"" << std::endl;
    	}
        std::cout << "================================" << std::endl;
    }
	
	
	return 0;
}
