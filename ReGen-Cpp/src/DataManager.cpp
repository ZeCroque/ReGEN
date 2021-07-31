#include "DataManager.h"

#include <filesystem>
#include <pugixml.hpp>

constexpr auto FILE_EXTENSION = ".xml";

void DataManager::init(const char* inTestLayoutName, const char* inContentPath)
{
	const auto contentPath = std::string(inContentPath);
	const auto testLayoutPath = contentPath + "TestLayout/" + inTestLayoutName + FILE_EXTENSION;
	assert(std::filesystem::exists(testLayoutPath) && std::filesystem::is_regular_file(testLayoutPath));
	pugi::xml_document testLayoutDocument;
	testLayoutDocument.load_file(testLayoutPath.c_str());
	const auto testLayoutNode = testLayoutDocument.document_element();
	testLayout.numStoryToGenerate = testLayoutNode.child("numstoriestogenerate").text().as_int();
	testLayout.maxNumberOfRewrites = testLayoutNode.child("maxnumberofrewrites").text().as_int();

	for(const auto& metricToOptimize : testLayoutNode.child("metricstooptimize").children("metric"))
	{
		testLayout.metricsToOptimize.emplace_back(std::pair{
			metricToOptimize.attribute("name").as_string(),
			metricToOptimize.attribute("weight").as_int()
		});
	}

	for(const auto& metricToAnalyze : testLayoutNode.child("metricstoanalyze").children("metric"))
	{
		testLayout.metricsToAnalyze.emplace_back(metricToAnalyze.attribute("name").as_string());
	}
	
	const auto worldGraphPath = contentPath + "SocialGraphs/" + testLayoutNode.child("socialgraph").text().as_string() + FILE_EXTENSION;
	assert(std::filesystem::exists(worldGraphPath) && std::filesystem::is_regular_file(worldGraphPath));
	worldGraph.loadFromXml(worldGraphPath);

	const auto rulesPath = contentPath + "Rules/";
	loadRules(rulesPath + "InitializationRules/", testLayoutNode.child("initializationrules"), initializationRules);
	loadRules(rulesPath + "RewriteRules/", testLayoutNode.child("rewriterules"), rewriteRules);	
}

#ifndef NDEBUG
void DataManager::printTestLayout() const
{
	PRINTLN("NUM STORY TO GENERATE : " + std::to_string(testLayout.numStoryToGenerate));
	PRINTLN("MAX NUMBER OF REWRITE :" + std::to_string(testLayout.maxNumberOfRewrites));
	PRINTLN("");
	
	PRINT_SEPARATOR();
	PRINTLN("METRICS TO OPTIMIZE");
	PRINT_SEPARATOR();
	for(const auto& [name, weight] : testLayout.metricsToOptimize)
	{
		PRINTLN(name + " : " + std::to_string(weight));
	}
	PRINTLN("");
	
	PRINT_SEPARATOR();
	PRINTLN("METRICS TO ANALYZE");
	PRINT_SEPARATOR();
	for(const auto& name : testLayout.metricsToAnalyze)
	{
		PRINTLN(name);
	}
	PRINTLN("");
}

void DataManager::saveDataAsDotFormat(const bool inPrintWorldGraph, const bool inPrintRules) const
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
		for(const auto& [socialConditions, storyConditions, storyGraph, nodeModificationArguments] : initializationRules)
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
		for(const auto& [socialConditions, storyConditions, storyGraph, nodeModificationArguments] : rewriteRules)
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
