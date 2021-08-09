#include "DataManager.h"

#include <filesystem>
#include <pugixml.hpp>

#ifndef NDEBUG
#include "CommandsRegistry.h"
#endif

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
		testLayout.metricsToOptimize.emplace_back
		(
			std::pair
			{
				metricToOptimize.attribute("name").as_string(),
				metricToOptimize.attribute("weight").as_int()
			}
		);
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

void DataManager::printStoryModifications()
{
	for(const auto& rule : initializationRules)
	{
		PRINT_SEPARATOR();
		PRINT_SEPARATOR();
		PRINTLN("RULE NAME : " + rule.name);
		PRINT_SEPARATOR();
		PRINT_SEPARATOR();
		PRINTLN("");
		for(const auto& [modificationName, commandsData] : rule.nodeModificationArguments)
		{
			PRINT_SEPARATOR();
			PRINTLN("MODIFICATION NAME :" + modificationName);
			PRINT_SEPARATOR();
			for(const auto& commandData : commandsData)
			{
				CommandRegistry::getInstance()->executeCommand({}, commandData);
				PRINT_SEPARATOR();
			}
			PRINTLN("");
		}
		PRINTLN("");
		PRINTLN("");
	}
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
		for(const auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments, appliesOnce] : initializationRules)
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
		for(const auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments, appliesOnce] : rewriteRules)
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

const TestLayout& DataManager::getTestLayout() const
{
	return testLayout;
}

const Graph& DataManager::getWorldGraph() const
{
	return worldGraph;
}

const std::list<Rule>& DataManager::getInitializationRules() const
{
	return initializationRules;
}

const std::list<Rule>& DataManager::getRewriteRules() const
{
	return rewriteRules;
}

void DataManager::readArguments(const pugi::xml_node inParsedArguments, std::vector<std::any>& outArguments)
{
	outArguments.reserve(std::distance(inParsedArguments.children().begin(), inParsedArguments.children().end()));
	
	for(const auto& parsedArgument : inParsedArguments.children())
	{
		if(const auto tagName = std::string(parsedArgument.name()); tagName == "array")
		{
			std::vector<std::any> argumentArray;
			readArguments(parsedArgument, argumentArray);
			outArguments.emplace_back(argumentArray);
		}
		else if(tagName == "dict")
		{
			outArguments.emplace_back(std::string(parsedArgument.child("dict_element").attribute("value").as_string()));
		}
		else
		{
			outArguments.emplace_back(std::string(parsedArgument.attribute("value").as_string()));
		}
	}
}

void DataManager::loadRules(const std::string& inRulesPath, const pugi::xml_node& inRulesListingNode, std::list<Rule>& outRulesList) const
{
	for(const auto& ruleName : inRulesListingNode.children())
	{
		Rule rule;

		rule.name = ruleName.text().as_string();

		const auto ruleFolderPath = inRulesPath + rule.name + "/";
		const auto rulePathExtensionless = ruleFolderPath + ruleName.text().as_string();

		auto filePath = rulePathExtensionless + FILE_EXTENSION;
		assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
		pugi::xml_document ruleAttributes;
		ruleAttributes.load_file(filePath.c_str());
		rule.appliesOnce = ruleAttributes.document_element().attribute("applyonce").as_bool();

		filePath = rulePathExtensionless + "_Social_Condition" + FILE_EXTENSION; 
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

		for(auto& storyNode : storyGraph.child("nodes").children("node"))
		{
			if(const auto modificationName = std::string(storyNode.attribute("modification").as_string()); modificationName != "None")
			{
				pugi::xml_document modificationDocument;
				filePath = ruleFolderPath + "Modifications/";
				filePath += modificationName;
				assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
				modificationDocument.load_file(filePath.c_str());
				const auto modificationNode = modificationDocument.document_element();

				std::list<CommandData> commandsData;
			
				for(const auto& command : modificationNode.children("Method"))
				{
					CommandData commandData;
					commandData.name = command.attribute("name").as_string();

					commandData.caller = command.attribute("self").as_string();
					
					std::vector<std::any> arguments;
					const auto parsedArguments = command.child("args");
					readArguments(parsedArguments, arguments);
					commandData.arguments = arguments;

					commandsData.emplace_back(commandData);
				}

				for(const auto& command : modificationNode.children("Attribute"))
				{
					std::vector<std::any> arguments;
					arguments.reserve(2);
					arguments.emplace_back(std::string(command.attribute("key").as_string()));
					arguments.emplace_back(std::string(command.attribute("value").as_string()));
					
					commandsData.emplace_back
					(
						CommandData
						{
							"modify_attribute",
							command.attribute("name").as_string(),
							arguments
						}
					);
				}

				rule.nodeModificationArguments.insert({storyNode.attribute("name").as_string(), commandsData});
			}
		}

		outRulesList.emplace_back(rule);
	}
}
