//
//  Node.cpp
//  
//
//  Created by Chris Liu on 4/9/19.
//

#include "Node.hpp"

Node::Node(int id) {
	this->id = id;
}

/** Adds an edge and updates incoming and outgoing nodes
*/
void Node::addEdge(Edge* edge) {
	edges.push_back(edge);

	Node* other = edge->getConnectedNode(this);

	if (edge->edgeType == Edge::UNDIRECTED) {   // If it's an undirected edge
		incomingNodes.push_back(other);
		outgoingNodes.push_back(other);
		return;
	}

	if (edge->isChild(this)) {					// If the edge has a direction
		incomingNodes.push_back(other);			// If the current node is a child, then other is it's parent
	} else {
		outgoingNodes.push_back(other);		    // Otherwise, other is this node's child
	}
}

std::vector<Node*> Node::getOutgoingNodes() {
	return outgoingNodes;
}

std::vector<Node*> Node::getIncomingNodes() {
	return incomingNodes;
}

/** Overload == operator to match nodes based on assigned id
*/
bool Node::operator==(const Node& other) {
	return this->id == other.id;
}
