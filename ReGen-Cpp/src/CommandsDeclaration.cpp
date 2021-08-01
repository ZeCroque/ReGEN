#include "CommandsDeclaration.h"

#include "CommandsRegistry.h"

void declareCommands()
{	
	auto* commandRegistry = CommandRegistry::getInstance();
	
	/*******************************************************************************
	 * Remove victim relations (usually called after it dies)
	 *
	 * @param ref murderer
	 ******************************************************************************/	
	commandRegistry->registerCommand("removeVictimRelations", new Command
	                                 {
		                                 [](CommandData& inCommandData)
		                                 {
			                                 assert(inCommandData.arguments.size() == 1);
			                                 PRINTLN(std::any_cast<std::string>(inCommandData.arguments[0]));
		                                 },
		                                 [](CommandData& inCommandData) -> Conditions
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
	 * Updates relations of victim's friends toward its murderer
	 *
	 * @param ref murderer
	 ******************************************************************************/	
	commandRegistry->registerCommand("murder", new Command
	                                 {
		                                 [](CommandData& inCommandData)
		                                 {
			                                 assert(inCommandData.arguments.size() == 1);
		                                 },
		                                 [](CommandData& inCommandData) -> Conditions
		                                 {
			                                 assert(inCommandData.arguments.size() == 1);
			                                 return
			                                 {
				                                 std::list<Condition>{},
				                                 std::list<Condition>{}
				
			                                 };
		                                 }
	                                 });
	//TODO Add all command as well as their implementation and conditions
}
