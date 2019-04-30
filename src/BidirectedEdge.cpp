#include "BidirectedEdge.hpp"

BidirectedEdge::BidirectedEdge(nid_t id1, nid_t id2, bool from_left, bool to_right) 
: id1(id1), id2(id2), from_left(from_left), to_right(to_right) {
    if (id2 < id1) { // Keep smaller id as id1
        this->id1       = id2;
        this->id2       = id1;
        this->from_left = to_right;
        this->to_right  = from_left;
    }
}

bool BidirectedEdge::operator==(const BidirectedEdge &other) {
    return (this->id1 == other.id1 && this->id2 == other.id2 &&
            this->from_left == other.from_left && this->to_right == other.to_right);
}