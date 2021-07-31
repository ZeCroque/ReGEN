#include "Graph.h"

#include <filesystem>
#include <fstream>
#include <pugixml.hpp>

Node::Node() : index(NONE)
{
}

Node::Node(std::string inName, std::string inModificationName, std::unordered_map<std::string, NodeAttribute> inAttributes) : name(std::move(inName)), modificationName(std::move(inModificationName)), attributes(std::move(inAttributes)), index(NONE)
{
}

Edge::Edge() : index(NONE)
{
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
			nodeAttributes.insert({
				nodeAttribute.attribute("name").as_string(),
				NodeAttribute{
					nodeAttribute.attribute("type").as_string(),
					nodeAttribute.first_child().text().as_string()
				}
			});
		}
		
		addNode(
			Node(
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
			addEdge(
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

void Graph::addNode(Node inNode)
{
	inNode.index = nodeCount;
	++nodeCount;
	nodes.insert({inNode.name, std::move(inNode)});
}

void Graph::addEdge(std::pair<std::string, std::string> inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName)
{
	assert(inSourceNodeName != inTargetNodeName);

	const auto sourceNodeIndex = nodes.at(inSourceNodeName).index;
	const auto targetNodeIndex = nodes.at(inTargetNodeName).index;
	auto& edge = adjacencyList.getValueAt(sourceNodeIndex, targetNodeIndex);
	if(edge.index == NONE)
	{		
		edge.index = edgeCount;
		++edgeCount;
	}

	edge.attributes.insert(std::move(inEdgeAttribute));
	edge.sourceNodeName = inSourceNodeName;
	edge.targetNodeName = inTargetNodeName;
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

		for(const auto& [name, node] : nodes)
		{
			file << name <<  "[label=\"{" << name << "|";
			for(const auto& [name, nodeAttribute] : node.attributes)
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
				const auto edgeExists = edge.index != NONE;
				if(inLogAdjacencyMatrix)
				{
					PRINT(edgeExists);
				}
				if(edgeExists)
				{
					for(const auto& [attribute, value] : edge.attributes)
					{
						file << edge.sourceNodeName << " -> " << edge.targetNodeName;
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
