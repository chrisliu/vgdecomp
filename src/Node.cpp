#include "Node.hpp"

Node::Node(int64_t id) 
: id(id) {

}

void Node::addEdge(Edge* edge) {
	connectedEdges.push_back(edge);
}

bool Node::operator==(const Node& other) {
	return this->id == other.id;
}