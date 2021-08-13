#include "Graph.h"

#include <filesystem>
#include <fstream>
#include <pugixml.hpp>

#include "Conditions.h"
#include "Utils.h"

Node::Node() : bIsValid(true), index(NONE)
{
}

Node::Node(std::string inName, std::unordered_map<std::string, NodeAttribute> inAttributes) : name(std::move(inName)), attributes(std::move(inAttributes)), bIsValid(true), index(NONE)
{
}

const std::string& Node::getName() const
{
	return name;
}

void Node::setName(const std::string& inName)
{
	name = inName;
	//TODO change key in nodesByName map and in edgesByNodeNames, not already done because it is atm only used on not yet added nodes
}

const std::unordered_map<std::string, NodeAttribute>& Node::getAttributes() const
{
	return attributes;
}

const NodeAttribute& Node::getAttribute(const std::string& inAttributeName) const
{
	return attributes.at(inAttributeName);
}

void Node::setAttribute(const std::string& inAttributeName, const NodeAttribute& inAttributeValue)
{
	attributes[inAttributeName] = inAttributeValue;
}

std::shared_ptr<ConditionsBlock> Node::getConditionsBlock() const
{
	if(!conditionsBlock)
	{
		conditionsBlock.reset(new ConditionsBlock());
	}
	return conditionsBlock;
}

void Node::setConditionsBlock(ConditionsBlock& inConditionsBlock) const
{
	conditionsBlock = std::make_shared<ConditionsBlock>(inConditionsBlock);
}

const std::list<std::shared_ptr<Edge>>& Node::getIncomingEdges() const
{
	return incomingEdges;
}

const std::list<std::shared_ptr<Edge>>& Node::getOutgoingEdges() const
{
	return outgoingEdges;
}

int Node::getIndex() const
{
	return index;
}

bool Node::isValid() const
{
	return bIsValid;
}

void Node::clearEdges()
{
	incomingEdges.clear();
	outgoingEdges.clear();
}

std::shared_ptr<Node> Node::getSourceNodeFromIncomingEdgeWithAttribute(const std::pair<std::string, std::string>& inAttribute)
{
	std::shared_ptr<Node> sourceNode;
	for(const auto& incomingEdge : incomingEdges)
	{
		if(auto locationAttribute = incomingEdge->getAttributes().find<std::string>(inAttribute.first); locationAttribute != incomingEdge->getAttributes().end() && (inAttribute.second == "N/A" || locationAttribute->second == inAttribute.second))
		{
			sourceNode = incomingEdge->getSourceNode();
			break;
		}
	}
	return sourceNode;
}

std::shared_ptr<Node> Node::getTargetNodeFromOutgoingEdgeWithAttribute(const std::pair<std::string, std::string>& inAttribute)
{
	std::shared_ptr<Node> targetNode;
	for(const auto& outGoingEdge : outgoingEdges)
	{
		if(auto locationAttribute = outGoingEdge->getAttributes().find<std::string>(inAttribute.first); locationAttribute != outGoingEdge->getAttributes().end() && (inAttribute.second == "N/A" || locationAttribute->second == inAttribute.second))
		{
			targetNode = outGoingEdge->getTargetNode();
			break;
		}
	}
	return targetNode;
}

bool Node::isSubNode(const Node& inParentNode)
{
	return containsAttributes(inParentNode) && containsEdges(incomingEdges, inParentNode.incomingEdges) && containsEdges(outgoingEdges, inParentNode.outgoingEdges);
}

bool Node::containsAttributes(const Node& inParentNode)
{
	for(const auto& [parentAttributeName, parentAttributeData] : inParentNode.attributes)
	{
		bool containsAttribute = false;
		for(const auto& [attributeName, attributeData] : attributes)
		{
			if(parentAttributeName == attributeName && (parentAttributeName == "target" || parentAttributeData.value == "N/A" || parentAttributeData.value == attributeData.value)) //TODO make virtual method for story nodes after template hell is done
			{
				containsAttribute = true;
				break;
			}
		}
		if(!containsAttribute)
		{
			return false;
		}
	}
	return true;
}

