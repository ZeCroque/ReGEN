#include "Scheduler.h"

#include "CommandsRegistry.h"
#include "DataManager.h"
#include "Rule.h"
#include "Conditions.h"

std::default_random_engine Scheduler::randomEngine(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

Scheduler::Scheduler(std::string inQuestName) : questName(std::move(inQuestName))
{
}

void Scheduler::run()
{
	PRINTLN("Creating initial narrative");
	PRINT_SEPARATOR();

	Graph resultStory(questName, "Story_Graph");
	resultStory.addNode(new Node("Start_Quest", {{"Node_Type", {"str", "Start"}}}));
	resultStory.addNode(new Node("End_Quest", {{"Node_Type", {"str", "End"}}}));
	
	PRINTLN("Searching for Possible Narrative Rules...");
	std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction> possibleRules;
	getPossibleRules(DataManager::getInstance()->getInitializationRules(), DataManager::getInstance()->getWorldGraph(), possibleRules);
	PRINTLN(std::string("Found ") + std::to_string(possibleRules.size()) + " possible rules.");
	if(possibleRules.empty())
	{
		PRINTLN("No possible rules found. Generation failed.");
		return; //TODO proper failure handling
	}
	
	std::uniform_int_distribution randomIntDistribution{0, static_cast<int>(possibleRules.size()) - 1};
	int targetIndex = randomIntDistribution(randomEngine);
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
	PRINTLN("Randomly picked the " + randomRuleWithDataSets.first.name + " rule.");

	auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments] = randomRuleWithDataSets.first;
	
	randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(randomRuleWithDataSets.second.size()) - 1};
	targetIndex = randomIntDistribution(randomEngine);
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

	Graph finalStory;
	rewriteStory(resultStory, cast, finalStory);
	finalStory.saveAsDotFile();
}

void Scheduler::getPossibleRules(const std::list<Rule>& inRuleSet, const Graph& inGraph, std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction>& outPossibleRules)
{
	for(const auto& rule : inRuleSet)
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
			outPossibleRules[rule] = dataSet;
		}
	}
}

void Scheduler::rewriteStory(const Graph& inStory, const std::unordered_map<std::string, std::shared_ptr<Node>>& inCast, Graph& outStory, int inRewritesCount)
{
	if(inRewritesCount == DataManager::getInstance()->getTestLayout().maxNumberOfRewrites)
	{
		return;
	}

	Graph tempStory(inStory);
	std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction> possibleRewriteRules;
	getPossibleRules(DataManager::getInstance()->getRewriteRules(), tempStory, possibleRewriteRules);
	if(possibleRewriteRules.empty())
	{
		return;
	}

	bool castChanged = false;
	auto tempCast(inCast);

	std::uniform_int_distribution randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(possibleRewriteRules.size()) - 1};
	int targetIndex = randomIntDistribution(randomEngine);
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

	randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(rewriteRuleWithDataSets.second.size()) - 1};
	targetIndex = randomIntDistribution(randomEngine);
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

	auto& [rewriteRuleName, rewriteRuleSocialConditions, rewriteRuleStoryConditions, rewriteRuleStoryGraph, rewriteRuleNodeModificationArguments] = rewriteRuleWithDataSets.first;

	bool isValid = false;
	for(const auto& [socialNodeName, socialNode] : rewriteRuleSocialConditions.getNodesByName())
	{
		if(auto foundNode = inCast.find<std::string>(socialNodeName); foundNode != inCast.end())
		{
			socialNode->setAttribute("name", {"str", foundNode->second->getName()});
			isValid = true;
		}
	}

	bool storyRewritten = false;
	if(isValid)
	{
		std::list<std::list<std::shared_ptr<Node> > > possibleRewriteRuleCasts;
		DataManager::getInstance()->getWorldGraph().getIsomorphicSubGraphs(rewriteRuleSocialConditions, possibleRewriteRuleCasts);
		if(!possibleRewriteRuleCasts.empty())
		{
			randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(possibleRewriteRuleCasts.size()) - 1};
			targetIndex = randomIntDistribution(randomEngine);
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
			for(const auto& node : rewriteRuleCast)
			{
				const auto socialNodeName = rewriteRuleSocialConditions.getNodeByIndex(count)->getName();
				if(auto foundActor = tempCast.find<std::string>(rewriteRuleSocialConditions.getNodeByIndex(count)->getName()); foundActor == tempCast.end())
				{
					tempCast[socialNodeName] = node;
					castChanged = true;
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

			for(auto [storyNodeIndex, storyNode] : rewriteRuleStoryGraph.getNodesByIndex())
			{
				auto generatedNode = tempStory.addNode(new Node(*storyNode));
				generatedNode->setAttribute("target", {"str", tempCast[storyNode->getAttribute("target").value]->getName()});
				generatedNode->clearEdges();
				if(storyNode->getIncomingEdges().empty())
				{
					for(const auto& nodePreviouslyConnectedToRewriteStartNode : nodesPreviouslyConnectedToRewriteStartNode)
					{
						tempStory.addEdge({"N/A", "N/A"}, nodePreviouslyConnectedToRewriteStartNode, generatedNode);
					}
				}
				if(storyNode->getOutgoingEdges().empty())
				{
					for(const auto& nodePreviouslyConnectedToRewriteEndNode : nodesPreviouslyConnectedToRewriteEndNode)
					{
						tempStory.addEdge({"N/A", "N/A"}, generatedNode, nodePreviouslyConnectedToRewriteEndNode);
					}
				}
				createNodeConditions(rewriteRuleNodeModificationArguments, tempCast, generatedNode);
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
	Graph story;
	outStory = storyRewritten ? tempStory : inStory;
	rewriteStory(outStory, castChanged ? tempCast : inCast, story, ++inRewritesCount);
}

#ifndef NDEBUG
void Scheduler::printNodeConditions(const std::string& inNodeName, std::shared_ptr<ConditionsBlock> inConditionsBlock)
{
	PRINTLN("");
	PRINT_SEPARATOR();
	PRINTLN("Conditions for " + inNodeName + " :");
	PRINT_SEPARATOR();
	inConditionsBlock->print();
}
#endif

void Scheduler::createNodeConditions(const std::unordered_map<std::string, std::list<CommandData> >& inRuleCommandsData, const std::unordered_map<std::string, std::shared_ptr<Node> >& inCast, std::shared_ptr<Node> inNode)
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