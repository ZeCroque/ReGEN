#include "Scheduler.h"

#include "DataManager.h"
#include "Rule.h"

Scheduler::Scheduler(const Graph& inGraph) : randomEngine(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())), graph(inGraph)
{
}

void Scheduler::run()
{
	PRINTLN("Creating initial narrative");
	PRINT_SEPARATOR();

	Graph resultStory("New_Quest", "Story_Graph");
	resultStory.addNode(new Node("Start_Quest", "N/A", {{"Node_Type", {"str", "Start"}}}));
	resultStory.addNode(new Node("End_Quest", "N/A", {{"Node_Type", {"str", "End"}}}));
	
	PRINTLN("Searching for Possible Narrative Rules...");
	std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction> possibleRules;
	getPossibleRules(DataManager::getInstance()->getInitializationRules(), possibleRules);
	PRINTLN(std::string("Found ") + std::to_string(possibleRules.size()) + " possible rules.");
	if(possibleRules.empty())
	{
		PRINTLN("No possible rules found. Generation failed.");
		exit(0);
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

	randomIntDistribution = std::uniform_int_distribution{0, static_cast<int>(randomRuleWithDataSets.second.size()) - 1};
	targetIndex = randomIntDistribution(randomEngine);
	count = 0;
	std::list<std::shared_ptr<Node> > randomDataSet;
	for(const auto& ruleDataSet : randomRuleWithDataSets.second)
	{
		if(targetIndex == count)
		{
			randomDataSet = ruleDataSet;
			break;
		}
		++count;
	}

	const auto playerNode = DataManager::getInstance()->getWorldGraph().getNodeByName("Player");
	bool playerYetAdded = false;
	for(const auto& node : randomDataSet)
	{
		if(node->getName() == playerNode->getName())
		{
			playerYetAdded = true;
			break;
		}
	}
	if(!playerYetAdded)
	{
		randomDataSet.emplace_back(playerNode);
	}

	for(const auto& node : randomDataSet)
	{
		PRINTLN(node->getName());

	}
	
	auto& [name, socialConditions, storyConditions, storyGraph, nodeModificationArguments] = randomRuleWithDataSets.first;
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
				resultStory.addNode(generatedNode);
				break;
			}
			++count;
		}
	}

	for(const auto& [storyEdgeNames, storyEdge] : storyGraph.getEdgesByNodesNames())
	{
		resultStory.addEdge({"N/A", "N/A"}, storyEdgeNames.first, storyEdgeNames.second);
	}
	resultStory.addEdge({"N/A", "N/A"}, 0, 2); //Between start node and first story node
	resultStory.addEdge({"N/A", "N/A"}, resultStory.getNodeCount() - 1, 1);
	resultStory.saveAsDotFile();
}

void Scheduler::getPossibleRules(const std::list<Rule>& inRuleSet, std::unordered_map<Rule, std::list<std::list<std::shared_ptr<Node> > >, RuleHashFunction>& outPossibleRules) const
{
	for(const auto& rule : inRuleSet)
	{
		std::list<std::list<std::shared_ptr<Node> > > dataSet;
		if( graph.getType() == "Social_Graph")
		{
			graph.getIsomorphicSubGraphs(rule.socialConditions, dataSet);
		}
		else if(graph.getType() == "Story_Graph") 
		{
			graph.getIsomorphicSubGraphs(rule.storyConditions, dataSet);
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
