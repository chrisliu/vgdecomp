#ifndef UndirectedEdge_hpp
#define UndirectedEdge_hpp

#include <cstdint>
#include "DirectedEdge.hpp"

typedef int64_t id_t;

class UndirectedEdge : public DirectedEdge {
    public:
        UndirectedEdge(id_t id1, id_t id2);
        bool operator==(const UndirectedEdge& other);
};

#endif