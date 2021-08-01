#include "CommandsDeclaration.h"

#include "CommandsRegistry.h"

void declareCommands()
{
	auto* commandRegistry = CommandRegistry::getInstance();

	/*******************************************************************************
	 * Updates relations of victim's friends toward its murderer
	 *
	 * @caller ref victim
	 * @param ref murderer
	 ******************************************************************************/
	commandRegistry->registerCommand("murder", new Command
	{
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(std::any_cast<std::string>(inCommandData.arguments[0]) + " murdered " + inCommandData.caller);
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

			};
		}
	});

	/*******************************************************************************
	 * Remove victim relations upon its death
	 *
	 * @caller ref victim
	 ******************************************************************************/
	commandRegistry->registerCommand("die", new Command
	{
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.empty());			
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " died");
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.empty());
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

			};
		}
	});

	/*******************************************************************************
	 * Adds a new relation between people whom had the specified relationship with caller and target
	 *
	 * @caller ref entity
	 * @param array<string> relationsToVictim
	 * @param string newRelationForTarget
	 * @param ref target
	 * @param str reason
	 ******************************************************************************/
	commandRegistry->registerCommand("set_other_nodes_relations", new Command
	{
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 4);			
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN("People whom had one of the following relationship with " + inCommandData.caller + ":");
			for(auto& relationToVictim : std::any_cast<std::vector<std::any> >(inCommandData.arguments[0]))
			{
				PRINTLN("\t- " + std::any_cast<std::string>(relationToVictim));
			}
			PRINTLN("Now have the relationship \"" + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" with " + std::any_cast<std::string>(inCommandData.arguments[2]) + " because of " + std::any_cast<std::string>(inCommandData.arguments[3]));
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 4);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

			};
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
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);		
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " moved to " + std::any_cast<std::string>(inCommandData.arguments[0]));
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

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
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);		
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " moved to " + std::any_cast<std::string>(inCommandData.arguments[0]) + "'s location");
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

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
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 1);			
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " killed " + std::any_cast<std::string>(inCommandData.arguments[0]));
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 1);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

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
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 3);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " is now owned by " + std::any_cast<std::string>(inCommandData.arguments[0]) + " and has status \""  + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" because of "  + std::any_cast<std::string>(inCommandData.arguments[2]));
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 3);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

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
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 3);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + " now has \"" + std::any_cast<std::string>(inCommandData.arguments[1]) + "\" relationship with " + std::any_cast<std::string>(inCommandData.arguments[0]) + " because of " + std::any_cast<std::string>(inCommandData.arguments[2]));
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 3);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

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
		[](const CommandData& inCommandData)
		{
			assert(inCommandData.arguments.size() == 2);
			PRINTLN("Command: " + inCommandData.name);
			PRINTLN(inCommandData.caller + "'s \"" + std::any_cast<std::string>(inCommandData.arguments[0]) + "\" attribute is now equal to " + std::any_cast<std::string>(inCommandData.arguments[1]));
		},
		[](const CommandData& inCommandData) -> Conditions
		{
			assert(inCommandData.arguments.size() == 2);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}

			};
		}
	});
	//TODO Add proper implementation and conditions
}
