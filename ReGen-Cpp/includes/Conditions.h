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

	[[nodiscard]] bool conflicts(const NodeCondition& inNodeCondition) const;
	[[nodiscard]] bool compares(const NodeCondition& inNodeCondition) const;

#ifndef NDEBUG
	void print() const;
#endif
};

class EdgeCondition : public Edge
{
public:
	EdgeCondition(const std::shared_ptr<Node>& inSourceNode, const std::shared_ptr<Node>& inTargetNode, const std::unordered_map<std::string, std::string> & inAttributes);

	[[nodiscard]] bool conflicts(const EdgeCondition& inEdgeCondition) const;
};

struct Conditions
{
	std::list<NodeCondition> nodeConditions;
	std::list<EdgeCondition> edgeConditions;

	[[nodiscard]] bool conflicts(const Conditions& inCondition);
	void append(Conditions& inConditions);

#ifndef NDEBUG
	void print() const;
#endif
};

struct ConditionsBlock
{
	Conditions preConditions;
	Conditions postConditions;

	void append(ConditionsBlock&& inConditionsBlock);
#ifndef NDEBUG
	void print() const;
#endif
};

#endif // CONDITIONS_H
