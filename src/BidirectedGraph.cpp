#include "BidirectedGraph.hpp"

unordered_map<uint64_t, set<uint64_t> > sets;

void BidirectedGraph::add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right){
    BidirectedEdge tempedge(id1, id2);
    tempedge.from_left = from_left;
    tempedge.to_right = to_right;
    edges.insert({id1, tempedge});
    edges.insert({id2, tempedge});
}

void BidirectedGraph::populate_reachable_nodes(){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator mapiter = edges.begin();
    while(mapiter!=edges.end()){
        vector<BidirectedEdge>::iterator veciter = mapiter->second.begin();
        while(veciter!=mapiter->second.end()){
            if(veciter->id1==mapiter->first){
                sets.at(mapiter->first).insert(veciter->id2);
                reachable_nodes_helper(veciter->id2, sets.at(mapiter->first), veciter->to_right);
            }
            else if(veciter->id2==mapiter->first){
                sets.at(mapiter->first).insert(veciter->id1);
                reachable_nodes_helper(veciter->id1, sets.at(mapiter->first), veciter->from_left);
            }
            veciter++;
        }
        mapiter++;
    }
    unordered_map<uint64_t, set<uint64_t> >:: iterator setmapiter = sets.begin();
    while(setmapiter!=sets.end()){
        set<uint64_t>:: iterator setiter = sets.at(setmapiter->first).begin();
        while(setiter!=sets.at(setmapiter->first).end()){
            reachable_nodes.insert(make_pair(setmapiter->first, *setiter));
            setiter++;
        }
        setmapiter++;
    }
}

void BidirectedGraph::reachable_nodes_helper(uint64_t id, set<uint64_t> set, bool into_left){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator mapiter = edges.begin();
    while(mapiter!=edges.end()){
        vector<BidirectedEdge>::iterator veciter = mapiter->second.begin();
        while(veciter!=mapiter->second.end()){
            if(veciter->id1==mapiter->first && veciter->from_left!=into_left){
                set.insert(veciter->id2);
                reachable_nodes_helper(veciter->id2, set, !veciter->to_right);
            }
            else if(veciter->id2==mapiter->first && veciter->to_right==into_left){
                set.insert(veciter->id1);
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

void BidirectedGraph::print_reachable_nodes(){
    unordered_map<uint64_t, vector<uint64_t> >::iterator reachable = reachable_nodes.begin();
    while(reachable!=reachable_nodes.end()){
        vector<uint64_t>::iterator nodeiter = reachable->second.begin();
        printf("Printing path connected nodes from %d:\n", reachable->first);
        while(nodeiter!=reachable->second.end()){
            printf("%d\n", *nodeiter);
        }
    }
}