#include "Scheduler.h"

#include "CommandsRegistry.h"
#include "DataManager.h"
#include "Rule.h"
#include "Conditions.h"

std::default_random_engine Scheduler::randomEngine = std::mt19937(42);
std::unordered_map<std::string, int> Scheduler::rulesUsages;  // NOLINT(clang-diagnostic-exit-time-destructors)

Scheduler::Scheduler(std::string inQuestName) : questName(std::move(inQuestName))
{
	if(rulesUsages.empty())
	{
		for(const auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments, appliesOnce] : DataManager::getInstance()->getInitializationRules())
		{
			rulesUsages[name] = 0;
		}
	}
}

void Scheduler::run()
{
	PRINTLN("Creating initial narrative");
	PRINT_SEPARATOR();

	Graph resultStory(questName, "Story_Graph");
	resultStory.addNode(new Node("Start_Quest", {{"Node_Type", {"str", "Start"}}}));
	resultStory.addNode(new Node("End_Quest", {{"Node_Type", {"str", "End"}}}));
	
	PRINTLN("Searching for Possible Narrative Rules...");
	std::list<std::pair<Rule, std::list<std::list<std::shared_ptr<Node> > > > > possibleRules;
	getPossibleRules(DataManager::getInstance()->getInitializationRules(), DataManager::getInstance()->getWorldGraph(), rulesUsages, possibleRules);
	PRINTLN(std::string("Found ") + std::to_string(possibleRules.size()) + " possible rules.");
	if(possibleRules.empty())
	{
		PRINTLN("No possible rules found. Generation failed.");
		return; //TODO proper failure handling
	}

	size_t targetIndex = randomEngine() % possibleRules.size();
	int count = 0;
	std::pair<Rule, std::list<std::list<std::shared_ptr<Node> > > > randomRuleWithDataSets;
	PRINTLN("Found the following possible rules :");
	for(const auto& rule : possibleRules)
	{
		if(targetIndex == count)
		{
			randomRuleWithDataSets = rule;
#ifdef NDEBUG
			break;
#endif
		}
		++count;
		PRINTLN("\t" + rule.first.name);
	}
	auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments, appliesOnce] = randomRuleWithDataSets.first;
	PRINTLN("Randomly picked the " + name + " rule.");
	++rulesUsages[name];

	targetIndex = randomEngine() % randomRuleWithDataSets.second.size();
	count = 0;
	std::unordered_map<std::string, std::shared_ptr<Node> > cast;
	std::list<std::shared_ptr<Node> > randomDataSet;
	for(const auto& ruleDataSet : randomRuleWithDataSets.second)
	{
		if(targetIndex == count)
		{
			int i = 0;
			for(const auto& socialNode : ruleDataSet)
			{
				cast[socialConditions.getNodeByIndex(i)->getName()] = socialNode;
				++i;
			}
			randomDataSet = ruleDataSet;
			break;
		}
		++count;
	}

	if(!cast.contains<std::string>("Player"))
	{
		cast["Player"] = DataManager::getInstance()->getWorldGraph().getNodeByName("Player");
	}

	const auto startingNode = resultStory.getNodesByIndex().begin()->second;
	for(const auto& [socialNodeIndex, socialNode] : socialConditions.getNodesByIndex())
	{
		for(const auto& [attributeName, attributeData] : socialNode->getAttributes())
		{
			startingNode->getConditionsBlock()->preConditions.nodeConditions.emplace_back(NodeCondition{cast.at( socialNode->getName()), attributeName, attributeData.value, ComparisonType::Equal});
		}

		for(const auto& edge : socialNode->getOutgoingEdges())
		{
			startingNode->getConditionsBlock()->preConditions.edgeConditions.emplace_back(EdgeCondition{cast.at(edge->getSourceNode()->getName()), cast.at(edge->getTargetNode()->getName()), edge->getAttributes()});
		}
	}

#ifndef NDEBUG
	printNodeConditions(startingNode->getName(), startingNode->getConditionsBlock());
#endif

	for(const auto& [storyNodeIndex, storyNode] : storyGraph.getNodesByIndex())
	{
		const auto index = socialConditions.getNodeByName(storyNode->getAttribute("target").value)->getIndex();
		count = 0;
		for(const auto& node : randomDataSet)
		{
			if(count == index)
			{
				const auto generatedNode = resultStory.addNode(new Node(*storyNode));
				generatedNode->setAttribute("target", {"str", node->getName()});
				createNodeConditions(nodeModificationArguments, cast, generatedNode);
				generatedNode->clearEdges();
				
				break;
			}
			++count;
		}
	}

#ifndef NDEBUG
	const auto lastNode = resultStory.getNodeByIndex(1);
	printNodeConditions(lastNode->getName(), lastNode->getConditionsBlock());
