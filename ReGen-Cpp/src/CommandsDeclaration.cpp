#include "CommandsDeclaration.h"

#include "CommandsRegistry.h"

void declareCommands()
{
	const auto* commandRegistry = CommandRegistry::getInstance();

	auto changeAffinityTowardTarget = [](const std::shared_ptr<Node> inCaller, const std::list<std::string>& inRequiredRelations, const std::shared_ptr<Node> inTarget, const std::string& inNewRelationName, const std::string& inTemplatedReason) -> ConditionsBlock  // NOLINT(performance-unnecessary-value-param)
	{	
		const auto& reason = inTemplatedReason + inCaller->getName();

		ConditionsBlock result;
		for(const auto& edge : inCaller->getIncomingEdges())
		{
			for(auto& requiredRelation : inRequiredRelations)
			{
				if(auto foundAttribute = edge->getAttributes().find<std::string>(requiredRelation); foundAttribute != edge->getAttributes().end())
				{
					if(const auto sourceNode = edge->getSourceNode(); !(sourceNode->getName() == inTarget->getName()))
					{
						result.preConditions.edgeConditions.emplace_back(EdgeCondition{sourceNode, inCaller, {{foundAttribute->first, foundAttribute->second}}});
						result.postConditions.edgeConditions.emplace_back(EdgeCondition{sourceNode, inTarget, {{inNewRelationName, reason}}});
					}
				}
			}
		}
		return result;
	};
	
	/*******************************************************************************
	 * Updates relations of victim's friends toward its murderer
	 *
	 * @caller ref victim
	 * @param ref murderer
	 ******************************************************************************/
	commandRegistry->registerCommand("murder", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]))->getName() + " murdered " + inCast.at(inCommandData.caller)->getName());
		},
		[changeAffinityTowardTarget](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);
			auto result = changeAffinityTowardTarget(inCast.at(inCommandData.caller), {"Loves","Friends"}, inCast.at(std::any_cast<std::string>(inCommandData.arguments[0])), "Hates", "Murder_of_");
			result.append(changeAffinityTowardTarget(inCast.at(inCommandData.caller), {"Hates","Enemies"}, inCast.at(std::any_cast<std::string>(inCommandData.arguments[0])), "Friends", "Murder_of_"));
			return result;
		}
	});

	/*******************************************************************************
	 * Remove victim relations upon its death
	 *
	 * @caller ref victim
	 ******************************************************************************/
	commandRegistry->registerCommand("die", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.empty());			
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(inCommandData.caller)->getName() + " died");
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.empty());
			return{};
		}
	});

	/*******************************************************************************
	 * Adds a new relation between people whom had the specified relationship with caller and target
	 *
	 * @caller ref entity
	 * @param array<string> relationToVictim
	 * @param string newRelationForTarget
	 * @param ref target
	 * @param str reason
	 ******************************************************************************/
	commandRegistry->registerCommand("set_other_nodes_relations", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 4);

			const auto callerName = inCast.at(inCommandData.caller)->getName();
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN("People whom had the following relationship with " + callerName + ":");
			const auto& edgeAttribute = std::any_cast<std::vector<std::any> >(inCommandData.arguments[0]);
			assert(edgeAttribute.size() == 2);
			PRINTLN("\t- " + std::any_cast<std::string>(edgeAttribute[0]) + " : " + std::any_cast<std::string>(edgeAttribute[1]));
			PRINTLN("Now have the relationship \"" + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" with " + inCast.at(std::any_cast<std::string>(inCommandData.arguments[2]))->getName() + " because of " + std::any_cast<std::string>(inCommandData.arguments[3]) + callerName);
		},
		[changeAffinityTowardTarget](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 4);
			std::list<std::string> requiredRelations;
			for(auto& element : std::any_cast<std::vector<std::any> >(inCommandData.arguments[0]))
			{
				requiredRelations.emplace_back(std::any_cast<std::string>(element));
			}
			return changeAffinityTowardTarget(inCast.at(inCommandData.caller), requiredRelations, inCast.at(std::any_cast<std::string>(inCommandData.arguments[2])), std::any_cast<std::string>(inCommandData.arguments[1]), std::any_cast<std::string>(inCommandData.arguments[3]));
		}
	});

	/*******************************************************************************
	 * Moves the player to entity's location
	 *
	 * @caller ref player
	 * @param ref entity
	 ******************************************************************************/
	commandRegistry->registerCommand("move_player_to_node", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);		
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(inCommandData.caller)->getName() + " moved to " + inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]))->getName() + "'s location");
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);



			return
			{
				{},
				{
					{},
				{EdgeCondition{inCast.at(inCommandData.caller), inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]))->getTargetNodeFromOutgoingEdgeWithAttribute({"Lives", "N/A"}), {{"Currently_In", "N/A"}}}}
				}
			};
		}
	});

	/*******************************************************************************
	 * Kills the target enemy
	 *
	 * @caller ref player
	 * @param ref enemy
	 ******************************************************************************/
	commandRegistry->registerCommand("killed_enemy", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);			
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(inCommandData.caller)->getName() + " killed " + inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]))->getName());
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);

			const auto enemy = inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]));
			return
			{
				{
					{NodeCondition{enemy, "number", "0", ComparisonType::Greater}},
					{}
				},
				{
					{NodeCondition{enemy, "number", "0", ComparisonType::Equal}},
					{}
				}

			};
		}
	});

	/*******************************************************************************
	 * Change ownership of an object
	 *
	 * @caller ref object
	 * @param ref newOwner
	 * @param string newObjectStatus
	 * @param string reason
	 ******************************************************************************/
	commandRegistry->registerCommand("new_owner", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 3);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(inCommandData.caller)->getName() + " is now owned by " + inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]))->getName() + " and has status \""  + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" because of "  + std::any_cast<std::string>(inCommandData.arguments[2]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 3);

			const auto& caller = inCast.at(inCommandData.caller);
			const auto previousOwner = caller->getSourceNodeFromIncomingEdgeWithAttribute({"Owns", "N/A"});
			const auto previousLocation = previousOwner->getTargetNodeFromOutgoingEdgeWithAttribute({"Lives", "N/A"});

			return
			{
				{
					{},
					{EdgeCondition{caller, previousLocation, {{"Currently_In", "N/A"}}}}
				},
				{
					{NodeCondition{caller, "status", std::any_cast<std::string>(inCommandData.arguments[1]), ComparisonType::Equal}},
					{EdgeCondition{inCast.at(std::any_cast<std::string>(inCommandData.arguments[0])), caller, {{"Owns", std::any_cast<std::string>(inCommandData.arguments[2])}}}}
				}
			};
		}
	});

	/*******************************************************************************
	 * Adds a relation between caller and target
	 *
	 * @caller ref entity
	 * @param ref target
	 * @param string relationAttributeName
	 * @param string relationAttributeValue
	 ******************************************************************************/
	commandRegistry->registerCommand("add_edge", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 3);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(inCommandData.caller)->getName() + " now has \"" + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" relationship with " + inCast.at(std::any_cast<std::string>(inCommandData.arguments[0]))->getName() + " because of " + std::any_cast<std::string>(inCommandData.arguments[2]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 3);
			return
			{
				{},
				{
					{},
					{EdgeCondition{inCast.at(inCommandData.caller), inCast.at(std::any_cast<std::string>(inCommandData.arguments[0])), {{std::any_cast<std::string>(inCommandData.arguments[1]), std::any_cast<std::string>(inCommandData.arguments[2])}}}}
				}

			};
		}
	});

	/*******************************************************************************
	 * Modifies an attribute of the caller
	 *
	 * @caller ref entity
	 * @param string attributeName
	 * @param string attributeValue
	 ******************************************************************************/
	commandRegistry->registerCommand("modify_attribute", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 2);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCast.at(inCommandData.caller)->getName() + "'s \"" + std::any_cast<std::string>(inCommandData.arguments[0]) + "\" attribute is now equal to " + std::any_cast<std::string>(inCommandData.arguments[1]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 2);
			return
			{
				{},
				{
					{NodeCondition{inCast.at(inCommandData.caller), std::any_cast<std::string>(inCommandData.arguments[0]), std::any_cast<std::string>(inCommandData.arguments[1]), ComparisonType::Equal}},
					{}
				}
			};
		}
	});
}
