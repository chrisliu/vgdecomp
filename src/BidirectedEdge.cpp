#include "BidirectedEdge.hpp"

BidirectedEdge::BidirectedEdge(nid_t id1, nid_t id2, bool from_left, bool to_right) 
: id1(id1), id2(id2), from_left(from_left), to_right(to_right) {

}

bool BidirectedEdge::operator==(const BidirectedEdge &other) {
    return ((this->id1 == other.id1 && this->id2 == other.id2 && // When they're the same "direction"
             this->from_left == other.from_left && this->to_right == other.to_right) ||
            (this->id1 == other.id2 && this->id2 == other.id1 && // When they're the opposite "direction"
             this->from_left == other.to_right && this->to_right == other.from_left));
}