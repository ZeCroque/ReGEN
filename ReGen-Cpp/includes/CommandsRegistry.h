#ifndef COMMANDS_REGISTRY_H
#define COMMANDS_REGISTRY_H

#include <functional>
#include <map>
#include <any>

#include "Singleton.h"

using Condition = int; //TODO do proper Condition struct
using CommandArguments = std::any;  //TODO do proper CommandArguments struct

struct Conditions
{
	std::list<Condition> preConditions;
	std::list<Condition> postConditions;
};

struct Command
{
	std::function<void(std::vector<std::any>&)> implementation;
	std::function<Conditions(std::vector<std::any>&)> conditionConstructor;
};


class CommandRegistry final : public Singleton<CommandRegistry>
{
	friend class Singleton<CommandRegistry>;

public:
	void registerCommand(size_t inKey, Command* inObject) const;
	void executeCommand(size_t inKey, std::vector<std::any>& args) const;
	[[nodiscard]] Conditions getCommandConditions(size_t inKey, std::vector<std::any>& args) const;

private:
	Command* find(size_t inKey) const;
	
	mutable std::map<size_t, Command*> commands;
};

#endif // COMMANDS_REGISTRY_H