bool Node::containsEdges(const std::list<std::shared_ptr<Edge>>& inNodeEdges, const std::list<std::shared_ptr<Edge>>& inParentNodeEdges)
{
	std::list<std::shared_ptr<Edge> > foundEdges;
	for(const auto& parentNodeEdge : inParentNodeEdges) //For each edges from parent node
	{
		bool containsEdges = false;
		for(const auto& edge : inNodeEdges) //We compare it to all edges from main node
		{
			if //If the edge we are looking at hasn't yet been set as a sub-edge in previous iteration
			(
				std::ranges::none_of(foundEdges, [edge](const std::shared_ptr<Edge>& e)
				{
					return e.get() == edge.get();
				})
			)
			{
				for(const auto& [parentNodeEdgeAttributeName, parentNodeEdgeAttributeValue] : parentNodeEdge->getAttributes()) //Then we iterate through each attributes of the parentEdge
				{
					bool containsEdgeAttribute = false;
					for(const auto& [edgeAttributeName, edgeAttributeValue] : edge->getAttributes()) //And compare it to all attributes of the currently processed main edge
					{
						if(parentNodeEdgeAttributeName == edgeAttributeName && (parentNodeEdgeAttributeValue == "N/A" || parentNodeEdgeAttributeValue == edgeAttributeValue)) //If it matches, we add it to our foundSubEdges list and stop checking the attributes of this edge
						{
							containsEdgeAttribute = true;
							foundEdges.emplace_back(edge);
							break;
						}
					}
					if(containsEdgeAttribute) //If we found an edge with the required attributes then we stop searching for sub-edges of the currently checked parentEdge
					{
						containsEdges = true;
						break;
					}
				}
				if(containsEdges) //Forwarding previous statement
				{
					break;
				}
			}
		}
		if(!containsEdges) //If our comparison ended without entering all conditions each time, then the edges of the main node are not sub-edges of the parentNode
		{
			return false;
		}
	}
	return true; //Else they are
}

void Node::validateNode(Conditions preConditions, bool inValid)
{
	PRINTLN("Validating conditions for: " + name);
	bIsValid = inValid && bIsValid;

	if(conditionsBlock)
	{
		if(preConditions.conflicts(conditionsBlock->postConditions))
		{
			PRINTLN("Invalid");
#ifndef NDEBUG
			preConditions.print();
			conditionsBlock->postConditions.print();
#endif
			bIsValid = false;
		}

		preConditions.append(conditionsBlock->preConditions);
	}

	for(const auto& incomingEdge : incomingEdges)
	{
		incomingEdge->getSourceNode()->validateNode(preConditions, bIsValid);
	}
}

Edge::Edge(std::shared_ptr<Node> inSourceNode, std::shared_ptr<Node> inTargetNode, std::unordered_map<std::string, std::string> inAttributes) : sourceNode(std::move(inSourceNode)), targetNode(std::move(inTargetNode)), attributes(std::move(inAttributes))
{
}

std::shared_ptr<Node> Edge::getSourceNode() const
{
	return sourceNode;
}

std::shared_ptr<Node> Edge::getTargetNode() const
{
	return targetNode;
}

const std::unordered_map<std::string, std::string>& Edge::getAttributes() const
{
	return attributes;
}

#ifndef NDEBUG
void Edge::print() const
{
	for(const auto& [attributeName, attributeValue] : attributes)
	{
		PRINTLN(sourceNode->getName() + "_" + attributeName + "_" + attributeValue + "_" + targetNode->getName());
	}
}
#endif

Graph::Graph() : name("none"), type("default"), nodeCount(0), edgeCount(0)
{
}

Graph::Graph(std::string inName, std::string inType) : name(std::move(inName)), type(std::move(inType)), nodeCount(0), edgeCount(0)
{
}

const std::string& Graph::getName() const
{
	return name;
}

const std::string& Graph::getType() const
{
	return type;
}

std::shared_ptr<Node> Graph::getNodeByName(const std::string& inName) const
{
	return nodesByName.at(inName);
}

std::shared_ptr<Node> Graph::getNodeByIndex(int inIndex) const
{
	return nodesByIndex.at(inIndex);
}

const std::unordered_map<std::string, std::shared_ptr<Node>>& Graph::getNodesByName() const
{
	return nodesByName;
}

const std::map<int, std::shared_ptr<Node>>& Graph::getNodesByIndex() const
{
	return nodesByIndex;
}

const std::map<std::pair<std::string, std::string>, std::shared_ptr<Edge>>& Graph::getEdgesByNodesNames() const
{
	return edgesByNodesNames;
}

const std::map<std::pair<int, int>, std::shared_ptr<Edge>>& Graph::getEdgesByNodesIndex() const
{
	return edgesByNodesIndex;
}

int Graph::getNodeCount() const
{
	return nodeCount;
}

int Graph::getEdgeCount() const
{
	return edgeCount;
}

