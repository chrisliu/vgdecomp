#include "UndirectedEdge.hpp"

UndirectedEdge::UndirectedEdge(id_t id1, id_t id2) {
    if (id1 < id2) {
        downstream_id = id1;
        upstream_id = id2;
    } else {
        downstream_id = id2;
        upstream_id = id1;
    }
}

bool UndirectedEdge::operator==(const UndirectedEdge& other) {
    return DirectedEdge::operator==(other);
}