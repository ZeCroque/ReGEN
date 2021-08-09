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
	static void getPossibleRules(const std::list<Rule>& inRuleSet, const Graph& inGraph, std::unordered_map<Rule, std::list<std::list<std::shared_ptr<class Node> > >, struct RuleHashFunction>& outPossibleRules);
	static void rewriteStory(const Graph& inStory, const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, Graph& outStory, int inRewritesCount = 0);
#ifndef NDEBUG
	static void printNodeConditions(const std::string& inNodeName, std::shared_ptr<struct ConditionsBlock> inConditionsBlock);
#endif
	static void createNodeConditions(const std::unordered_map<std::string, std::list<struct CommandData>>& inRuleCommandsData, const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, std::shared_ptr<Node> inNode);

	static std::default_random_engine randomEngine;
	std::string questName;
};

#endif // SCHEDULER_H
