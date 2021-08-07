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

#ifndef NDEBUG
	void print() const;
#endif
};

#ifndef NDEBUG
inline void NodeCondition::print() const
{
	std::string comparisonString;
	switch(comparisonType)
	{
		case ComparisonType::Equal:
			comparisonString = "=";
			break;
		case ComparisonType::Greater:
			comparisonString = ">";
			break;
		case ComparisonType::Lesser:
			comparisonString = "<";
			break;
	}
	PRINTLN(node->getName() + "_" + attributeName + "_" + comparisonString + "_" + attributeValue);
}
#endif

using EdgeCondition = Edge;
struct Conditions
{
	std::list<NodeCondition> nodeConditions;
	std::list<EdgeCondition> edgeConditions;

	void append(Conditions& inConditions);

#ifndef NDEBUG
	void print() const;
#endif
};

inline void Conditions::append(Conditions& inConditions)
{
	nodeConditions.splice(nodeConditions.end(), inConditions.nodeConditions);
	edgeConditions.splice(edgeConditions.end(), inConditions.edgeConditions);
}

#ifndef NDEBUG
inline void Conditions::print() const
{
	for(const auto& nodeCondition : nodeConditions)
	{
		nodeCondition.print();
	}

	for(const auto& edgeCondition : edgeConditions)
	{
		edgeCondition.print();
	}
}
#endif

struct ConditionsBlock
{
	Conditions preConditions;
	Conditions postConditions;

	void append(ConditionsBlock&& inConditionsBlock);
#ifndef NDEBUG
	void print() const;
#endif
};

inline void ConditionsBlock::append(ConditionsBlock&& inConditionsBlock)
{
	preConditions.append(inConditionsBlock.preConditions);
	postConditions.append(inConditionsBlock.postConditions);
}

#ifndef NDEBUG
inline void ConditionsBlock::print() const
{
	PRINTLN("PreConditions :");
	preConditions.print();
	PRINTLN("");
	PRINTLN("PostConditions :");
	postConditions.print();
}
#endif

#endif // CONDITIONS_H
