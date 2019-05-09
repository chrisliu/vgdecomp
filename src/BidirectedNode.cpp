#include "BidirectedNode.hpp"

BidirectedNode::BidirectedNode(nid_t id)
: id(id) {

}

bool BidirectedNode::operator==(const BidirectedNode& other) {
    return this->id == other.id;
}