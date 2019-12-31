#include "BidirectedGraph.hpp"
#include <queue>

#include "../deps/handlegraph/util.hpp"

#ifdef DEBUG_BIDIRECTED_GRAPH
#include <iostream>
using namespace std;
#endif /* DEBUG_BIDIRECTED_GRAPH */

BidirectedGraph::BidirectedGraph() {

}

// vector<const handle_t> BidirectedGraph::get_reachable_nodes(handle_t id){
//     return reachable_nodes[get_id(id)];
// }

void BidirectedGraph::add_edge(nid_t id1, nid_t id2, bool from_left, bool to_right){
    BidirectedEdge from_id1(id1, id2, from_left, to_right);
    BidirectedEdge from_id2(id2, id1, !to_right, !from_left);

    edges.emplace(make_pair(id1, vector<BidirectedEdge>()));
    edges.emplace(make_pair(id2, vector<BidirectedEdge>()));
    edges[id1].push_back(from_id1);
    edges[id2].push_back(from_id2);
}

// bool BidirectedGraph::is_acyclic(){
//     return true;
// }
// //idk if this works
// void BidirectedGraph::populate_reachable_nodes(){
    
//     unordered_map<const nid_t, char > colormap;
//     for_each_handle_impl([&] (const handle_t& handle) {
//         vector<const handle_t> component;
//         queue<handle_t> queue;
//         queue.push(handle);
//         colormap[get_id(handle)] = 'g';
//         while(queue.size()!=0){
//             handle_t currnode = queue.front();
//             queue.pop();
//             follow_edges_impl(currnode, true, [&](const handle_t& handle){
//                 if(colormap.find(get_id(handle))!=colormap.end()){
//                     return true;
//                 }
//                 ((vector<const handle_t>) component).push_back(handle);
//                 colormap[get_id(handle)] = 'g';
//                 return true;
//             });
//         }
//         colormap[get_id(handle)] = 'b';
//         vector<const handle_t>::iterator iter= component.begin();
//         while(iter!=component.end()){
//             reachable_nodes.emplace(make_pair(get_id(*iter), component));
//         }
//         return true;
//     }, false);
// }


// lowercase g means entering from left
// uppercase G means entering from right
//void BidirectedGraph::mod_BFS(nid_t id){
    /*
    if(reachable_nodes.find(id)==reachable_nodes.end()){
        reachable_nodes.emplace(id, vector<handle_t>());
    }
    
    queue<nid_t> queue = ::queue<nid_t>();
    queue.push(id);
    colormap.emplace(make_pair(id, 'g'));
    while(queue.size()!=0){
        nid_t currnode = queue.front();
        queue.pop();
        char color = colormap.at(currnode);
        vector<BidirectedEdge>::iterator veciter = edges.at(id).begin();
        while(veciter!=edges.at(id).end()){
            if(color=='g'){
                if(veciter->id1==currnode && !veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id2, false));
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id1, false));
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            else if(color=='G'){
                if(veciter->id1==currnode && veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id2, false));
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && !veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id1, false));
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            ++veciter;
        }
        colormap.at(currnode) = 'b';
    }
    queue = ::queue<nid_t>();
    colormap = unordered_map<nid_t, char>();
    queue.push(id);
    colormap.emplace(make_pair(id, 'G'));
    while(queue.size()!=0){
        nid_t currnode = queue.front();
        queue.pop();
        char color = colormap.at(currnode);
        vector<BidirectedEdge>::iterator veciter = edges.at(id).begin();
        while(veciter!=edges.at(id).end()){
            if(color=='g'){
                if(veciter->id1==currnode && !veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id2, false));
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id1, false));
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            else if(color=='G'){
                if(veciter->id1==currnode && veciter->from_left && colormap.find(veciter->id2)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id2, false));
                    queue.push(veciter->id2);
                    colormap.emplace(veciter->id2, veciter->to_right ? 'G' : 'g');
                }
                else if(veciter->id2==currnode && !veciter->to_right && colormap.find(veciter->id1)==colormap.end()){
                    reachable_nodes.at(id).push_back(get_handle(veciter->id1, false));
                    queue.push(veciter->id1);
                    colormap.emplace(veciter->id1, veciter->from_left ? 'g' : 'G');
                }
            }
            ++veciter;
        }
        colormap.at(currnode) = 'b';
    }
    */
