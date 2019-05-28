#ifndef BidirectedNode_hpp
#define BidirectedNode_hpp

#include "handlegraph/types.hpp"

class BidirectedNode {
    public:
        nid_t id;
        BidirectedNode(nid_t id);
        bool operator==(const BidirectedNode& other);
};

#endif /* BidirectedNode_hpp */