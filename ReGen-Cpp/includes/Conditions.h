#ifndef CONDITIONS_H
#define CONDITIONS_H

#include "Graph.h"

enum class ComparisonType
{
	Equal = 0,
	Greater = 1,
	Lesser = 2
};

struct NodeCondition
{
	std::shared_ptr<Node> node;
	std::string attributeName;
	std::string attributeValue;
	ComparisonType comparisonType;
};

using EdgeCondition = Edge;
struct Conditions
{
	std::list<NodeCondition> nodeConditions;
	std::list<EdgeCondition> edgeConditions;
};

struct ConditionsBlock
{
	Conditions preConditions;
	Conditions postConditions;
};


#endif // CONDITIONS_H