#endif


	for(const auto& [storyEdgeNames, storyEdge] : storyGraph.getEdgesByNodesNames())
	{
		resultStory.addEdge({"N/A", "N/A"}, storyEdgeNames.first, storyEdgeNames.second);
	}
	resultStory.addEdge({"N/A", "N/A"}, 0, 2); //Between start node and first story node
	resultStory.addEdge({"N/A", "N/A"}, resultStory.getNodeCount() - 1, 1);

	std::unordered_map<std::string, int> rewriteRulesUsages;
	for(const auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments, appliesOnce] : DataManager::getInstance()->getRewriteRules())
	{
		rewriteRulesUsages[name] = 0;
	}

	bool canRewrite = true;
	int rewriteCount = 0;
	Graph& finalStory = resultStory;
	Graph tempStory;
	while(canRewrite && rewriteCount < DataManager::getInstance()->getTestLayout().maxNumberOfRewrites)
	{
		canRewrite = rewriteStory(finalStory, cast, rewriteRulesUsages, tempStory);
		finalStory = tempStory;
		++rewriteCount;
	}
	PRINTLN("Stopped rewriting: " + std::string(canRewrite ? "Max rewrite count reached." : "No rewrite rules available."));

	finalStory.saveAsDotFile();
}

void Scheduler::getPossibleRules(const std::list<Rule>& inRuleSet, const Graph& inGraph, const std::unordered_map<std::string, int>& inRuleUsages, std::list<std::pair<Rule, std::list<std::list<std::shared_ptr<Node> > > > >& outPossibleRules)
{
	for(const auto& rule : inRuleSet)
	{

		if(!rule.appliesOnce || !inRuleUsages.at(rule.name))
		{
			std::list<std::list<std::shared_ptr<Node> > > dataSet;
			if(inGraph.getType() == "Social_Graph")
			{
				inGraph.getIsomorphicSubGraphs(rule.socialConditions, dataSet);
			}
			else if(inGraph.getType() == "Story_Graph") 
			{
				inGraph.getIsomorphicSubGraphs(rule.storyConditions, dataSet);
			}
			else
			{
				assert(false);
			}
			if(!dataSet.empty())
			{
				outPossibleRules.push_back({rule, dataSet});
			}
		}
	}
}

