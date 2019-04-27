#ifndef BidirectedNode_hpp
#define BidirectedNode_hpp
#include <stdint.h>
// This class is not used?
class BidirectedNode{
    public:
        uint64_t id;
        BidirectedNode(int64_t id);
        bool operator==(const BidirectedNode& other);
};

#endif