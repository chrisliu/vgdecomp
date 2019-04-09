//
//  Node.cpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#include "Node.hpp"

void Node::addEdge(Edge* edge) {
	edges.push_back(edge);

	Node* other = edge.getConnectedNode(this);
	if (edge.isChild(this)) {
		incomingNodes.push_back(other);
	} else {
		outgoingNodes.push_back(other);
	}
}

std::vector Node::getOutgoingNodes() {
	return outgoingNodes;
}

std::vector Node::getIncomingNodes() {
	return incomingNodes;
}

bool Node::operator==(const Node& other) {
	return this->id == other.id;
}