bool Scheduler::rewriteStory(const Graph& inStory, const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, std::unordered_map<std::string, int>& inRuleUsages, Graph& outStory)
{
	PRINTLN("");
	PRINTLN("Attempting to rewrite");
	PRINT_SEPARATOR();
	PRINTLN("Checking rewrite rules...");
	Graph tempStory(inStory);
	std::list<std::pair<Rule, std::list<std::list<std::shared_ptr<Node> > > > > possibleRewriteRules;
	getPossibleRules(DataManager::getInstance()->getRewriteRules(), tempStory, inRuleUsages, possibleRewriteRules);

	bool storyRewritten = false;
	if(!possibleRewriteRules.empty())
	{
		PRINTLN("Found " + std::to_string(possibleRewriteRules.size()) + " possible rewrite rules.");
		int targetIndex = randomEngine() % possibleRewriteRules.size();
		int count = 0;
		std::pair<Rule, std::list<std::list<std::shared_ptr<Node> > > > rewriteRuleWithDataSets;
		for(const auto& rule : possibleRewriteRules)
		{
			if(targetIndex == count)
			{
				rewriteRuleWithDataSets = rule;
				break;
			}
			++count;
		}

		auto& [rewriteRuleName, rewriteRuleSocialConditions, rewriteRuleStoryConditions, rewriteRuleStoryGraph, rewriteRuleNodeModificationArguments, rewriteRuleAppliesOnce] = rewriteRuleWithDataSets.first;
		PRINTLN("Picked the " + rewriteRuleName + " rewrite rule.");
		++inRuleUsages[rewriteRuleName];
		
		for(const auto& [socialNodeName, socialNode] : rewriteRuleSocialConditions.getNodesByName())
		{
			if(auto foundNode = inCast.find<std::string>(socialNodeName); foundNode != inCast.end())
			{
				socialNode->setAttribute("name", {"str", foundNode->second->getName()});
			}
		}

		std::list<std::list<std::shared_ptr<Node> > > possibleRewriteRuleCasts;
		DataManager::getInstance()->getWorldGraph().getIsomorphicSubGraphs(rewriteRuleSocialConditions, possibleRewriteRuleCasts);

		if(!possibleRewriteRuleCasts.empty())
		{
			targetIndex = randomEngine() % rewriteRuleWithDataSets.second.size();
			count = 0;
			std::list<std::shared_ptr<Node> > rewriteRuleDataSet; //This is the node(s) that could be replaced by the rewrite rule
			for(const auto& dataSet : rewriteRuleWithDataSets.second)
			{
				if(targetIndex == count)
				{
					rewriteRuleDataSet = dataSet;
					break;
				}
				++count;
			}
			targetIndex = randomEngine() % possibleRewriteRuleCasts.size();
			count = 0;
			std::list<std::shared_ptr<Node> > rewriteRuleCast; //This is the objects that will be used to fill RewriteRule Story targets, with missing NPCs added to cast 
			for(const auto& dataSet : possibleRewriteRuleCasts)
			{
				if(targetIndex == count)
				{
					rewriteRuleCast = dataSet;
					break;
				}
				++count;
			}

			count = 0;
			auto tempCast(inCast);
			for(const auto& node : rewriteRuleCast)
			{
				const auto socialNodeName = rewriteRuleSocialConditions.getNodeByIndex(count)->getName();
				if(auto foundActor = tempCast.find<std::string>(rewriteRuleSocialConditions.getNodeByIndex(count)->getName()); foundActor == tempCast.end())
				{
					tempCast[socialNodeName] = node;
				}
				++count;
			}

			const auto rewriteStartNode = *rewriteRuleDataSet.begin();
			const auto rewriteEndNode = *--rewriteRuleDataSet.end();

			std::list<std::shared_ptr<Node> > nodesPreviouslyConnectedToRewriteStartNode;
			std::list<std::shared_ptr<Node> > nodesPreviouslyConnectedToRewriteEndNode;

			while(!rewriteStartNode->getIncomingEdges().empty())
			{
				auto incomingEdge = *rewriteStartNode->getIncomingEdges().begin();
				nodesPreviouslyConnectedToRewriteStartNode.emplace_back(incomingEdge->getSourceNode());
				tempStory.removeEdge(incomingEdge);
			}
			while(!rewriteEndNode->getOutgoingEdges().empty())
			{
				auto outgoingEdge = *rewriteEndNode->getOutgoingEdges().begin();
				nodesPreviouslyConnectedToRewriteEndNode.emplace_back(outgoingEdge->getTargetNode());
				tempStory.removeEdge(outgoingEdge);
			}
			tempStory.removeNode(rewriteStartNode);
			tempStory.removeNode(rewriteEndNode);

			for(auto [storyNodeName, storyNode] : rewriteRuleStoryGraph.getNodesByName())
			{
				auto* generatedNode = new Node(*storyNode);
				while(tempStory.getNodesByName().contains<std::string>(generatedNode->getName()))
				{
					generatedNode->setName(generatedNode->getName() + "_");
				}

				auto addedNode = tempStory.addNode(generatedNode);
				addedNode->setAttribute("target", {"str", tempCast[storyNode->getAttribute("target").value]->getName()});
				addedNode->clearEdges();
				if(storyNode->getIncomingEdges().empty())
				{
					for(const auto& nodePreviouslyConnectedToRewriteStartNode : nodesPreviouslyConnectedToRewriteStartNode)
					{
						tempStory.addEdge({"N/A", "N/A"}, nodePreviouslyConnectedToRewriteStartNode, addedNode);
					}
				}
				if(storyNode->getOutgoingEdges().empty())
				{
					for(const auto& nodePreviouslyConnectedToRewriteEndNode : nodesPreviouslyConnectedToRewriteEndNode)
					{
						tempStory.addEdge({"N/A", "N/A"}, addedNode, nodePreviouslyConnectedToRewriteEndNode);
					}
				}
				createNodeConditions(rewriteRuleNodeModificationArguments, tempCast, addedNode);
			}


			for(const auto& [storyEdgeNames, storyEdge] : rewriteRuleStoryGraph.getEdgesByNodesNames())
			{
				tempStory.addEdge({"N/A", "N/A"}, storyEdgeNames.first, storyEdgeNames.second);
			}

			auto lastNode = tempStory.getNodeByIndex(1);
			lastNode->validateNode({}, true);
			storyRewritten = lastNode->isValid();
			PRINTLN("Story valid ? " + std::to_string(storyRewritten));

		}
	}
	outStory = storyRewritten ? tempStory : inStory;
	return true;
}

#ifndef NDEBUG
void Scheduler::printNodeConditions(const std::string& inNodeName, const std::shared_ptr<ConditionsBlock> inConditionsBlock)  // NOLINT(performance-unnecessary-value-param)
{
	PRINTLN("");
	PRINT_SEPARATOR();
	PRINTLN("Conditions for " + inNodeName + " :");
	PRINT_SEPARATOR();
	inConditionsBlock->print();
}
#endif

void Scheduler::createNodeConditions(const std::unordered_map<std::string, std::list<CommandData> >& inRuleCommandsData, const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, const std::shared_ptr<Node> inNode)  // NOLINT(performance-unnecessary-value-param)
{
	ConditionsBlock conditionsBlock;
	if(const auto& commandsData = inRuleCommandsData.find<std::string>(inNode->getName()); commandsData != inRuleCommandsData.end())
	{
		for(const auto& commandData : commandsData->second)
		{
			conditionsBlock.append(CommandRegistry::getInstance()->getCommandConditions(inCast,  commandData));
		}
	}
	inNode->setConditionsBlock(conditionsBlock);
#ifndef NDEBUG
	printNodeConditions(inNode->getName(), inNode->getConditionsBlock());
#endif
}
