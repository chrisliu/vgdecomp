#ifndef BidirectedNode_hpp
#define BidirectedNode_hpp

#include "../deps/handlegraph/types.hpp"

using namespace handlegraph;

class BidirectedNode {
    public:
        nid_t id;
        BidirectedNode(nid_t id_) : id(id_) {}
        bool operator==(const BidirectedNode& other) {
            return id == other.id;
        }
};

#endif /* BidirectedNode_hpp */