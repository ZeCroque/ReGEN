#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <pugixml.hpp>

#include "Graph.h"
#include "Rule.h"
#include "Singleton.h"

class DataManager final : public Singleton<DataManager>
{
	friend class Singleton<DataManager>;

public:
	void init(const char* inTestLayoutName, const char* inContentPath = "./Data/");
#ifndef NDEBUG
	void saveDataAsDotFormat(bool inPrintWorldGraph = true, bool inPrintRules = false);
#endif

private:
	void loadRules(const std::string& inRulesPath, const pugi::xml_node& inRulesListingNode, std::list<Rule>& outRulesList) const;
	
	pugi::xml_document testLayout; //TODO extract data to newly created structure
	
	Graph worldGraph;
	std::list<Rule> initializationRules;
	std::list<Rule> rewriteRules;
};

#endif // DATAMANAGER_H
