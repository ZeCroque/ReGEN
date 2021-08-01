#ifndef COMMAND_H
#define COMMAND_H

#include <any>
#include <functional>
#include <list>

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
	std::function<void(CommandData&)> implementation;
	std::function<Conditions(CommandData&)> conditionConstructor;
};

#endif // COMMAND_H
