#include "DirectedEdge.hpp"

DirectedEdge::DirectedEdge(id_t upstream_id, id_t dowstream_id)
: upstream_id(upstream_id), downstream_id(downstream_id) {

}

DirectedEdge* DirectedEdge::reverse() {
    return new DirectedEdge(downstream_id, upstream_id);
}

id_t DirectedEdge::get_upstream_id() {
    return upstream_id;
}

id_t DirectedEdge::get_downstream_id() {
    return downstream_id;
}

bool DirectedEdge::operator==(const DirectedEdge& other) {
    return this->upstream_id == other.upstream_id &&
           this->downstream_id == other.downstream_id;
}