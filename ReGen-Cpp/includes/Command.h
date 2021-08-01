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
	std::function<void(std::vector<std::any>&)> implementation;
	std::function<Conditions(std::vector<std::any>&)> conditionConstructor;
};

#endif // COMMAND_H
