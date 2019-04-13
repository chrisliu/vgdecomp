#include "Edge.hpp"

Edge::Edge(int64_t id1, int64_t id2, bool isDirected, bool isBackwards = false) 
: isDirected(isDirected), isBackwards(isBackwards) {
	nodes.push_back(id1);
	nodes.push_back(id2);
}

bool Edge::reverse() {
	isBackwards = !isBackwards;
	return isBackwards;
}

int64_t Edge::getUpstreamId() {
	if (!isDirected) {
		return nodes.at(0);
	}

	return nodes.at(isBackwards);
}

int64_t Edge::getDownstreamId() {
	if (!isDirected) {
		return nodes.at(1);
	}

	return nodes.at(!isBackwards);
}

int64_t Edge::traverse(int64_t id) {
	if (!isDirected) {
		bool isFirst = nodes.at(0) == id;
		return nodes.at(isFirst);
	}

	if (getUpstreamId() == id) {
		return getDownstreamId();
	}

	return -1;
}

bool Edge::operator==(const Edge& other) {
	if (this->isDirected != other.isDirected) {
		return false;
	}

	if (!this->isDirected) {
		// Could be improved by some intersection algorithm for vectors
		return this->nodes.at(0) == other.nodes.at(0) ||
			   this->nodes.at(0) == other.nodes.at(1) ||
			   this->nodes.at(1) == other.nodes.at(0) ||
			   this->nodes.at(1) == other.nodes.at(1);
	}

	return this->getUpstreamId() == other.nodes.at(other.isBackwards);
}