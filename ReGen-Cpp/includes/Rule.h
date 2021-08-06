#ifndef RULE_H
#define RULE_H

#include "Command.h"
#include "Graph.h"

struct Rule
{
	friend bool operator==(const Rule& inFirstRule, const Rule& inSecondRule);

	std::string name;
	Graph socialConditions;
	Graph storyConditions;
	Graph storyGraph;
	std::unordered_map<std::string, std::list<CommandData> > nodeModificationArguments;
};

inline bool operator==(const Rule& inFirstRule, const Rule& inSecondRule)
{
	return inFirstRule.name == inSecondRule.name;
}

struct RuleHashFunction
{
    size_t operator()(const Rule& inRule) const
    {
    	const std::hash<std::string> hasher;
        return hasher(inRule.name);
    }
};


#endif // RULE_H
