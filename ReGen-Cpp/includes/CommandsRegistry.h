#ifndef COMMANDS_REGISTRY_H
#define COMMANDS_REGISTRY_H

#include <map>

#include "Command.h"
#include "Singleton.h"

class CommandRegistry final : public Singleton<CommandRegistry>
{
	friend class Singleton<CommandRegistry>;

public:
	void registerCommand(size_t inKey, Command* inObject) const;
	void executeCommand(size_t inKey, std::vector<std::any>& args) const;
	[[nodiscard]] Conditions getCommandConditions(size_t inKey, std::vector<std::any>& args) const;

private:
	Command* find(size_t inKey) const;
	
	mutable std::map<size_t, Command*> commands; //TODO replace with unordered map and use string as key
};

#endif // COMMANDS_REGISTRY_H
