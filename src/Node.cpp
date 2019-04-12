#include "Node.hpp"

Node::Node(int64_t id) 
: id(id) {

}

bool Node::operator==(const Node& other) {
	return this->id == other.id;
}