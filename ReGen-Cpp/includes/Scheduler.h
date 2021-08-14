#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <random>

struct Rule;
class Graph;

//TODO threading
class Scheduler 
{
public:
	explicit Scheduler(std::string inQuestName);
	void run();

private:
	static void getPossibleRules(const std::list<Rule>& inRuleSet, const Graph& inGraph, const std::unordered_map<std::string, int>& inRuleUsages, std::list<std::pair<Rule, std::list<std::list<std::shared_ptr<class Node> > > > >& outPossibleRules);
	static bool rewriteStory(const Graph& inStory, const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, std::unordered_map<std::string, int>& inRuleUsages, Graph& outStory);
#ifndef NDEBUG
	static void printNodeConditions(const std::string& inNodeName, std::shared_ptr<struct ConditionsBlock> inConditionsBlock);
#endif
	static void createNodeConditions(const std::unordered_map<std::string, std::list<struct CommandData>>& inRuleCommandsData, const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, std::shared_ptr<Node> inNode);

	static std::default_random_engine randomEngine;
	static std::unordered_map<std::string, int> rulesUsages;
	std::string questName;
};

#endif // SCHEDULER_H
