#include "CommandsRegistry.h"

#include <cassert>

void CommandManager::registerCommand(const size_t inKey, Command* inObject) const
{	
	if (commands.find(inKey) == commands.end())
	{
		commands[inKey] = inObject;
		return;
	}
	assert(false);	
}

void CommandManager::executeCommand(const size_t inKey, std::vector<std::any>& args) const
{
	find(inKey)->implementation(args);
}

Conditions CommandManager::getCommandConditions(const size_t inKey, std::vector<std::any>& args) const
{
	return find(inKey)->conditionConstructor(args);
}

Command* CommandManager::find(const size_t inKey) const
{
	Command* result = nullptr;
	if (const auto command = commands.find(inKey); command != commands.end())
	{
		result = command->second;			
	}
	assert(result);
	return result;
}
