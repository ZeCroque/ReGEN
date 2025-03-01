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

void CommandRegistry::executeCommand(const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, const CommandData& inCommandData) const
{
	find(inCommandData.name)->implementation(inCast, inCommandData);
}

ConditionsBlock CommandRegistry::getCommandConditions(const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, const CommandData& inCommandData) const
{
	return find(inCommandData.name)->conditionConstructor(inCast, inCommandData);
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
