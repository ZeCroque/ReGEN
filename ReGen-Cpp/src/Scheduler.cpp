#include "Scheduler.h"

#include "CommandsRegistry.h"
#include "DataManager.h"
#include "Rule.h"
#include "Conditions.h"

Scheduler::Scheduler(const Graph& inGraph) : randomEngine(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())), graph(inGraph)
{
}

void Scheduler::run()
{
	PRINTLN("Creating initial narrative");
	PRINT_SEPARATOR();

	Graph resultStory("New_Quest", "Story_Graph");
	resultStory.addNode(new Node("Start_Quest", {{"Node_Type", {"str", "Start"}}}));
	resultStory.addNode(new Node("End_Quest", {{"Node_Type", {"str", "End"}}}));
	
	PRINTLN("Searching for Possible Narrative Rules...");
	std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction> possibleRules;
	getPossibleRules(DataManager::getInstance()->getInitializationRules(), graph, possibleRules);
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
	constexpr auto printNodeConditions = [](const std::string& inNodeName, std::shared_ptr<ConditionsBlock> inConditionsBlock)
	{
		PRINTLN("");
		PRINT_SEPARATOR();
		PRINTLN("Conditions for " + inNodeName + " :");
		PRINT_SEPARATOR();
		inConditionsBlock->print();
	};

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
				auto* generatedNode = new Node(*storyNode);
				generatedNode->setAttribute("target", {"str", node->getName()});
				ConditionsBlock conditionsBlock;
				for(const auto& commandData : nodeModificationArguments.at(generatedNode->getName()))
				{
					conditionsBlock.append(CommandRegistry::getInstance()->getCommandConditions(cast,  commandData));
				}
				generatedNode->setConditionsBlock(conditionsBlock);
#ifndef NDEBUG
				printNodeConditions(generatedNode->getName(), generatedNode->getConditionsBlock());
#endif
				resultStory.addNode(generatedNode);
				break;
			}
			++count;
		}
	}

#ifndef NDEBUG
	const auto lastNode = (++resultStory.getNodesByIndex().begin())->second;
	printNodeConditions(lastNode->getName(), lastNode->getConditionsBlock());
#endif


	for(const auto& [storyEdgeNames, storyEdge] : storyGraph.getEdgesByNodesNames())
	{
		resultStory.addEdge({"N/A", "N/A"}, storyEdgeNames.first, storyEdgeNames.second);
	}
	resultStory.addEdge({"N/A", "N/A"}, 0, 2); //Between start node and first story node
	resultStory.addEdge({"N/A", "N/A"}, resultStory.getNodeCount() - 1, 1);

	int rewriteCount = 0;
	bool canRewrite = true;
	while(canRewrite && rewriteCount < DataManager::getInstance()->getTestLayout().maxNumberOfRewrites)
	{
		std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction> possibleRewriteRules;
		getPossibleRules(DataManager::getInstance()->getRewriteRules(), resultStory, possibleRewriteRules);

		if(!possibleRewriteRules.empty())
		{
			randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(possibleRewriteRules.size()) - 1};
			targetIndex = randomIntDistribution(randomEngine);
			count = 0;
			std::pair<Rule, std::list<std::list<std::shared_ptr<Node> > > > randomRewriteRuleWithDataSets;
			for(const auto& rule : possibleRewriteRules)
			{
				if(targetIndex == count)
				{
					randomRewriteRuleWithDataSets = rule;
					break;
				}
				++count;
			}

			randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(randomRewriteRuleWithDataSets.second.size()) - 1};
			targetIndex = randomIntDistribution(randomEngine);
			count = 0;
			std::list<std::shared_ptr<Node> > randomRewriteDataSets; //This is the node(s) that could be replaced by the rewrite rule
			for(const auto& dataSet : randomRewriteRuleWithDataSets.second)
			{
				if(targetIndex == count)
				{
					randomRewriteDataSets = dataSet;
					break;
				}
				++count;
			}

			auto& [rewriteRuleName, rewriteRuleSocialConditions, rewriteRuleStoryConditions, rewriteRuleStoryGraph, rewriteRuleNodeModificationArguments] = randomRewriteRuleWithDataSets.first;

			bool isValid = false;
			for(const auto& [socialNodeName, socialNode] : rewriteRuleSocialConditions.getNodesByName())
			{
				if(auto foundNode = cast.find<std::string>(socialNodeName); foundNode != cast.end())
				{
					socialNode->setAttribute("name", {"str", foundNode->second->getName()});
					isValid = true;
				}
			}

			if(isValid)
			{
				std::list<std::list<std::shared_ptr<Node> > > randomNodeDataSets;
				graph.getIsomorphicSubGraphs(rewriteRuleSocialConditions, randomNodeDataSets);
				if(!randomNodeDataSets.empty())
				{
					randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(randomNodeDataSets.size()) - 1};
					targetIndex = randomIntDistribution(randomEngine);
					count = 0;
					std::list<std::shared_ptr<Node> > randomNodeDataSet; //This is the objects that will be used to fill RewriteRule Story targets, with missing NPCs added to cast 
					for(const auto& dataSet : randomNodeDataSets)
					{
						if(targetIndex == count)
						{
							randomNodeDataSet = dataSet;
							break;
						}
						++count;
					}

					for(const auto& node : randomNodeDataSet)
					{
						//TODO add new NPCs to cast
					}
				}
			}
			++rewriteCount;
		}
		else
		{
			canRewrite = false;
		}
	}

	resultStory.saveAsDotFile();
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
