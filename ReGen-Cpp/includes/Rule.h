#ifndef RULE_H
#define RULE_H

#include "Command.h"
#include "Graph.h"

struct Rule
{
	std::string name;
	Graph socialConditions;
	Graph storyConditions;
	Graph storyGraph;
	std::unordered_map<std::string, std::list<CommandData> > nodeModificationArguments;
};

#endif // RULE_H
