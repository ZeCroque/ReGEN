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
	worldGraph.load_file(worldGraphPath.c_str());

	initializationRules.append_child("rules");
	loadRules(contentPath);
	
	rewriteRules.append_child("rules");
	loadRules(contentPath, true);
}

void DataManager::loadRules(const std::string& inContentPath, const bool inLoadRewriteRules) const
{
	auto& rules = inLoadRewriteRules ? rewriteRules : initializationRules;
	const auto rulesFolderPath = inContentPath + "Rules/" + (inLoadRewriteRules ? "RewriteRules/" : "InitializationRules/");
	for(const auto& ruleName : testLayout.document_element().child(inLoadRewriteRules ? "rewriterules" : "initializationrules").children())
	{
		const auto ruleFolderPath = rulesFolderPath + ruleName.text().as_string() + "/";
		const auto rulePathExtensionless = ruleFolderPath + ruleName.text().as_string();
		pugi::xml_document document;
		
		auto filePath = rulePathExtensionless + FILE_EXTENSION;
		assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
		document.load_file(filePath.c_str());
		auto rule = rules.document_element().append_copy(document.document_element());

		filePath = rulePathExtensionless + "_Social_Condition" + FILE_EXTENSION; 
		assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
		document.load_file(filePath.c_str());
		auto socialConditionNode = rule.child("socialcondition");
		socialConditionNode.remove_children();
		socialConditionNode.append_copy(document.document_element());

		filePath = rulePathExtensionless + "_Story_Modification" + FILE_EXTENSION; 
		assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
		document.load_file(filePath.c_str());
		auto storyModificationNode = rule.child("storymodification");
		storyModificationNode.remove_children();
		auto storyModification = storyModificationNode.append_copy(document.document_element());

		for(auto& storyNode : storyModification.child("nodes").children("node"))
		{
			if(const auto modificationName = std::string(storyNode.attribute("modification").as_string()); modificationName != "None")
			{
				filePath = ruleFolderPath + "/Modifications/";
				filePath += modificationName;
				assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
				document.load_file(filePath.c_str());
				storyNode.append_copy(document.document_element());
			}
			else
			{
				storyNode.append_child("Modification");
			}
			storyNode.remove_attribute("modification");
		}

		if(inLoadRewriteRules)
		{
			filePath = rulePathExtensionless + "_Story_Condition" + FILE_EXTENSION; 
			assert(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath));
			document.load_file(filePath.c_str());
			auto storyConditionNode = rule.child("storycondition");
			storyConditionNode.remove_children();
			storyConditionNode.append_copy(document.document_element());
		}
	}
}
