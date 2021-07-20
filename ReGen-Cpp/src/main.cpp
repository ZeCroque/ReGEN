#include "pugixml.hpp"
#include <iostream>

int main()
{
    pugi::xml_document doc;
	
    doc.load_file("Data/Rules/InitializationRules/Assassinate/Assassinate_Social_Condition.xml");
        
    for (const pugi::xml_node& node : doc.child("graph").child("nodes").children("node"))
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