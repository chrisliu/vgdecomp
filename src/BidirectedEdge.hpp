#ifndef BidirectedEdge_hpp
#define BidirectedEdge_hpp

#include "handlegraph/types.hpp"

class BidirectedEdge {
    public:
        nid_t id1;
        nid_t id2;
        bool from_left;
        bool to_right;
        BidirectedEdge(nid_t id1, nid_t id2, 
            bool from_left = false, bool to_right = false);
        bool operator==(const BidirectedEdge& other);
};

#endif /* BidirectedEdge_hpp */