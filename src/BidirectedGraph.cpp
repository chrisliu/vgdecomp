#include "BidirectedGraph.hpp"
void BidirectedGraph::add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right){
    BidirectedEdge* tempedge = new BidirectedEdge(id1, id2);
    tempedge->from_left = from_left;
    tempedge->to_right = to_right;
    edges.insert(make_pair(id1, tempedge));
    return;
}

void BidirectedGraph::populate_reachable_nodes(){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator mapiter = edges.begin();
    set<uint64_t> temp;
    while(mapiter!=edges.end()){
        vector<BidirectedEdge>::iterator veciter = mapiter->second.begin();
        while(veciter!=mapiter->second.end()){
            if(veciter->id1==mapiter->first){
                reachable_nodes_helper(veciter->id1, temp, !(veciter->to_right));
            }
            else{
                reachable_nodes_helper(veciter->id2, temp, veciter->from_left);
            }
            veciter++;
        }
        mapiter++;
    }
    
}

void BidirectedGraph::reachable_nodes_helper(uint64_t id, set<uint64_t> set, bool into_left){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator mapiter = edges.begin();
    while(mapiter!=edges.end()){
        vector<BidirectedEdge>::iterator veciter = mapiter->second.begin();
        while(veciter!=mapiter->second.end()){
            if(veciter->id1==mapiter->first && veciter->from_left!=into_left){
                set.insert(id);
                reachable_nodes_helper(veciter->id2, set, !veciter->to_right);
            }
            else if(veciter->id2==mapiter->first && veciter->to_right==into_left){
                set.insert(id);
                reachable_nodes_helper(veciter->id1, set, veciter->from_left);
            }
            veciter++;
        }
        mapiter++;
    }
}
vector<uint64_t> BidirectedGraph::get_reachable_nodes(uint64_t id){
    return reachable_nodes.at(id);
}