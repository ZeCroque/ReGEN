#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <random>

struct Rule;

//TODO threading
class Scheduler 
{
public:
	explicit Scheduler(const class Graph& inGraph);
	void run();

private:
	static void getPossibleRules(const std::list<Rule>& inRuleSet, const Graph& inGraph, std::unordered_map<Rule, std::list<std::list<std::shared_ptr<class Node> > >, struct RuleHashFunction>& outPossibleRules);

	std::default_random_engine randomEngine;
	const Graph& graph;
};

#endif // SCHEDULER_H
