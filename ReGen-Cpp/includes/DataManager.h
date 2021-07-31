#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Graph.h"
#include "Rule.h"
#include "Singleton.h"
#include "TestLayout.h"

class DataManager final : public Singleton<DataManager>
{
	friend class Singleton<DataManager>;

public:
	void init(const char* inTestLayoutName, const char* inContentPath = "./Data/");
#ifndef NDEBUG
	void printTestLayout() const;
	void saveDataAsDotFormat(bool inPrintWorldGraph = true, bool inPrintRules = false) const;
#endif

private:
	void loadRules(const std::string& inRulesPath, const pugi::xml_node& inRulesListingNode, std::list<Rule>& outRulesList) const;
	
	TestLayout testLayout;	
	Graph worldGraph;
	std::list<Rule> initializationRules;
	std::list<Rule> rewriteRules;
};

#endif // DATAMANAGER_H
