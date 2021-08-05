#include "Graph.h"

#include <filesystem>
#include <fstream>
#include <pugixml.hpp>

#include "Matrix.h"
#include "Utils.h"


Node::Node() : index(NONE)
{
}

Node::Node(std::string inName, std::string inModificationName, std::unordered_map<std::string, NodeAttribute> inAttributes) : name(std::move(inName)), modificationName(std::move(inModificationName)), attributes(std::move(inAttributes)), index(NONE)
{
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
				for(const auto& [parentNodeEdgeAttributeName, parentNodeEdgeAttributeValue] : parentNodeEdge->attributes) //Then we iterate through each attributes of the parentEdge
				{
					bool containsEdgeAttribute = false;
					for(const auto& [edgeAttributeName, edgeAttributeValue] : edge->attributes) //And compare it to all attributes of the currently processed main edge
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

Graph::Graph() : name("none"), type("default"), nodeCount(0), edgeCount(0)
{
}

Graph::Graph(std::string inName, std::string inType) : name(std::move(inName)), type(std::move(inType)), nodeCount(0), edgeCount(0)
{
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

void Graph::addEdge(std::pair<std::string, std::string> inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName)
{
	assert(inSourceNodeName != inTargetNodeName);

	auto sourceNode = nodesByName.at(inSourceNodeName);
	auto targetNode = nodesByName.at(inTargetNodeName);
	
	const auto sourceNodeIndex = sourceNode->index;
	const auto targetNodeIndex = targetNode->index;
	auto edge = adjacencyList.getValueAt(sourceNodeIndex, targetNodeIndex);
	if(!edge)
	{	
		edge.reset(new Edge());
		adjacencyList.setValueAt(edge, sourceNodeIndex, targetNodeIndex);
		edge->sourceNode = sourceNode;
		edge->targetNode = targetNode;	
		sourceNode->outgoingEdges.emplace_back(edge);
		targetNode->incomingEdges.emplace_back(edge);
		++edgeCount;
	}
	edge->attributes.insert(std::move(inEdgeAttribute));

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
				const auto edge = adjacencyList.getValueAt(i, j);
				if(inLogAdjacencyMatrix)
				{
					PRINT(edge);
				}
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
			if(inLogAdjacencyMatrix)
			{
				PRINTLN("");
			}
		}
		file << "}";
		file.close();
		system(("dot -Tpng " + dotFilePath + " -o " + fullPathWithoutExtension + ".png").c_str());
		return;
	}
	assert(false);
}


void Graph::getIsomorphicSubGraphs(const Graph& inSearchedGraph, std::list<Graph>& outFoundSubGraphs)
{
	auto s_graph_nodes = inSearchedGraph.nodesByName; 
	
	const int p_a = inSearchedGraph.nodeCount;
	const int p_b = nodeCount;

	int** m = new int*[p_a];
	for(auto i = 0; i < p_a; ++i)
	{
		m[i] = new int[p_b];
		for(int j = 0; j < p_b; ++j)
		{
			m[i][j] = 0;
		}
	}
	
	for(int row = 0; row < p_a; ++row)
	{
		for(int col = 0; col < p_b; ++col)
		{
			if(const auto node = nodesByIndex.at(col), subNode = inSearchedGraph.nodesByIndex.at(row); node->incomingEdges.size() >= subNode->incomingEdges.size()
				&& node->outgoingEdges.size() >= subNode->outgoingEdges.size()
				&& node->isSubNode(*subNode))
			{
				m[row][col] = 1;
			}		
		}
	}

	std::vector<std::vector<int> > subNodesIndexes;
	subNodesIndexes.resize(p_a);
	for(int row = 0; row < p_a; ++row)
	{
		subNodesIndexes[row].reserve(p_b);
		for(int col = 0; col < p_b; ++col)
		{
			if(m[row][col])
			{
				subNodesIndexes[row].emplace_back(col);
			}
		}
	}
	
	auto nodesCombinations = Math::customCartesianProduct(subNodesIndexes[0], subNodesIndexes[1]);
	for(auto i = 2; i < p_a; ++i)
	{
		nodesCombinations = Math::customCartesianProduct(subNodesIndexes[i], nodesCombinations);
	}

	std::vector<Matrix<int> > ullmanArrays;
	ullmanArrays.resize(nodesCombinations.size());

	auto currentArrayIndex = 0;
	for (auto& nodesCombination : nodesCombinations)
	{
		ullmanArrays[currentArrayIndex] = Matrix<int>(p_a, p_b);
		for(size_t row = 0; row < nodesCombination.size(); ++row)
		{
			ullmanArrays[currentArrayIndex][row][nodesCombination[row]] = 1;
		}
		++currentArrayIndex;
	}

	for(const auto& ullmanArray : ullmanArrays)
	{
		bool isSubGraph = true;
		const auto ullmanMatrix = (ullmanArray * *multiplyAdjacencyListBy(ullmanArray)->transpose())->transpose();
		for(int i = 0; i < p_a; ++i)
		{
			for(int j = 0; j < p_a; ++j)
			{
				if(inSearchedGraph.adjacencyList.getValueAt(i,j) && !((*ullmanMatrix)[i][j] == 1))
				{
					isSubGraph = false;
					break;
				}
			}
		}
		PRINTLN(isSubGraph);
	}
}

std::shared_ptr<Matrix<int> > Graph::multiplyAdjacencyListBy(const Matrix<int>& inMatrix)
{
	assert(inMatrix.getColumnsCount() == static_cast<size_t>(nodeCount));
	auto result = std::make_shared<Matrix<int> >(inMatrix.getRowsCount(), inMatrix.getColumnsCount());
	for(size_t i = 0; i < inMatrix.getRowsCount(); ++i)    
	{    
		for(int j = 0; j < nodeCount; ++j)    
		{    
			(*result)[i][j] = 0;    
			for(size_t k = 0; k < inMatrix.getColumnsCount(); ++k)    
			{    
				(*result)[i][j] += inMatrix[i][k] * (adjacencyList.getValueAt(k,j) != nullptr);    
			}    
		}    
	}
	return result;
}
