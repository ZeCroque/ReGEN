// ReSharper disable CppInconsistentNaming
#ifndef GRAPH_H
#define GRAPH_H

#include <armadillo>

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
struct ConditionsBlock;

class Node
{
friend class Graph;
public:
	Node();
	Node(std::string inName, std::unordered_map<std::string, NodeAttribute> inAttributes);
	
	[[nodiscard]] const std::string& getName() const;
	void setName(const std::string& inName);
	[[nodiscard]] const std::unordered_map<std::string, NodeAttribute>& getAttributes() const;
	[[nodiscard]] const NodeAttribute& getAttribute(const std::string& inAttributeName) const;
	void setAttribute(const std::string& inAttributeName, const NodeAttribute& inAttributeValue);
	[[nodiscard]] std::shared_ptr<ConditionsBlock> getConditionsBlock() const;
	void setConditionsBlock(ConditionsBlock& inConditionsBlock) const;
[[nodiscard]] const std::list<std::shared_ptr<Edge>>& getIncomingEdges() const;
	[[nodiscard]] const std::list<std::shared_ptr<Edge>>& getOutgoingEdges() const;
	[[nodiscard]] int getIndex() const;
	[[nodiscard]] bool isValid() const;

	void clearEdges();
	std::shared_ptr<Node> getSourceNodeFromIncomingEdgeWithAttribute(const std::pair<std::string, std::string>& inAttribute);
	std::shared_ptr<Node> getTargetNodeFromOutgoingEdgeWithAttribute(const std::pair<std::string, std::string>& inAttribute);
	bool isSubNode(const Node& inParentNode);
	bool containsAttributes(const Node& inParentNode);
	static bool containsEdges(const std::list<std::shared_ptr<Edge> >& inNodeEdges, const std::list<std::shared_ptr<Edge> >& inParentNodeEdges);
	void validateNode(struct Conditions preConditions, bool inValid);

private:	
	std::string name;
	std::unordered_map<std::string, NodeAttribute> attributes;
	mutable std::shared_ptr<ConditionsBlock> conditionsBlock; //TODO complex template dev to prevent non-story graphs from having conditions
	bool bIsValid; //TODO complex template dev to prevent non-story graphs from having validation

	std::list<std::shared_ptr<Edge>> incomingEdges;
	std::list<std::shared_ptr<Edge>> outgoingEdges;
	int index;
};

class Edge
{
	friend class Graph;

public:
	Edge() = default;
	Edge(std::shared_ptr<Node> inSourceNode, std::shared_ptr<Node> inTargetNode, std::unordered_map<std::string, std::string> inAttributes);

	[[nodiscard]] std::shared_ptr<Node> getSourceNode() const;
	[[nodiscard]] std::shared_ptr<Node> getTargetNode() const;
	[[nodiscard]] const std::unordered_map<std::string, std::string>& getAttributes() const;

#ifndef NDEBUG
	void print() const;
#endif

private:
	std::shared_ptr<Node> sourceNode;
	std::shared_ptr<Node> targetNode;
	std::unordered_map<std::string, std::string> attributes;
};

class Graph
{
  public:
	Graph();
    Graph(std::string inName, std::string inType);

	[[nodiscard]] const std::string& getName() const;
	[[nodiscard]] const std::string& getType() const;
	[[nodiscard]] std::shared_ptr<Node> getNodeByName(const std::string& inName) const;
	[[nodiscard]] std::shared_ptr<Node> getNodeByIndex(int inIndex) const;
	[[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<Node>>& getNodesByName() const;
	[[nodiscard]] const std::map<int, std::shared_ptr<Node>>& getNodesByIndex() const;
	[[nodiscard]] const std::map<std::pair<std::string, std::string>, std::shared_ptr<Edge>>& getEdgesByNodesNames() const;
	[[nodiscard]] const std::map<std::pair<int, int>, std::shared_ptr<Edge>>& getEdgesByNodesIndex() const;
	[[nodiscard]] int getNodeCount() const;
	[[nodiscard]] int getEdgeCount() const;
	
	void loadFromXml(const pugi::xml_node& inParsedXml);
	void loadFromXml(const std::string& inPath);
	std::shared_ptr<Node> addNode(Node* inNode);
	void removeNode(std::shared_ptr<Node> inNodeToRemove) const;
	void addEdge(std::pair<std::string, std::string> inEdgeAttribute, std::shared_ptr<Node> inSourceNode, std::shared_ptr<Node> inTargetNode);
	void addEdge(std::pair<std::string, std::string>&& inEdgeAttribute, int inSourceIndex, int inTargetIndex);
    void addEdge(std::pair<std::string, std::string>&& inEdgeAttribute, const std::string& inSourceNodeName, const std::string& inTargetNodeName);
	void removeEdge(const std::shared_ptr<Edge> inEdge);
	void removeEdge(int inSourceIndex, int inTargetIndex);
	void removeEdge(const std::string& inSourceNodeName, const std::string& inTargetNodeName);
    void saveAsDotFile(const std::string& inColor = "ivory4", const std::string& inFontColor = "ivory4", const std::string& inOutputPath = "./Output/ParityTest", bool inLogAdjacencyMatrix = false) const;
	void getIsomorphicSubGraphs(const Graph& inSearchedGraph, std::list<std::list<std::shared_ptr<Node>>>& outFoundSubNodes) const;


private:	
	std::string name;
	std::string type;
	int nodeCount;
	int edgeCount;
	mutable std::unordered_map<std::string, std::shared_ptr<Node> > nodesByName;
	mutable std::map<int, std::shared_ptr<Node> > nodesByIndex;
	mutable std::map<std::pair<std::string, std::string>, std::shared_ptr<Edge> > edgesByNodesNames;
	mutable std::map<std::pair<int, int>, std::shared_ptr<Edge> > edgesByNodesIndex;
    arma::mat adjacencyList;
};

#endif // GRAPH_H

