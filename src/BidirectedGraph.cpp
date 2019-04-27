#include "BidirectedGraph.hpp"

void BidirectedGraph::add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right){
    BidirectedEdge* tempedge = new BidirectedEdge(id1, id2);
    tempedge->from_left = from_left;
    tempedge->to_right = to_right;
    edges.insert(make_pair(id1, tempedge));
    return;
}

void BidirectedGraph::populate_reachable_nodes(){
    
}