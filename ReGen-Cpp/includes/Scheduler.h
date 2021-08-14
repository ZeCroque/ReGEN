#ifndef SCHEDULER_H
#define SCHEDULER_H

#define NOMINMAX
#include <random>
#include <windows.h>

struct Rule;
class Graph;

//TODO threading
class Scheduler 
{
public:
	explicit Scheduler(std::string inQuestName);
	void run();

	static void init();
private:
	static void getPossibleRules(const std::list<Rule>& inRuleSet, const Graph& inGraph, const std::unordered_map<std::string, int>& inRuleUsages, std::unordered_map<Rule, std::list<std::list<std::shared_ptr<class Node> > >, struct RuleHashFunction>& outPossibleRules);
	static bool rewriteStory(const Graph& inStory, const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, std::unordered_map<std::string, int>& inRuleUsages, Graph& outStory);
#ifndef NDEBUG
	static void printNodeConditions(const std::string& inNodeName, std::shared_ptr<struct ConditionsBlock> inConditionsBlock);
#endif
	static void createNodeConditions(const std::unordered_map<std::string, std::list<struct CommandData>>& inRuleCommandsData, const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, std::shared_ptr<Node> inNode);
	static std::default_random_engine randomEngine;

	std::string questName;
public:
	static void updateProfiler();
	static std::unordered_map<std::string, int> rulesUsages;
	static size_t maxVirtualMem;
	static size_t maxPhysicalMem;
	static size_t minVirtualMem;
	static size_t minPhysicalMem;
	static size_t virtualMemSum;
	static size_t physMemSum;
	static size_t measureCount;
	static double maxCpuUsage;
	static double minCpuUsage;
	static double cpuUsageSum;
private:
	static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	static int numProcessors;
	static HANDLE self;
};

#endif // SCHEDULER_H
