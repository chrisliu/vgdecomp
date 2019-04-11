//
//  Graph.cpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#include "Graph.hpp"

int Graph::getIndex(int id) {
	Node comp = Node(id);

	std::vector<Node*>::iterator itr = std::find(vertices.begin(), vertices.end(), &comp);

	if (itr != vertices.end()) {					 // If it's in the vector
		return std::distance(vertices.begin(), itr);
	}

	return -1;
}

Node* Graph::getNode(int id) {
	int index = getIndex(id);
	if (index == -1) {
		return nullptr;
	}

	return vertices.at(index);
}

void Graph::addEdge(int id1, int id2, int edgeType) {
	Node* n1 = getNode(id1); Node* n2 = getNode(id2);

	if (n1 != nullptr && n2 != nullptr) {
		Edge edge = Edge(n1, n2, edgeType);
		n1->addEdge(&edge);
		n2->addEdge(&edge);
	}
}

void Graph::addVertex(int id) {
	Node vertex = Node(id);
	vertices.push_back(&vertex);
}

void Graph::addUndirectedEdge(int id1, int id2) {
	addEdge(id1, id2, Edge::UNDIRECTED);
}

void Graph::addDirectedEdge(int id1, int id2) {
	addEdge(id1, id2, Edge::DIRECTED_TO);
}

std::vector<Node*> Graph::getConnectedVertices(int id) {
	Node* node = getNode(id);

	if (node == nullptr) {
		return std::vector<Node*>();
	}

	std::vector<Node*> outgoingVertices = node->getOutgoingNodes();
	std::vector<Node*> incomingVertices = node->getIncomingNodes();

	std::vector<Node*> connected;
	connected.reserve(outgoingVertices.size() + incomingVertices.size());
	connected.insert(connected.end(), outgoingVertices.begin(), outgoingVertices.end());
	connected.insert(connected.end(), incomingVertices.begin(), incomingVertices.end());

	return connected;
}