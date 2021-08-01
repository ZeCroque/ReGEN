#include "CommandsRegistry.h"

void CommandRegistry::registerCommand(const std::string& inKey, Command* inObject) const
{	
	if (commands.find(inKey) == commands.end())
	{
		commands[inKey] = inObject;
		return;
	}
	assert(false);	
}

void CommandRegistry::executeCommand(const std::string& inKey, CommandData& inCommandData) const
{
	find(inKey)->implementation(inCommandData);
}

Conditions CommandRegistry::getCommandConditions(const std::string& inKey, CommandData& inCommandData) const
{
	return find(inKey)->conditionConstructor(inCommandData);
}

Command* CommandRegistry::find(const std::string& inKey) const
{
	Command* result = nullptr;
	if (const auto command = commands.find(inKey); command != commands.end())
	{
		result = command->second;			
	}
	assert(result);
	return result;
}
