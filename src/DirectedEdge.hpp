#ifndef DirectedEdge_hpp
#define DirectedEdge_hpp

#include <cstdint>

typedef int64_t id_t;

class DirectedEdge {
    protected:
        id_t downstream_id;
        id_t upstream_id;
    
    public:
        DirectedEdge(id_t upstream_id, id_t downstream_id);
        DirectedEdge* reverse();
        id_t get_upstream_id();
        id_t get_downstream_id();
        bool operator==(const DirectedEdge& other);
};

#endif