void Graph::loadFromXml(const std::string& inPath)
{
	pugi::xml_document document;
	document.load_file(inPath.c_str());
	const auto& parsedXml = document.document_element();

	loadFromXml(parsedXml);
}

void Graph::loadFromXml(const pugi::xml_node& inParsedXml)
{
	name = inParsedXml.attribute("name").as_string();
	type = inParsedXml.attribute("type").as_string();

	const auto parsedNodeCount = std::distance(inParsedXml.child("nodes").children().begin(), inParsedXml.child("nodes").children().end());
	adjacencyList = arma::mat(parsedNodeCount, parsedNodeCount, arma::fill::zeros);
	
	for(const auto& node : inParsedXml.child("nodes").children())
	{
		std::unordered_map<std::string, NodeAttribute> nodeAttributes;
		for(const auto& nodeAttribute : node.children("attr"))
		{
			const auto attributeName = std::string();
			nodeAttributes.insert
			({
				nodeAttribute.attribute("name").as_string(),
				NodeAttribute
				{
					nodeAttribute.attribute("type").as_string(),
					nodeAttribute.first_child().text().as_string()
				}
			});
		}
		
		addNode
		(
			new Node
			(
				node.attribute("name").as_string(),
				nodeAttributes
			)
		);
	}
	for(const auto& connection : inParsedXml.child("connections").children())
	{
		std::unordered_map<std::string, std::string> edgeAttributes;

		for(const auto& attribute : connection.children("relation"))
		{
			addEdge
			(
				{
					attribute.first_attribute().name(),
					attribute.first_attribute().as_string()
				},
				connection.attribute("from").as_string(),
				connection.attribute("to").as_string()
			);
		}
		
	}
}

std::shared_ptr<Node> Graph::addNode(Node* inNode)
{
	inNode->index = nodeCount;
	
	std::shared_ptr<Node> newNode(inNode);
	nodesByName[inNode->name] = newNode;
	nodesByIndex[nodeCount] = newNode;

	++nodeCount;
	if(const auto size = static_cast<size_t>(nodeCount); adjacencyList.n_rows < size)
	{
		adjacencyList.resize(size, size);
	}

	return newNode;
}

void Graph::removeNode(const std::shared_ptr<Node> inNodeToRemove) const
{
	nodesByName.erase(inNodeToRemove->getName());

	if(nodesByIndex.contains(inNodeToRemove->getIndex()))
	{
		nodesByIndex[inNodeToRemove->getIndex()].reset();
	}
}

void Graph::addEdge(std::pair<std::string, std::string> inEdgeAttribute, std::shared_ptr<Node> inSourceNode, std::shared_ptr<Node> inTargetNode)
{
	assert(inSourceNode.get() != inTargetNode.get());
	auto edge = edgesByNodesNames[{inSourceNode->name, inTargetNode->name}];
	if(!edge)
	{	
		edge.reset(new Edge());

		inSourceNode->outgoingEdges.emplace_back(edge);
		inTargetNode->incomingEdges.emplace_back(edge);
		
		edgesByNodesNames[{inSourceNode->name, inTargetNode->name}] = edge;
		
		const size_t sourceNodeIndex = inSourceNode->index;
		const size_t targetNodeIndex = inTargetNode->index;
		adjacencyList.at(sourceNodeIndex, targetNodeIndex) = 1;
		edgesByNodesIndex[{static_cast<int>(sourceNodeIndex), static_cast<int>(targetNodeIndex)}] = edge;

		edge->sourceNode = std::move(inSourceNode);
		edge->targetNode = std::move(inTargetNode);	
		
		++edgeCount;
	}
	edge->attributes.insert(std::move(inEdgeAttribute));
}

void Graph::addEdge(std::pair<std::string, std::string>&& inEdgeAttribute, const int inSourceIndex, const int inTargetIndex)
{
	addEdge(inEdgeAttribute, nodesByIndex.at(inSourceIndex), nodesByIndex.at(inTargetIndex));
}

void Graph::addEdge(std::pair<std::string, std::string>&& inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName)
{
	addEdge(inEdgeAttribute, nodesByName.at(inSourceNodeName), nodesByName.at(inTargetNodeName));
}

