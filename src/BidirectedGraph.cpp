#include "BidirectedGraph.hpp"
#include "../deps/json/json/json.h"

#include "../deps/handlegraph/util.hpp"

#ifdef DEBUG_BIDIRECTED_GRAPH
#include <iostream>
using namespace std;
#endif /* DEBUG_BIDIRECTED_GRAPH */


//******************************************************************************
// Non handle graph functions 
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

    /// Construct nodes
    Json::Value nodes = graph_json["node"];
    for (auto& node : nodes) {
        nid_t id1 = node["id"].asInt64();
        string sequence = node["sequence"].asString();
        create_handle(sequence, id1);
    }

    /// Construct edges
    Json::Value edges = graph_json["edge"];
    for (auto& edge : edges) {
        nid_t id1 = edge["from"].asInt64();
        nid_t id2 = edge["to"].asInt64();
        bool from_left = edge.get("from_start", false).asBool();
        bool to_right = edge.get("to_end", false).asBool();
        create_edge(get_handle(id1, from_left), get_handle(id2, to_right));
    }
    return true;
}

//******************************************************************************
// Handle graph public functions 
//******************************************************************************

/// Method to check if a node exists by ID
bool BidirectedGraph::has_node(nid_t nodeid) const {
    return nodes.count(nodeid);
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
    return get_sequence(handle).size();
}

/// Get the sequence of a node, presented in the handle's local forward
/// orientation.
string BidirectedGraph::get_sequence(const handle_t& handle) const {
    return nodes.at(get_id(handle));
}

/// Return the number of nodes in the graph
size_t BidirectedGraph::get_node_count() const {
    return nodes.size();
}

/// Return the smallest ID in the graph, or some smaller number if the
/// smallest ID is unavailable. Return value is unspecified if the graph is empty.
nid_t BidirectedGraph::min_node_id() const {
    auto res = min_element(nodes.begin(), nodes.end(),
        [] (const auto& n1, const auto& n2) {
            return n1.first < n2.first;
        }
    );
    if (res != nodes.end()) {
        return res->first;   
    }
    return 0;
}

/// Return the largest ID in the graph, or some larger number if the
/// largest ID is unavailable. Return value is unspecified if the graph is empty.
nid_t BidirectedGraph::max_node_id() const {
    auto res = max_element(nodes.begin(), nodes.end(),
        [](const auto& n1, const auto& n2) {
            return n1.first < n2.first;
        }
    );
    if (res != nodes.end()) {
        return res->first;
    }
    return 0;
}

//******************************************************************************
// Mutable handle graph public functions 
//******************************************************************************

handle_t BidirectedGraph::create_handle(const string& sequence) {
    nodes[cur_id] = sequence;
    cur_id++; /// Increment node id
    return get_handle(cur_id - 1);
}

handle_t BidirectedGraph::create_handle(const string& sequence, const nid_t& id) {
    nodes[id] = sequence;
    cur_id = (id > cur_id) ? id + 1 : cur_id; // Simple cur_id update function
    return get_handle(id);
}

void BidirectedGraph::create_edge(const handle_t& left, const handle_t& right) {
    /// From left to right
    edges.emplace(left, unordered_set<handle_t>());
    edges[left].insert(right);

    // Don't create complement if it's an inversion
    if (left == flip(right)) return;

    /// Create complement from right to left
    edges.emplace(flip(right), unordered_set<handle_t>());
    edges[flip(right)].insert(flip(left));
}

handle_t BidirectedGraph::apply_orientation(const handle_t& handle) {
    /// TODO: Incorrect implementation
    return handle;
}

vector<handle_t> BidirectedGraph::divide_handle(const handle_t& handle, const vector<size_t>& offsets) {
    /// TODO: Incorrect implementation
    return vector<handle_t>();
}

void BidirectedGraph::optimize(bool allow_id_reassignment) {
    /// TODO: Incorrect implementation
}

void BidirectedGraph::apply_ordering(const vector<handle_t>& order, bool compact_ids) {
    /// TODO: Incorrect implementation
}

void BidirectedGraph::set_id_increment(const nid_t& min_id) {
    /// TODO: Incorrect implementation
}

void BidirectedGraph::reassign_node_ids(const function<nid_t(const nid_t&)>& get_new_id) {
    /// TODO: Incorrect implementation
}

//******************************************************************************
// Deletable handle graph public functions
//******************************************************************************

void BidirectedGraph::destroy_handle(const handle_t& handle) {
    /// Erase from nodes
    nodes.erase(get_id(handle));

    /// Remove edges in the "forward" direction
    handle_t flipped = flip(handle);
    /// Remove complement edges
    for (auto& rhandle : edges[handle]) {
        edges[flip(rhandle)].erase(flipped);
    }
    /// Delete "forward edges" from handle
    edges.erase(handle);

    /// Remove edges in the "backward" direction
    /// Remove complement edges
    for (auto& rhandle : edges[flipped]) {
        edges[flip(rhandle)].erase(handle);
    }
    /// Delete "forward edges" from handle
    edges.erase(flipped);
}

void BidirectedGraph::destroy_edge(const handle_t& left, const handle_t& right) {
    /// Check if it's been destroyed already
    if (!edges.count(left)) return;

    /// Erase forward edge
    edges[left].erase(right);
    /// Erase complement edge
    edges[flip(right)].erase(flip(left));
}

void BidirectedGraph::clear() {
    nodes.clear();
    edges.clear();
}

//******************************************************************************
// Handle graph protected functions
//******************************************************************************

bool BidirectedGraph::follow_edges_impl(const handle_t& handle, bool go_left, const function<bool(const handle_t&)>& iteratee) const {
    /// Get handle of proper direction
    handle_t lhandle;
    if (go_left) lhandle = flip(handle);
    else lhandle = *const_cast<handle_t*>(&handle);

    /// Early exit if there aren't any edges for that handle
    if (!edges.count(lhandle)) return true;

    /// Otherwise iterate through every edge
    for (auto& rhandle : edges.at(lhandle)) {
        if (!iteratee((go_left ? flip(rhandle) : rhandle))) return false;
    }
    return true;
}
        

bool BidirectedGraph::for_each_handle_impl(const function<bool(const handle_t&)>& iteratee, bool parallel) const {
    for (auto& [nid, _] : nodes) {
        if (!iteratee(get_handle(nid))) return false;
    }
    return true;
}