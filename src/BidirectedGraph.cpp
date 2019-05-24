#include "BidirectedGraph.hpp"

unordered_map<uint64_t, set<uint64_t> > sets;

vector<uint64_t> BidirectedGraph::get_reachable_nodes(uint64_t id){
    return reachable_nodes.at(id);
}

void BidirectedGraph::add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right){
    BidirectedEdge tempedge(id1, id2);
    tempedge.from_left = from_left;
    tempedge.to_right = to_right;
  
    edges.emplace(make_pair(id1, vector<BidirectedEdge>()));
    edges.emplace(make_pair(id2, vector<BidirectedEdge>()));
    edges[id1].push_back(tempedge);
    edges[id2].push_back(tempedge);
}

bool BidirectedGraph::is_acyclic(){
    return true;
}

void BidirectedGraph::populate_reachable_nodes(){
    unordered_map<uint64_t, vector<BidirectedEdge> >::iterator mapiter = edges.begin();
    while(mapiter!=edges.end()){
        vector<BidirectedEdge>::iterator veciter = mapiter->second.begin();
        while(veciter!=mapiter->second.end()){

            if(veciter->id1==mapiter->first){
                sets.emplace(make_pair(mapiter->first, set<uint64_t>()));
                sets[mapiter->first].insert(veciter->id2);

                reachable_nodes_helper(veciter->id2, sets.at(mapiter->first), !veciter->to_right);
            }
            else if(veciter->id2==mapiter->first){
                sets.emplace(make_pair(mapiter->first, set<uint64_t>()));
                sets[mapiter->first].insert(veciter->id1);
                reachable_nodes_helper(veciter->id1, sets.at(mapiter->first), veciter->from_left);
            }
            veciter++;
        }
        mapiter++;
    }
    unordered_map<uint64_t, set<uint64_t> >:: iterator setmapiter = sets.begin();
    while(setmapiter!=sets.end()){
        set<uint64_t>:: iterator setiter = sets.at(setmapiter->first).begin();
        reachable_nodes.emplace(setmapiter->first, vector<uint64_t>());
        while(setiter!=sets.at(setmapiter->first).end()){
            reachable_nodes[setmapiter->first].push_back(*setiter);
            setiter++;
        }
        setmapiter++;
    }
}

void BidirectedGraph::reachable_nodes_helper(uint64_t id, set<uint64_t>& set, bool into_left){
        
        vector<BidirectedEdge>::iterator veciter = edges.at(id).begin();
        while(veciter!=edges.at(id).end()){
            if(veciter->id1==id && veciter->from_left!=into_left){
                set.insert(veciter->id2);
                reachable_nodes_helper(veciter->id2, set, !veciter->to_right);
            }
            else if(veciter->id2==id && veciter->to_right==into_left){
                set.insert(veciter->id1);
                reachable_nodes_helper(veciter->id1, set, veciter->from_left);
            }
            veciter++;
        }

}


void BidirectedGraph::print_reachable_nodes(){
    unordered_map<uint64_t, vector<uint64_t> >::iterator reachable = reachable_nodes.begin();
    while(reachable!=reachable_nodes.end()){
        vector<uint64_t>::iterator nodeiter = reachable->second.begin();
        printf("Printing path connected nodes from %llu:\n", reachable->first);
        while(nodeiter!=reachable->second.end()){
            printf("%llu\n", *nodeiter);
            nodeiter++;
        }
        reachable++;
    }
}