void Graph::removeEdge(const std::shared_ptr<Edge> inEdge)
{
	const auto sourceNode = inEdge->getSourceNode();
	const auto targetNode = inEdge->getTargetNode();

	adjacencyList.at(sourceNode->getIndex(), targetNode->getIndex()) = 0;
	edgesByNodesIndex.erase({sourceNode->getIndex(), targetNode->getIndex()});
	edgesByNodesNames.erase({sourceNode->getName(), targetNode->getName()});

	const auto isSameEdgePredicate = [inEdge](const std::shared_ptr<Edge>& element){return element.get() == inEdge.get();};
	sourceNode->outgoingEdges.remove_if(isSameEdgePredicate);
	targetNode->incomingEdges.remove_if(isSameEdgePredicate);
}

void Graph::removeEdge(const int inSourceIndex, const int inTargetIndex)
{
	removeEdge(edgesByNodesIndex.at({inSourceIndex, inTargetIndex}));
}

void Graph::removeEdge(const std::string& inSourceNodeName, const std::string& inTargetNodeName)
{
	removeEdge(edgesByNodesNames.at({inSourceNodeName, inTargetNodeName}));
}

void Graph::saveAsDotFile(const std::string& inColor, const std::string& inFontColor, const std::string& inOutputPath, const bool inLogAdjacencyMatrix) const
{
	if(!std::filesystem::exists(inOutputPath))
	{
		std::filesystem::create_directories(inOutputPath);
	}
	assert(!std::filesystem::is_regular_file(inOutputPath));
	
	const auto fullPathWithoutExtension = inOutputPath + "/" + name;
	const auto dotFilePath = fullPathWithoutExtension + ".txt";
	if(std::ofstream file(dotFilePath, std::ios::out | std::ios::trunc); file)
	{
		std::list<std::string> nodesNames;
		for(const auto& [name, node] : nodesByName)
		{
			nodesNames.emplace_back(name);
		}

		nodesNames.sort();

		for(const auto& nodeName : nodesNames)
		{
			file << nodeName << std::endl;
		}

		file << std::endl << std::endl;

		std::map<std::string, std::list<std::string> > targetNamesBySourceNames;
		for(const auto& nodeName : nodesNames)
		{
			targetNamesBySourceNames[nodeName] = std::list<std::string>();
			for(const auto& [edgesNodesNames, edge] : edgesByNodesNames)
			{
				if(edgesNodesNames.first == nodeName)
				{
					targetNamesBySourceNames[nodeName].emplace_back(edgesNodesNames.second);
				}
			}
		}

		for(auto& [sourceName, targetNames] : targetNamesBySourceNames)
		{
			targetNames.sort();
			for(const auto& targetName : targetNames)
			{
				file << sourceName << " -> " << targetName << std::endl;
			}
		}

		file.close();
		return;
	}
	assert(false);
}


