#include "Graph.h"

#include <filesystem>
#include <fstream>
#include <pugixml.hpp>

#include "Utils.h"

Node::Node() : index(NONE)
{
}

Node::Node(std::string inName, std::string inModificationName, std::unordered_map<std::string, NodeAttribute> inAttributes) : name(std::move(inName)), modificationName(std::move(inModificationName)), attributes(std::move(inAttributes)), index(NONE)
{
}

const std::string& Node::getName() const
{
	return name;
}

const std::string& Node::getModificationName() const
{
	return modificationName;
}

const NodeAttribute& Node::getAttribute(const std::string& inAttributeName) const
{
	return attributes.at(inAttributeName);
}

void Node::setAttribute(const std::string& inAttributeName, const NodeAttribute& inAttributeValue)
{
	attributes[inAttributeName] = inAttributeValue;
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
			if((parentAttributeName == "N/A" || parentAttributeName == attributeName) && (parentAttributeData.value == "N/A" || parentAttributeData.value == attributeData.value))
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
				std::none_of(foundEdges.begin(), foundEdges.end(), [edge](const std::shared_ptr<Edge>& e)
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
				std::string(),
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

void Graph::addNode(Node* inNode)
{
	inNode->index = nodeCount;
	
	const std::shared_ptr<Node> newNode(inNode);
	nodesByName.insert({inNode->name, newNode});
	nodesByIndex.insert({nodeCount, newNode});

	++nodeCount;
}

void Graph::addEdge(std::pair<std::string, std::string> inEdgeAttribute, std::shared_ptr<Node> inSourceNode, std::shared_ptr<Node> inTargetNode)
{
	assert(inSourceNode.get() != inTargetNode.get());
	auto edge = edgesByNodesNames[{inSourceNode->name, inTargetNode->name}];
	if(!edge)
	{	
		edge.reset(new Edge());
		edge->sourceNode = inSourceNode;
		edge->targetNode = inTargetNode;	
		inSourceNode->outgoingEdges.emplace_back(edge);
		inTargetNode->incomingEdges.emplace_back(edge);
		
		edgesByNodesNames[{inSourceNode->name, inTargetNode->name}] = edge;
		
		const auto sourceNodeIndex = inSourceNode->index;
		const auto targetNodeIndex = inTargetNode->index;

		if(adjacencyList.size() <= sourceNodeIndex || adjacencyList.size() <= targetNodeIndex)
		{
			adjacencyList.resize(std::max(sourceNodeIndex, targetNodeIndex) + 1);
		}
		
		adjacencyList.at(sourceNodeIndex, targetNodeIndex) = 1;
		edgesByNodesIndex[{sourceNodeIndex, targetNodeIndex}] = edge;
		
		++edgeCount;
	}
	edge->attributes.insert(std::move(inEdgeAttribute));
}

void Graph::addEdge(std::pair<std::string, std::string> inEdgeAttribute, const int inSourceIndex, const int inTargetIndex)
{
	addEdge(inEdgeAttribute, nodesByIndex.at(inSourceIndex), nodesByIndex.at(inTargetIndex));
}

void Graph::addEdge(std::pair<std::string, std::string> inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName)
{
	addEdge(inEdgeAttribute, nodesByName.at(inSourceNodeName), nodesByName.at(inTargetNodeName));
}

void Graph::saveAsDotFile(const std::string& inColor, const std::string& inFontColor, const std::string& inOutputPath, const bool inLogAdjacencyMatrix) const
{
	if(!std::filesystem::exists(inOutputPath))
	{
		std::filesystem::create_directories(inOutputPath);
	}
	assert(!std::filesystem::is_regular_file(inOutputPath));
	
	const auto fullPathWithoutExtension = inOutputPath + "/" + name;
	const auto dotFilePath = fullPathWithoutExtension + ".dot";
	if(std::ofstream file(dotFilePath, std::ios::out | std::ios::trunc); file)
	{
		file << "digraph " << name << " {" << std::endl << "node [shape = \"record\"]" << std::endl;

		for(const auto& [name, node] : nodesByName)
		{
			file << name <<  "[label=\"{" << name << "|";
			for(const auto& [name, nodeAttribute] : node->attributes)
			{
				file << name << "=" << nodeAttribute.value << "\\l";  
			}
			file << "}\"] [color=" << inColor << " fontcolor=" << inFontColor << "]" << std::endl;
			
		}
		
		for(auto j = 0; j < nodeCount; ++j)
		{
			for(auto i = 0; i < nodeCount; ++i)
			{
				const auto edge = edgesByNodesIndex[std::pair{i, j}];
#ifndef NDEBUG
				if(inLogAdjacencyMatrix)
				{
					PRINT(edge);
				}
#endif
				if(edge)
				{
					for(const auto& [attribute, value] : edge->attributes)
					{
						file << edge->sourceNode->name << " -> " << edge->targetNode->name;
						if (attribute != "none")
						{
							file << " [label=" << "\"{'" << attribute << "' : '" << value << "'}\"] [color=" << inColor << " fontcolor=" << inFontColor << "]"; 
						}
						file << std::endl;
					}
				}
			}
#ifndef NDEBUG
			if(inLogAdjacencyMatrix)
			{
				PRINTLN("");
			}
#endif
		}
		file << "}";
		file.close();
		system(("dot -Tpng " + dotFilePath + " -o " + fullPathWithoutExtension + ".png").c_str());
		return;
	}
	assert(false);
}


void Graph::getIsomorphicSubGraphs(const Graph& inSearchedGraph, std::list<std::list<std::shared_ptr<Node>>>& outFoundSubGraphs) const
{
	//Find subNodes for each nodes of searched graph
	arma::mat m(inSearchedGraph.nodeCount, nodeCount, arma::fill::zeros);
	for(int row = 0; row < inSearchedGraph.nodeCount; ++row)
	{
		for(int col = 0; col < nodeCount; ++col)
		{
			if(const auto node = nodesByIndex.at(col), subNode = inSearchedGraph.nodesByIndex.at(row); node->incomingEdges.size() >= subNode->incomingEdges.size()
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
			outFoundSubGraphs.push_back(foundSubNodes);
		}
	}
}