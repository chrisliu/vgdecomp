#include "Edge.hpp"

Edge::Edge(int64_t id1, int64_t id2, bool isBackwards = false) 
: isBackwards(isBackwards) {

}

bool Edge::reverse() {
	return false;
}

int64_t Edge::getUpstreamId() {
	return -1;
}

int64_t Edge::getDownstreamId() {
	return -1;
}

int64_t Edge::traverse(int64_t id) {
	return -1;
}

bool Edge::operator==(const Edge& other) {

}