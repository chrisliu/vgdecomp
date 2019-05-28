#include "BidirectedGraph.hpp"

unordered_map<nid_t, set<nid_t> > sets;


vector<nid_t> BidirectedGraph::get_reachable_nodes(nid_t id){
    return reachable_nodes.at(id);
}

void BidirectedGraph::add_edge(nid_t id1, nid_t id2, bool from_left, bool to_right){
    BidirectedEdge from_id1(id1, id2, from_left, to_right);
    BidirectedEdge from_id2(id2, id1, to_right, from_left);

    edges.emplace(make_pair(id1, vector<BidirectedEdge>()));
    edges.emplace(make_pair(id2, vector<BidirectedEdge>()));
    edges[id1].push_back(from_id1);
    edges[id2].push_back(from_id2);
}

bool BidirectedGraph::is_acyclic(){
    return true;
}

void BidirectedGraph::populate_reachable_nodes(){
    edge_map::iterator mapiter = edges.begin();
    while(mapiter!=edges.end()){
        vector<BidirectedEdge>::iterator veciter = mapiter->second.begin();
        while(veciter!=mapiter->second.end()){

            if(veciter->id1==mapiter->first){
                sets.emplace(make_pair(mapiter->first, set<nid_t>()));
                sets[mapiter->first].insert(veciter->id2);

                reachable_nodes_helper(veciter->id2, sets.at(mapiter->first), !veciter->to_right);
            }
            else if(veciter->id2==mapiter->first){
                sets.emplace(make_pair(mapiter->first, set<nid_t>()));
                sets[mapiter->first].insert(veciter->id1);
                reachable_nodes_helper(veciter->id1, sets.at(mapiter->first), veciter->from_left);
            }
            veciter++;
        }
        mapiter++;
    }
    unordered_map<nid_t, set<nid_t> >:: iterator setmapiter = sets.begin();
    while(setmapiter!=sets.end()){
        set<nid_t>:: iterator setiter = sets.at(setmapiter->first).begin();
        reachable_nodes.emplace(setmapiter->first, vector<nid_t>());
        while(setiter!=sets.at(setmapiter->first).end()){
            reachable_nodes[setmapiter->first].push_back(*setiter);
            setiter++;
        }
        setmapiter++;
    }
}

void BidirectedGraph::reachable_nodes_helper(nid_t id, set<nid_t>& set, bool into_left){
        
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
    unordered_map<nid_t, vector<nid_t> >::iterator reachable = reachable_nodes.begin();
    while(reachable!=reachable_nodes.end()){
        vector<nid_t>::iterator nodeiter = reachable->second.begin();
        printf("Printing path connected nodes from %llu:\n", reachable->first);
        while(nodeiter!=reachable->second.end()){
            printf("%llu\n", *nodeiter);
            nodeiter++;
        }
        reachable++;
    }
}

/// Method to check if a node exists by ID
bool BidirectedGraph::has_node(nid_t nodeid) const {
    return edges.find(nodeid) != edges.end();
}

/// Look up the handle for the node with the given ID in the given orientation
handle_t BidirectedGraph::get_handle(const nid_t& node_id, bool is_reverse) const {
    return encoder.pack(node_id, is_reverse);
}

/// Get the ID from a handle
nid_t BidirectedGraph::get_id(const handle_t& handle) const {
    return encoder.unpack_number(handle);
}

/// Get the orientation of a handle
bool BidirectedGraph::get_is_reverse(const handle_t& handle) const {
    return encoder.unpack_bit(handle);
}

/// Invert the orientation of a handle (potentially without getting its ID)
handle_t BidirectedGraph::flip(const handle_t& handle) const {
    return encoder.toggle_bit(handle);
}

/// Get the length of a node
size_t BidirectedGraph::get_length(const handle_t& handle) const {
    return 0;
}

/// Get the sequence of a node, presented in the handle's local forward
/// orientation.
std::string BidirectedGraph::get_sequence(const handle_t& handle) const {
    return "";
}

/// Return the number of nodes in the graph
size_t BidirectedGraph::get_node_count() const {
    return edges.size();
}

/// Return the smallest ID in the graph, or some smaller number if the
/// smallest ID is unavailable. Return value is unspecified if the graph is empty.
nid_t BidirectedGraph::min_node_id() const {
    edge_map::const_iterator res = min_element(edges.begin(), edges.end());
    // Should work with default comparator, otherwise use
    // edge_map::const_iterator res = min_element(edges.begin(), edges.end(),
    //                                     [](const nid_t& id1, const nid_t& id2) {
    //                                         return id1 < id2;
    //                                     })
    if (res != edges.end()) {
        return res->first;
    }
    return 0;
}

/// Return the largest ID in the graph, or some larger number if the
/// largest ID is unavailable. Return value is unspecified if the graph is empty.
nid_t BidirectedGraph::max_node_id() const {
    edge_map::const_iterator res = max_element(edges.begin(), edges.end());
    if (res != edges.end()) {
        return res->first;
    }
    return 0;
}

bool BidirectedGraph::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    nid_t node_id = get_id(handle);
    for (const BidirectedEdge& node_edge : edges.at(node_id)) {
        if (node_edge.from_left == go_left && !iteratee(get_handle(node_edge.id2, go_left))) {
            return false;
        }
    }
    return true;
}
        

bool BidirectedGraph::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const {
    for (const auto& node_edges : edges) {
        if (!iteratee(get_handle(node_edges.first, false))) {
            return false;
        }
    }
    return true;
}
