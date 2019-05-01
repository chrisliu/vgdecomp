#include "BidirectedGraph.hpp"
void BidirectedGraph::add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right){
    BidirectedEdge* tempedge = new BidirectedEdge(id1, id2);
    tempedge->from_left = from_left;
    tempedge->to_right = to_right;
    edges.insert(make_pair(id1, tempedge));
    return;
}

void BidirectedGraph::populate_reachable_nodes(){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator iter = edges.begin();
    set<uint64_t> temp;
    while(iter!=edges.end()){

    }
    
}

void BidirectedGraph::reachable_nodes_helper(uint64_t id, set<uint64_t> set, bool into_left){

}
vector<uint64_t> BidirectedGraph::get_reachable_nodes(uint64_t id){
    return reachable_nodes.at(id);
}