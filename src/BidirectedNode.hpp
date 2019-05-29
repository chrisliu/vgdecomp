#ifndef BidirectedNode_hpp
#define BidirectedNode_hpp

#include "handlegraph/types.hpp"

using namespace handlegraph;

class BidirectedNode {
    public:
        nid_t id;
        BidirectedNode(nid_t id);
        bool operator==(const BidirectedNode& other);
};

#endif /* BidirectedNode_hpp */