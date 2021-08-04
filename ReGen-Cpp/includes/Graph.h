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

class Edge;

class Node
{
	friend class Graph; //TODO getter/setter

public:
	Node();
	Node(std::string inName, std::string inModificationName, std::unordered_map<std::string, NodeAttribute> inAttributes);
	bool isSubNode(const Node& inParentNode);
	bool containsAttributes(const Node& inParentNode);
	static bool containsEdges(const std::list<std::shared_ptr<Edge> >& inNodeEdges, const std::list<std::shared_ptr<Edge> >& inParentNodeEdges);

private:	
	std::string name;
	std::string modificationName;
	std::unordered_map<std::string, NodeAttribute> attributes;

	std::list<std::shared_ptr<Edge>> incomingEdges;
	std::list<std::shared_ptr<Edge>> outgoingEdges;
	int index;
};

class Edge
{
	friend class Graph; //TODO getter/setter
	friend class Node;

public:
	Edge() = default;

private:
	std::shared_ptr<Node> sourceNode;
	std::shared_ptr<Node> targetNode;
	std::unordered_map<std::string, std::string> attributes;
};

template<class T> class Matrix;

class Graph
{
  public:
	Graph();
    Graph(std::string inName, std::string inType);
	void loadFromXml(const pugi::xml_node& inParsedXml);
	void loadFromXml(const std::string& inPath);
	void addNode(Node* inNode);
    void addEdge(std::pair<std::string, std::string> inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName);
    void saveAsDotFile(const std::string& inColor = "ivory4", const std::string& inFontColor = "ivory4", const std::string& inOutputPath = "./Output", bool inLogAdjacencyMatrix = false) const;
	void getIsomorphicSubGraphs(const Graph& inSearchedGraph, std::list<Graph>& outFoundSubGraphs);

private:
	std::shared_ptr<Matrix<int> > multiplyAdjacencyListBy(const Matrix<int>& inMatrix);
	
	std::string name;
	std::string type;
	int nodeCount;
	int edgeCount;
	std::unordered_map<std::string, std::shared_ptr<Node> > nodesByName;
	std::map<int, std::shared_ptr<Node> > nodesByIndex;
    ChunkMatrix<std::shared_ptr<Edge> > adjacencyList;
};

#endif // GRAPH_H

