#ifndef RULE_H
#define RULE_H

#include "CommandsRegistry.h"
#include "Graph.h"

struct Rule
{
	Graph socialConditions;
	Graph storyConditions;
	Graph storyGraph;
	std::unordered_map<std::string, std::vector<CommandArguments> > nodeModificationArguments;
};

#endif // RULE_H
