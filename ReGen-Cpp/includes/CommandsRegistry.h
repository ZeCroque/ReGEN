#ifndef COMMANDS_REGISTRY_H
#define COMMANDS_REGISTRY_H

#include "Command.h"
#include "Singleton.h"

class CommandRegistry final : public Singleton<CommandRegistry>
{
	friend class Singleton<CommandRegistry>;

public:
	void registerCommand(const std::string& inKey, Command* inObject) const;
	void executeCommand(const std::string& inKey, CommandData& inCommandData) const;
	[[nodiscard]] Conditions getCommandConditions(const std::string& inKey, CommandData& inCommandData) const;

private:
	Command* find(const std::string& inKey) const;
	
	mutable std::unordered_map<std::string, Command*> commands;
};

#endif // COMMANDS_REGISTRY_H
