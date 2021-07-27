#include "CommandsDeclaration.h"

#include "CommandsRegistry.h"

void declareCommands()
{	
	auto* commandRegistry = CommandRegistry::getInstance();
	const std::hash<std::string> hasher;
	
	/*******************************************************************************
	 * Remove victim relations (usually called after it dies)
	 *
	 * @param ref murderer
	 ******************************************************************************/	
	commandRegistry->registerCommand(hasher("removeVictimRelations"), new Command
	{
		[](std::vector<std::any>& args)
		{
			assert(args.size() == 1);
			DBG(std::any_cast<std::string>(args[0]));
		},
		[](std::vector<std::any>& args) -> Conditions
		{
			assert(args.size() == 1);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}
				
			};
		}
	});

	/*******************************************************************************
	 * Updates relations of victim's friends toward its murderer
	 *
	 * @param ref murderer
	 ******************************************************************************/	
	commandRegistry->registerCommand(hasher("updateVictimsFriendRelationsTowardMurderer"), new Command
	{
		[](std::vector<std::any>& args)
		{
			assert(args.size() == 1);
		},
		[](std::vector<std::any>& args) -> Conditions
		{
			assert(args.size() == 1);
			return
			{
				std::list<Condition>{},
				std::list<Condition>{}
				
			};
		}
	});
	//TODO Add all command as well as their implementation and conditions
}
