#ifndef BidirectedEdge_hpp
#define BidirectedEdge_hpp

#include "../deps/handlegraph/types.hpp"

using namespace handlegraph;

class BidirectedEdge {
    public:
        nid_t id1;
        nid_t id2;
        bool from_left;
        bool to_right;
        BidirectedEdge(nid_t id1_, nid_t id2_, 
            bool from_left_ = false, bool to_right_ = false) :
            id1(id1_), id2(id2_), from_left(from_left_), to_right(to_right_) {}
        bool operator==(const BidirectedEdge& other) {
            return (id1 == other.id1 && id2 == other.id2 && from_left == other.from_left && to_right == other.to_right) ||
                (id1 == other.id2 && id2 == other.id1 && from_left == to_right && to_right == from_left);
        }
};

#endif /* BidirectedEdge_hpp */