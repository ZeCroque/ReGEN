#ifndef COMMAND_H
#define COMMAND_H

#include <any>
#include <functional>

#include "Conditions.h"

struct CommandData
{
	std::string name;
	std::string caller;
	std::vector<std::any> arguments;
};

struct Command
{
	std::function<void(const std::unordered_map<std::string, std::shared_ptr<Node> >&, const CommandData&)> implementation;
	std::function<ConditionsBlock(const std::unordered_map<std::string, std::shared_ptr<Node> >&, const CommandData&)> conditionConstructor;
};

#endif // COMMAND_H
