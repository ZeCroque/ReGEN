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
	void printStoryModifications();
	void saveDataAsDotFormat(bool inPrintWorldGraph = true, bool inPrintRules = false) const;
#endif

	[[nodiscard]] const TestLayout& getTestLayout() const;
	[[nodiscard]] const Graph& getWorldGraph() const;
	[[nodiscard]] const std::list<Rule>& getInitializationRules() const;
	[[nodiscard]] const std::list<Rule>& getRewriteRules() const;
	
private:
	static void readArguments(pugi::xml_node inParsedArguments, std::vector<std::any>& outArguments);
	void loadRules(const std::string& inRulesPath, const pugi::xml_node& inRulesListingNode, std::list<Rule>& outRulesList) const;
	
	TestLayout testLayout;	
	Graph worldGraph;
	std::list<Rule> initializationRules;
	std::list<Rule> rewriteRules;
};

#endif // DATAMANAGER_H
