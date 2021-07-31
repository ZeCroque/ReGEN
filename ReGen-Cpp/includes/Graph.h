// ReSharper disable CppInconsistentNaming
#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>

#include "ChunkMatrix.h"

namespace pugi
{
	class xml_node;
}

constexpr int NONE = -1;

struct NodeAttribute
{
	std::string type;
	std::string value;
};

class Node
{
	friend class Graph;

public:
	Node();
	Node(std::string inName, std::string inModificationName, std::unordered_map<std::string, NodeAttribute> inAttributes);

private:	
	std::string name;
	std::string modificationName;
	std::unordered_map<std::string, NodeAttribute> attributes;

	int index;
};

class Edge
{
	friend class Graph;

public:
	Edge();

private:
	std::string sourceNodeName;
	std::string targetNodeName;
	std::unordered_map<std::string, std::string> attributes;
	int index;
};

class Graph
{
  public:
	Graph();
    Graph(std::string inName, std::string inType);
	void loadFromXml(const pugi::xml_node& inParsedXml);
	void loadFromXml(const std::string& inPath);
	void addNode(Node inNode);
    void addEdge(std::pair<std::string, std::string> inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName);
    void saveAsDotFile(const std::string& inColor = "ivory4", const std::string& inFontColor = "ivory4", const std::string& inOutputPath = "./Output", bool inLogAdjacencyMatrix = false) const;

private:
	std::string name;
	std::string type;
	int nodeCount;
	int edgeCount;
	std::unordered_map<std::string, Node> nodes;
    ChunkMatrix<Edge> adjacencyList;
};

#endif // GRAPH_H
