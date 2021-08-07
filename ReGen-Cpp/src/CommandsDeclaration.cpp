#include "CommandsDeclaration.h"

#include "CommandsRegistry.h"

void declareCommands()
{
	auto* commandRegistry = CommandRegistry::getInstance();

	auto changeAffinityTowardTarget = [](const std::shared_ptr<Node> inCaller, std::list<std::pair<std::string, std::string> >&& inRequiredRelations, const std::shared_ptr<Node> inTarget, const std::string& inNewRelationName, const std::string& inTemplatedReason) -> ConditionsBlock  // NOLINT(performance-unnecessary-value-param)
	{	
		const auto& reason = inTemplatedReason + inCaller->getName();

		ConditionsBlock result;
		for(const auto& edge : inCaller->getIncomingEdges())
		{
			for(auto& [relationName, relationReason] : inRequiredRelations)
			{
				if(edge->getAttributes().contains(relationName))
				{
					if(const auto sourceNode = edge->getSourceNode(); !(sourceNode->getName() == inTarget->getName()))
					{
						result.preConditions.edgeConditions.emplace_back(EdgeCondition{sourceNode, inCaller, {{relationName, relationReason}}});
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
			PRINTLN(std::any_cast<std::string>(inCommandData.arguments[0]) + " murdered " + inCommandData.caller);
		},
		[changeAffinityTowardTarget](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);
			auto result = changeAffinityTowardTarget(inCast.at(inCommandData.caller), {{"Loves","Friends"}}, inCast.at(std::any_cast<std::string>(inCommandData.arguments[0])), "Hates", "Murder_of_");

			auto [preConditions, postConditions] = changeAffinityTowardTarget(inCast.at(inCommandData.caller), {{"Hates","Enemies"}}, inCast.at(std::any_cast<std::string>(inCommandData.arguments[0])), "Friends", "Murder_of_");
			result.preConditions.edgeConditions.splice(result.preConditions.edgeConditions.end(), preConditions.edgeConditions);
			result.postConditions.edgeConditions.splice(result.postConditions.edgeConditions.end(), postConditions.edgeConditions);
			
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
			PRINTLN(inCommandData.caller + " died");
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
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN("People whom had the following relationship with " + inCommandData.caller + ":");
			const auto& edgeAttribute = std::any_cast<std::vector<std::any> >(inCommandData.arguments[0]);
			assert(edgeAttribute.size() == 2);
			PRINTLN("\t- " + std::any_cast<std::string>(edgeAttribute[0]) + " : " + std::any_cast<std::string>(edgeAttribute[1]));
			PRINTLN("Now have the relationship \"" + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" with " + std::any_cast<std::string>(inCommandData.arguments[2]) + " because of " + std::any_cast<std::string>(inCommandData.arguments[3]));
		},
		[changeAffinityTowardTarget](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 4);
			const auto& edgeAttribute = std::any_cast<std::vector<std::any> >(inCommandData.arguments[0]);
			assert(edgeAttribute.size() == 2);
			return changeAffinityTowardTarget(inCast.at(inCommandData.caller), {{std::any_cast<std::string>(edgeAttribute[0]), std::any_cast<std::string>(edgeAttribute[1])}}, inCast.at(std::any_cast<std::string>(inCommandData.arguments[2])), std::any_cast<std::string>(inCommandData.arguments[1]), std::any_cast<std::string>(inCommandData.arguments[3]));
		}
	});

	/*******************************************************************************
	 * Moves the player to location
	 *
	 * @caller ref player
	 * @param ref location
	 ******************************************************************************/
	commandRegistry->registerCommand("move_player", new Command
	{
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);		
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " moved to " + std::any_cast<std::string>(inCommandData.arguments[0]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				{},
				{}

			};
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
			PRINTLN(inCommandData.caller + " moved to " + std::any_cast<std::string>(inCommandData.arguments[0]) + "'s location");
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				{},
				{}

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
			PRINTLN(inCommandData.caller + " killed " + std::any_cast<std::string>(inCommandData.arguments[0]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				{},
				{}

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
			PRINTLN(inCommandData.caller + " is now owned by " + std::any_cast<std::string>(inCommandData.arguments[0]) + " and has status \""  + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" because of "  + std::any_cast<std::string>(inCommandData.arguments[2]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 3);
			return
			{
				{},
				{}

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
			PRINTLN(inCommandData.caller + " now has \"" + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" relationship with " + std::any_cast<std::string>(inCommandData.arguments[0]) + " because of " + std::any_cast<std::string>(inCommandData.arguments[2]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 3);
			return
			{
				{},
				{}

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
			PRINTLN(inCommandData.caller + "'s \"" + std::any_cast<std::string>(inCommandData.arguments[0]) + "\" attribute is now equal to " + std::any_cast<std::string>(inCommandData.arguments[1]));
		},
		[](const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const CommandData& inCommandData) -> ConditionsBlock
		{
			assert(inCommandData.arguments.size() == 2);
			return
			{
				{},
				{}
			};
		}
	});
	//TODO Add proper implementation and conditions
}
