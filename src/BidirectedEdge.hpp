#ifndef BidirectedEdge_hpp
#define BidirectedEdge_hpp
#include <cstdint>
class BidirectedEdge {
    public:
        uint64_t id1;
        uint64_t id2;
        bool from_left; 
                        // If it’s exiting from the left side of id1
        bool to_right;
                        // If it’s entering from the right side of id1
        BidirectedEdge(uint64_t id1, uint64_t id2);
        bool operator==(const BidirectedEdge& other);
};
#endif