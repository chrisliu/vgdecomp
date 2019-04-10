//
//  Edge.cpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#include "Edge.hpp"

const int Edge::DIRECTED_FROM = 0;
const int Edge::DIRECTED_TO   = 1;
const int Edge::UNDIRECTED    = 2;

Edge::Edge(Node* n1, Node* n2, int edgeType) {
	this->edgeType = edgeType;

	nodes.push_back(n1);
	nodes.push_back(n2);
}

Edge::~Edge() {
	nodes.clear();
	nodes.shrink_to_fit();
}

/** Returns whether or not the given node is a child of the edge
	ie. if the edge is n1->n2, then n1 is a child
	Note: In the case of an undirected edge both nodes are children
*/
bool Edge::isChild(Node* n) {
	std::vector<Node*>::iterator itr = std::find(nodes.begin(), nodes.end(), n);

	if (itr != nodes.end()) {
		int idx = std::distance(nodes.begin(), itr);
		if (idx == edgeType) {
			return true;
		}
	}

	return false;
}

/** Returns the other node connected by the same edge
*/
Node* Edge::getConnectedNode(Node *n) {
	if (isChild(n)) {
		return nodes.at(1 - edgeType);
	}

	return nodes.at(edgeType);
}