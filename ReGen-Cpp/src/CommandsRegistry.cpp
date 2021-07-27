#include "CommandsRegistry.h"

void CommandRegistry::registerCommand(const size_t inKey, Command* inObject) const
{	
	if (commands.find(inKey) == commands.end())
	{
		commands[inKey] = inObject;
		return;
	}
	assert(false);	
}

void CommandRegistry::executeCommand(const size_t inKey, std::vector<std::any>& args) const
{
	find(inKey)->implementation(args);
}

Conditions CommandRegistry::getCommandConditions(const size_t inKey, std::vector<std::any>& args) const
{
	return find(inKey)->conditionConstructor(args);
}

Command* CommandRegistry::find(const size_t inKey) const
{
	Command* result = nullptr;
	if (const auto command = commands.find(inKey); command != commands.end())
	{
		result = command->second;			
	}
	assert(result);
	return result;
}
