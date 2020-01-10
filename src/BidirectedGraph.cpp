#include "BidirectedGraph.hpp"
#include <string>
#include <queue>
#include "../deps/json/json/json.h"

#include "../deps/handlegraph/util.hpp"

#ifdef DEBUG_BIDIRECTED_GRAPH
#include <iostream>
using namespace std;
#endif /* DEBUG_BIDIRECTED_GRAPH */


//******************************************************************************
// Non handlegraph/mutable handlegraph functions 
//******************************************************************************

/// Deserializes vg JSON fromat
/// Returns true if deserialize successfully or false if otherwise
bool BidirectedGraph::deserialize(ifstream& infile) {
    Json::CharReaderBuilder reader;
    Json::Value graph_json;
    string errs;
    bool resp = Json::parseFromStream(reader, infile, &graph_json, &errs);

    /// If file not in json format, print error and return false
    if (!resp) {
        cerr << "Error: Parse failed" << endl << errs << endl;
        return false;
    }

    /// TODO: check if it's proper vg JSON format
    Json::Value edges = graph_json["edge"];
    for (auto& edge : edges) {
        nid_t id1 = edge["from"].asInt64();
        nid_t id2 = edge["to"].asInt64();
        bool from_left = edge.get("from_start", false).asBool();
        bool to_right = edge.get("to_end", false).asBool();
        add_edge(id1, id2, from_left, to_right);
    }
    return true;
}

void BidirectedGraph::add_edge(nid_t id1, nid_t id2, bool from_left, bool to_right){
    BidirectedEdge from_id1(id1, id2, from_left, to_right);
    edges.emplace(make_pair(id1, vector<BidirectedEdge>()));
    edges[id1].push_back(from_id1);

    /// Make a complement edge for id2 if the edge doesn't leave and enter the same node side
    if (id1 == id2 && from_left != to_right) return;

    BidirectedEdge from_id2(id2, id1, !to_right, !from_left);
    edges.emplace(make_pair(id2, vector<BidirectedEdge>()));
    edges[id2].push_back(from_id2);
}

//******************************************************************************
// Handlegraph public functions 
//******************************************************************************

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
string BidirectedGraph::get_sequence(const handle_t& handle) const {
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

//******************************************************************************
// Mutable handlegraph public functions 
//******************************************************************************

handle_t BidirectedGraph::create_handle(const string& sequence) {

}

handle_t BidirectedGraph::create_handle(const string& sequence, const nid_t& id) {

}

void BidirectedGraph::create_edge(const handle_t& left, const handle_t& right) {

}

handle_t BidirectedGraph::apply_orientation(const handle_t& handle) {

}

vector<handle_t> BidirectedGraph::divide_handle(const handle_t& handle, const vector<size_t>& offsets) {

}

void BidirectedGraph::optimize(bool allow_id_reassignment) {

}

void BidirectedGraph::apply_ordering(const vector<handle_t>& order, bool compact_ids) {

}

void BidirectedGraph::set_id_increment(const nid_t& min_id) {

}

void BidirectedGraph::reassign_node_ids(const function<nid_t(const nid_t&)>& get_new_id) {

}

//******************************************************************************
// Handlegraph protected functions
//******************************************************************************

bool BidirectedGraph::follow_edges_impl(const handle_t& handle, bool go_left, const function<bool(const handle_t&)>& iteratee) const {
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
        

bool BidirectedGraph::for_each_handle_impl(const function<bool(const handle_t&)>& iteratee, bool parallel) const {
    for (const auto& node_edges : edges) {
        if (!iteratee(get_handle(node_edges.first, false))) {
            return false;
        }
    }
    return true;
}