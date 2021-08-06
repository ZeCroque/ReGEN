#ifndef COMMAND_H
#define COMMAND_H

#include <any>
#include <functional>

using Condition = int; //TODO do proper Condition struct

struct CommandData
{
	std::string name;
	std::string caller;
	std::vector<std::any> arguments;
};

struct Conditions
{
	std::list<Condition> preConditions;
	std::list<Condition> postConditions;
};

struct Command
{
	std::function<void(const CommandData&)> implementation;
	std::function<Conditions(const CommandData&)> conditionConstructor;
};

#endif // COMMAND_H
