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
	std::unordered_map<std::string, std::string> attributes;
	std::string sourceNodeName;
	std::string targetNodeName;
	int index;
};

class Graph
{
  public:
    explicit Graph(std::string inName = "none");
    explicit Graph(const pugi::xml_node& parsedXml);
	void addNode(Node inNode);
    void addEdge(std::pair<std::string, std::string> inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName);
    void saveAsDotFile(const std::string& inOutputPath, bool inLogAdjacencyMatrix = false) const;

private:
	std::string name;
	int nodeCount;
	int edgeCount;
	std::unordered_map<std::string, Node> nodes;
    ChunkMatrix<Edge> adjacencyList;
};

#endif // GRAPH_H
