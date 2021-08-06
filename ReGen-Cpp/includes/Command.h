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
	std::function<void(const CommandData&)> implementation;
	std::function<ConditionsBlock(const CommandData&)> conditionConstructor;
};

#endif // COMMAND_H
