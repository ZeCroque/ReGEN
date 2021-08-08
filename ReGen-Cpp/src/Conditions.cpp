#include "Conditions.h"

bool NodeCondition::conflicts(const NodeCondition& inNodeCondition) const
{
	return node->getName() == inNodeCondition.node->getName()
		&& attributeName == inNodeCondition.attributeName
		&& !compares(inNodeCondition);
}

bool NodeCondition::compares(const NodeCondition& inNodeCondition) const
{
	switch(comparisonType)
	{
		case ComparisonType::Equal:
			return inNodeCondition.attributeValue == attributeValue;
		case ComparisonType::Greater:
			return std::stoi(inNodeCondition.attributeValue) > std::stoi(attributeValue);
		case ComparisonType::Lesser:
			return std::stoi(inNodeCondition.attributeValue) < std::stoi(attributeValue);
	}
	return false;
}

#ifndef NDEBUG
void NodeCondition::print() const
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

EdgeCondition::EdgeCondition(const std::shared_ptr<Node>& inSourceNode, const std::shared_ptr<Node>& inTargetNode, const std::unordered_map<std::string, std::string> & inAttributes) : Edge(inSourceNode, inTargetNode, inAttributes)
{
}

bool EdgeCondition::conflicts(const EdgeCondition& inEdgeCondition) const
{
	return getSourceNode()->getName() == inEdgeCondition.getSourceNode()->getName()
		&& getTargetNode()->getName() == inEdgeCondition.getTargetNode()->getName()
		&& getAttributes().begin()->first != inEdgeCondition.getAttributes().begin()->first;
}

bool Conditions::conflicts(const Conditions& inCondition)
{
	return
		std::ranges::any_of(nodeConditions, [inCondition](const NodeCondition& inNodeCondition)
		{
			return std::ranges::any_of(inCondition.nodeConditions, [inNodeCondition](const NodeCondition& inOtherNodeCondition)
			{
				return inNodeCondition.conflicts(inOtherNodeCondition);
			});
		})
		|| std::ranges::any_of(edgeConditions, [inCondition](const EdgeCondition& inEdgeCondition)
		{
			return std::ranges::any_of(inCondition.edgeConditions, [inEdgeCondition](const EdgeCondition& inOtherEdgeCondition)
			{
				return inEdgeCondition.conflicts(inOtherEdgeCondition);
			});
		});
}

void Conditions::append(Conditions& inConditions)
{
	nodeConditions.splice(nodeConditions.end(), inConditions.nodeConditions);
	edgeConditions.splice(edgeConditions.end(), inConditions.edgeConditions);
}

#ifndef NDEBUG
void Conditions::print() const
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

void ConditionsBlock::append(ConditionsBlock&& inConditionsBlock)
{
	preConditions.append(inConditionsBlock.preConditions);
	postConditions.append(inConditionsBlock.postConditions);
}

#ifndef NDEBUG
void ConditionsBlock::print() const
{
	PRINTLN("PreConditions :");
	preConditions.print();
	PRINTLN("");
	PRINTLN("PostConditions :");
	postConditions.print();
}
#endif