//}

// void BidirectedGraph::print_reachable_nodes(){
//     unordered_map<nid_t, vector<const handle_t> >::iterator reachable = reachable_nodes.begin();
//     while(reachable!=reachable_nodes.end()){
//         vector<const handle_t>::iterator nodeiter = reachable->second.begin();
//         printf("Printing path connected nodes from %llu:\n", reachable->first);
//         while(nodeiter!=reachable->second.end()){
//             printf("%llu\n", get_id(*nodeiter));
//             nodeiter++;
//         }
//         reachable++;
//     }
// }

/// Method to check if a node exists by ID
bool BidirectedGraph::has_node(nid_t nodeid) const {
    return edges.find(nodeid) != edges.end();
}

/// Look up the handle for the node with the given ID in the given orientation
handle_t BidirectedGraph::get_handle(const nid_t& node_id, bool is_reverse) const {
    return number_bool_packing::pack(node_id, is_reverse);
}

/// Get the ID from a handle
nid_t BidirectedGraph::get_id(const handle_t& handle) const {
    return number_bool_packing::unpack_number(handle);
}

/// Get the orientation of a handle
bool BidirectedGraph::get_is_reverse(const handle_t& handle) const {
    return number_bool_packing::unpack_bit(handle);
}

/// Invert the orientation of a handle (potentially without getting its ID)
handle_t BidirectedGraph::flip(const handle_t& handle) const {
    return number_bool_packing::toggle_bit(handle);
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
    // Should work with default comparator, otherwise use
    edge_map::const_iterator res = min_element(edges.begin(), edges.end(),
                                        [](const edge_map_pair& node1, const edge_map_pair& node2) {
                                            return node1.first < node2.first;
                                        });
    if (res != edges.end()) {
        return res->first;
    }
    return 0;
}

/// Return the largest ID in the graph, or some larger number if the
/// largest ID is unavailable. Return value is unspecified if the graph is empty.
nid_t BidirectedGraph::max_node_id() const {
    edge_map::const_iterator res = max_element(edges.begin(), edges.end(),
                                        [](const edge_map_pair& node1, const edge_map_pair& node2) {
                                            return node1.first < node2.first;
                                        });
    if (res != edges.end()) {
        return res->first;
    }
    return 0;
}

bool BidirectedGraph::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    nid_t node_id    = get_id(handle);
    bool  is_reverse = get_is_reverse(handle);
    bool  on_left    = (go_left && !is_reverse) || (!go_left && is_reverse);
    for (const BidirectedEdge& node_edge : edges.at(node_id)) {
        bool flow = (node_edge.from_left ^ node_edge.to_right) ^ is_reverse; // The node return should follow the "flow" of the walk
        if (node_edge.from_left == on_left && !iteratee(get_handle(node_edge.id2, flow))) {
            return false;
        }
    }
    return true;
}
        

bool BidirectedGraph::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    for (const auto& node_edges : edges) {
        if (!iteratee(get_handle(node_edges.first, false))) {
            return false;
        }
    }
    return true;
}

#ifdef DEBUG_BIDIRECTED_GRAPH
inline void pprint_edge(const BidirectedEdge& edge) {
    cout << " - " << "Node " << edge.id1 << ((edge.from_left) ? '-' : '+') << " <-> " << "Node " << edge.id2 << ((edge.to_right) ? '+' : '-') << endl;
}

void BidirectedGraph::print_edges() const {
    for (const auto& node_edges : edges) {
        cout << "Node " << node_edges.first << endl;
        for (const BidirectedEdge& edge : node_edges.second) {
            pprint_edge(edge);
        }
    }
}
#endif /* DEBUG_BIDIRECTED_GRAPH */