void Graph::getIsomorphicSubGraphs(const Graph& inSearchedGraph, std::list<std::list<std::shared_ptr<Node>>>& outFoundSubNodes) const
{
	//Find subNodes for each nodes of searched graph
	arma::mat m(inSearchedGraph.nodeCount, nodeCount, arma::fill::zeros);
	for(int row = 0; row < inSearchedGraph.nodeCount; ++row)
	{
		for(int col = 0; col < nodeCount; ++col)
		{
			if(const auto node = nodesByIndex.at(col), subNode = inSearchedGraph.nodesByIndex.at(row); node && subNode && node->incomingEdges.size() >= subNode->incomingEdges.size()
				&& node->outgoingEdges.size() >= subNode->outgoingEdges.size()
				&& node->isSubNode(*subNode))
			{
				m.at(row,col) = 1;
			}		
		}
	}

	//List found subNodesIndexes by searchedNodeIndexes
	std::vector<std::vector<int> > subNodesIndexes;
	subNodesIndexes.resize(inSearchedGraph.nodeCount);
	for(int row = 0; row < inSearchedGraph.nodeCount; ++row)
	{
		subNodesIndexes[row].reserve(nodeCount);
		for(int col = 0; col < nodeCount; ++col)
		{
			if(static_cast<int>(m.at(row,col)))
			{
				subNodesIndexes[row].emplace_back(col);
			}
		}
	}

	//Create all singleton combinations of subNodes
	std::vector<std::vector<int> > nodesCombinations;
	if(subNodesIndexes.size() > 1)
	{
		nodesCombinations = Math::customCartesianProduct(subNodesIndexes[0], subNodesIndexes[1]);
		for(auto i = 2; i < inSearchedGraph.nodeCount; ++i)
		{
			nodesCombinations = Math::customCartesianProduct(subNodesIndexes[i], nodesCombinations);
		}
	}
	else if(subNodesIndexes.size() == 1)
	{
		for(const auto& subNodeIndex : subNodesIndexes[0])
		{
			nodesCombinations.emplace_back(std::vector{subNodeIndex});	
		}
	}

	//Create ullman arrays
	std::vector<arma::mat > ullmanArrays;
	ullmanArrays.resize(nodesCombinations.size());
	auto currentArrayIndex = 0;
	for (auto& nodesCombination : nodesCombinations)
	{
		ullmanArrays[currentArrayIndex] = arma::mat(inSearchedGraph.nodeCount, nodeCount, arma::fill::zeros);
		for(size_t row = 0; row < nodesCombination.size(); ++row)
		{
			ullmanArrays[currentArrayIndex].at(row,nodesCombination[row]) = 1;
		}
		++currentArrayIndex;
	}

	//Checks for isomorphism
	std::list<std::list<std::shared_ptr<Node>>> isomorphNodes;
	for(const auto& ullmanArray : ullmanArrays)
	{
		bool isSubGraph = true;
		arma::mat ullmanMatrix = (ullmanArray * (ullmanArray * adjacencyList).t()).t();
		for(int i = 0; i < inSearchedGraph.nodeCount; ++i)
		{
			for(int j = 0; j < inSearchedGraph.nodeCount; ++j)
			{
				if(static_cast<int>(inSearchedGraph.adjacencyList.at(i,j)) && !static_cast<int>(ullmanMatrix.at(i,j)))
				{
					isSubGraph = false;
					break;
				}
			}
		}

		if(isSubGraph)
		{
			std::list<std::shared_ptr<Node> > foundSubNodes; 
			for(auto row = 0; row < inSearchedGraph.nodeCount; ++row)
			{
				for(auto col = 0; col < nodeCount; ++col)
				{
					if(static_cast<int>(ullmanArray.at(row,col)) == 1)
					{
						foundSubNodes.emplace_back(nodesByIndex.at(col));
					}
				}
			}
			isomorphNodes.push_back(foundSubNodes);
		}
	}

	//Ensure returned combinations are correct (due to the cartisian product done earlier, some of them don't answer the direction and label of edges
	for(auto& subNodes : isomorphNodes)
	{
		//First we check all nodes two by two
		bool isCorrect = true;
		auto subNode = subNodes.begin();
		for(auto i = 0; i < static_cast<int>(subNodes.size()) - 1; ++i)
		{
			auto previousNode = subNode;
			++subNode;

			const auto nextIndex = i + 1;
			
			if(static_cast<int>(inSearchedGraph.adjacencyList.at(i, nextIndex)))
			{
				if(auto edge = inSearchedGraph.edgesByNodesIndex.at({i, nextIndex}); !Node::containsEdges({edgesByNodesIndex.at({previousNode->get()->index, subNode->get()->index})}, {edge}))
				{
					isCorrect = false;
					break;
				}
			}

			if(static_cast<int>(inSearchedGraph.adjacencyList.at(nextIndex, i)))
			{
				if(auto edge = inSearchedGraph.edgesByNodesIndex.at({nextIndex, i}); !Node::containsEdges({edgesByNodesIndex.at({subNode->get()->index, previousNode->get()->index})}, {edge}))
				{
					isCorrect = false;
					break;
				}
			}
		}

		//Then we check the first one with the last one
		if(isCorrect && inSearchedGraph.nodeCount > 1)
		{
			const auto lastNode = --subNodes.end();
			const auto beforeLastNode  = --(--subNodes.end());
			const int lastIndex = static_cast<int>(subNodes.size() - 1);
			const int beforeLastIndex = static_cast<int>(subNodes.size() - 2);
			
			if(static_cast<int>(inSearchedGraph.adjacencyList.at(beforeLastIndex, lastIndex)))
			{
				if(!Node::containsEdges({edgesByNodesIndex.at({beforeLastNode->get()->index, lastNode->get()->index})}, {inSearchedGraph.edgesByNodesIndex.at({beforeLastIndex, lastIndex})}))
				{
					isCorrect = false;
				}
			}

			if(isCorrect)
			{
				if(static_cast<int>(inSearchedGraph.adjacencyList.at( lastIndex, beforeLastIndex)))
				{
					if(!Node::containsEdges({edgesByNodesIndex.at({ lastNode->get()->index, beforeLastNode->get()->index})}, {inSearchedGraph.edgesByNodesIndex.at({lastIndex, beforeLastIndex})}))
					{
						isCorrect = false;
					}
				}
			}
		}
		if(isCorrect)
		{
			outFoundSubNodes.emplace_back(subNodes);
		}
	}
}