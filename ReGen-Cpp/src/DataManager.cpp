#include "DataManager.h"

#include <filesystem>

constexpr auto FILE_EXTENSION = ".xml";

void DataManager::init(const char* inTestLayoutName, const char* inContentPath)
{
	const auto contentPath = std::string(inContentPath);
	const auto testLayoutPath = contentPath + "TestLayout/" + inTestLayoutName + FILE_EXTENSION;
	assert(std::filesystem::exists(testLayoutPath) && std::filesystem::is_regular_file(testLayoutPath));
	pugi::xml_document testLayoutDocument;
	testLayout.load_file(testLayoutPath.c_str());
	
	const auto worldGraphPath = contentPath + "SocialGraphs/" + testLayout.document_element().child("socialgraph").text().as_string() + FILE_EXTENSION;
	assert(std::filesystem::exists(worldGraphPath) && std::filesystem::is_regular_file(worldGraphPath));
	worldGraph.loadFromXml(worldGraphPath);

	const auto rulesPath = contentPath + "Rules/";
	loadRules(rulesPath + "InitializationRules/", testLayout.document_element().child("initializationrules"), initializationRules);
	loadRules(rulesPath + "RewriteRules/", testLayout.document_element().child("rewriterules"), rewriteRules);	
}

#ifndef NDEBUG
void DataManager::saveDataAsDotFormat(const bool inPrintWorldGraph, const bool inPrintRules)
{
	std::string color = "lightblue4";
	std::string fontColor = "lightblue4";
	std::string baseOutputPath = "./Output/SocialGraph";

	if(inPrintWorldGraph)
	{
		worldGraph.saveAsDotFile(color, fontColor, baseOutputPath);
	}

	if(inPrintRules)
	{
		color = "ivory4";
		fontColor = "ivory4";
		
		char i = '0';
		baseOutputPath = "./Output/InitRules/";
		for(auto& [socialConditions, storyConditions, storyGraph, nodeModificationArguments] : initializationRules)
		{
			auto outputPath(baseOutputPath);
			outputPath.push_back(i);
			storyGraph.saveAsDotFile(color, fontColor, outputPath);
			socialConditions.saveAsDotFile(color, fontColor, outputPath);
			storyConditions.saveAsDotFile(color, fontColor, outputPath);
			++i;
		}

		i = '0';
		baseOutputPath = "./Output/RewriteRules/";
		for(auto& [socialConditions, storyConditions, storyGraph, nodeModificationArguments] : rewriteRules)
		{
			auto outputPath(baseOutputPath);
			outputPath.push_back(i);
			storyGraph.saveAsDotFile(color, fontColor, outputPath);
			socialConditions.saveAsDotFile(color, fontColor, outputPath);
			storyConditions.saveAsDotFile(color, fontColor, outputPath);
			++i;
		}
	}
}
#endif

void DataManager::loadRules(const std::string& inRulesPath, const pugi::xml_node& inRulesListingNode, std::list<Rule>& outRulesList) const
{
	for(const auto& ruleName : inRulesListingNode.children())
	{
		const auto ruleFolderPath = inRulesPath + ruleName.text().as_string() + "/";
		const auto rulePathExtensionless = ruleFolderPath + ruleName.text().as_string();

		Rule rule;
		
		auto filePath = rulePathExtensionless + "_Social_Condition" + FILE_EXTENSION; 
		assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
		rule.socialConditions.loadFromXml(filePath);

		filePath = rulePathExtensionless + "_Story_Graph_Condition" + FILE_EXTENSION; 
		if(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
		{
			rule.storyConditions.loadFromXml(filePath);
		}

		filePath = rulePathExtensionless + "_Story_Graph" + FILE_EXTENSION; 
		assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
		pugi::xml_document document;
		document.load_file(filePath.c_str());
		const auto storyGraph = document.document_element();
		rule.storyGraph.loadFromXml(storyGraph);

		/*for(auto& storyNode : storyGraph.child("nodes").children("node"))
		{
			if(const auto modificationName = std::string(storyNode.attribute("modification").as_string()); modificationName != "None")
			{
				filePath = ruleFolderPath + "/Modifications/";
				filePath += modificationName;
				assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
				document.load_file(filePath.c_str());
				const auto modification = document.document_element();
				//TODO load modifications
			}
		}*/

		outRulesList.emplace_back(rule);
	}
}
