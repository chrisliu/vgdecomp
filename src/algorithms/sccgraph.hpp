#ifndef VG_SCCGRAPH_HPP_INCLUDED
#define VG_SCCGRAPH_HPP_INCLUDED

#define debug
#ifdef debug
#include <iostream>
#endif /* debug */

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <functional>

#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/handlegraph/util.hpp"

using namespace std;
using namespace handlegraph;

using sccid_t = uint64_t;

struct SCCEdge {
    sccid_t scc1;
    sccid_t scc2;
    bool from_left;
    bool to_right;
};

class SCCGraph : public HandleGraph {
    private:
        unordered_map<sccid_t, vector<SCCEdge>> edges;
        sccid_t min_node;
        sccid_t max_node;

    public:
        SCCGraph(const HandleGraph& g, const vector<unordered_set<nid_t>> sccs);

        /// Method to check if a node exists by ID
        bool has_node(nid_t node_id) const;
    
        /// Look up the handle for the node with the given ID in the given orientation
        handle_t get_handle(const nid_t& node_id, bool is_reverse = false) const;
        
        /// Get the ID from a handle
        nid_t get_id(const handle_t& handle) const;
        
        /// Get the orientation of a handle
        bool get_is_reverse(const handle_t& handle) const;
        
        /// Invert the orientation of a handle (potentially without getting its ID)
        handle_t flip(const handle_t& handle) const;
        
        /// Get the length of a node
        size_t get_length(const handle_t& handle) const;
        
        /// Get the sequence of a node, presented in the handle's local forward
        /// orientation.
        std::string get_sequence(const handle_t& handle) const;
        
        /// Return the number of nodes in the graph
        size_t get_node_count() const;
        
        /// Return the smallest ID in the graph, or some smaller number if the
        /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t min_node_id() const;
        
        /// Return the largest ID in the graph, or some larger number if the
        /// largest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t max_node_id() const;

    protected:
        
        /// Loop over all the handles to next/previous (right/left) nodes. Passes
        /// them to a callback which returns false to stop iterating and true to
        /// continue. Returns true if we finished and false if we stopped early.
        bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const;
        
        /// Loop over all the nodes in the graph in their local forward
        /// orientations, in their internal stored order. Stop if the iteratee
        /// returns false. Can be told to run in parallel, in which case stopping
        /// after a false return value is on a best-effort basis and iteration
        /// order is not defined. Returns true if we finished and false if we 
        /// stopped early.
        bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const;
};

SCCGraph::SCCGraph(const HandleGraph& g, const vector<unordered_set<nid_t>> sccs) {
    /// Set min and max nodes
    min_node = 0;
    max_node = sccs.size() - 1;

    /// Reverse index that maps node to SCC
    unordered_map<nid_t, sccid_t> node_scc;
    for (sccid_t i = 0; i < sccs.size(); i++) {
        /// Construct reverse index
        for (auto id : sccs[i]) {
            node_scc[id] = i;
        }

        /// Initialze vectors for edges
        edges[i] = vector<SCCEdge>();
    }

#ifdef debug
    cout << "(debug) Node-to-SCC map" << endl;
    for (auto& [node_id, scc_id] : node_scc) {
        cout << "(debug) | Node ID: " << node_id << " SCC ID: " << scc_id << endl;
    }
#endif /* debug */

    /// SCC node orientations for consistency in decomposition
    unordered_map<nid_t, handle_t> orientations(g.get_node_count());

    function<void(const handle_t&)> orient_scc = [&](const handle_t& handle) {
        /// Exit early if the orientation for an SCC has been found
        if (orientations.count(g.get_id(handle))) return;

        /// Run left dfs to get orientations in SCC
        orientations[g.get_id(handle)] = handle;
        vector<handle_t> stack(1, handle);
        while (!stack.empty()) {
            handle_t cur_handle = stack.back();
            stack.pop_back();

            g.follow_edges(cur_handle, false, [&](const handle_t& child_handle) {
                /// If node isn't in the same SCC, ignore
                if (node_scc[g.get_id(handle)] != node_scc[g.get_id(child_handle)]) return;

                /// If node has been traversed, ignore
                if (orientations.count(g.get_id(child_handle))) return;
                
                /// Add node's orientation into SCC and set as next node
                orientations[g.get_id(child_handle)] = child_handle;
                stack.push_back(child_handle);
            });
        }
    };

    /// Establish node orientations within each SCC
    g.for_each_handle([&](const handle_t& handle) { orient_scc(handle); });

#ifdef debug
    cout << "(debug) SCC node orientations" << endl;
    for (sccid_t i = 0; i < sccs.size(); i++) {
        cout << "(debug) | SCC " << i << endl;
        for (auto id : sccs[i]) {
            cout << "(debug) | | Node " << id << " is reverse: " << (g.get_is_reverse(orientations[id]) ? "true" : "false") << endl;
        }
    }
#endif /* debug */

    /// Find edges between each SCC
    /// TODO: integrate this into the SCC node orientation step
    vector<tuple<handle_t, handle_t, bool>> found_edges;

    for (auto& it : orientations) {
        handle_t& handle = it.second;
        /// Check left side
        g.follow_edges(handle, true, [&](const handle_t& child_handle) {
            /// If the node is in the same SCC, ignore
            if (node_scc[g.get_id(child_handle)] == node_scc[g.get_id(handle)]) return;

            found_edges.push_back(make_tuple(handle, child_handle, true));
        });
        /// Check right side
        g.follow_edges(handle, false, [&](const handle_t& child_handle) {
            /// If the node is in the same SCC, ignore
            if (node_scc[g.get_id(child_handle)] == node_scc[g.get_id(handle)]) return;

            found_edges.push_back(make_tuple(handle, child_handle, false));
        });
    }

#ifdef debug
    cout << "(debug) SCC edges" << endl;
    for (auto& [handle1, handle2, from_left] : found_edges) {
        bool to_right = (from_left ? g.flip(handle2) : handle2) != orientations[g.get_id(handle2)];
        cout << "(debug) | Edge from the " << (from_left ? "left" : "right") << " of Node " << g.get_id(handle1);
        cout << " to the " << (to_right ? "right" : "left") << " of Node" << g.get_id(handle2) << endl;
    } 
#endif /* debug */


    /// Construct SCC edges
    /// TODO: the edge_exists algorithm is O(edges(SCC)), should be O(1) 
    function<bool(const sccid_t&, const sccid_t&, bool, bool)> edge_exists = 
    [&](const sccid_t& scc1, const sccid_t& scc2, bool from_left, bool to_right) {
        for (auto& edge : edges[scc1]) {
            /// If a match is found
            if (edge.scc2 == scc2 && edge.from_left == from_left && edge.to_right == to_right) return true;
        }
        return false;
    };

    for (auto& [handle1, handle2, from_left] : found_edges) {
        /// Parent node (node1)'s from_left is just from_left due to how its traversed
        /// Get attributes for the child node of this edge
        sccid_t scc1 = node_scc[g.get_id(handle1)];
        sccid_t scc2 = node_scc[g.get_id(handle2)];
        bool to_right = (from_left ? g.flip(handle2) : handle2) != orientations[g.get_id(handle2)];

        /// If edge has already been created, ignore
        if (edge_exists(scc1, scc2, from_left, to_right)) continue;

        /// Create edge
        edges[scc1].push_back({scc1, scc2, from_left, to_right});
        /// Create complement edge
        edges[scc2].push_back({scc2, scc1, !to_right, !from_left});
    }
}

bool SCCGraph::has_node(nid_t nodeid) const {
    return edges.find(nodeid) != edges.end();
}

/// Look up the handle for the node with the given ID in the given orientation
handle_t SCCGraph::get_handle(const nid_t& node_id, bool is_reverse) const {
    return number_bool_packing::pack(node_id, is_reverse);
}

/// Get the ID from a handle
nid_t SCCGraph::get_id(const handle_t& handle) const {
    return number_bool_packing::unpack_number(handle);
}

/// Get the orientation of a handle
bool SCCGraph::get_is_reverse(const handle_t& handle) const {
    return number_bool_packing::unpack_bit(handle);
}

/// Invert the orientation of a handle (potentially without getting its ID)
handle_t SCCGraph::flip(const handle_t& handle) const {
    return number_bool_packing::toggle_bit(handle);
}

/// Get the length of a node
size_t SCCGraph::get_length(const handle_t& handle) const {
    return 0;
}

/// Get the sequence of a node, presented in the handle's local forward
/// orientation.
std::string SCCGraph::get_sequence(const handle_t& handle) const {
    return "";
}

/// Return the number of nodes in the graph
size_t SCCGraph::get_node_count() const {
    return edges.size();
}

nid_t SCCGraph::min_node_id() const {
    return min_node;
}

nid_t SCCGraph::max_node_id() const {
    return max_node;
}

bool SCCGraph::follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const {
    sccid_t node_id  = get_id(handle);
    bool  is_reverse = get_is_reverse(handle);
    bool  on_left    = (go_left && !is_reverse) || (!go_left && is_reverse);
    for (const auto& node_edge : edges.at(node_id)) {
        bool flow = (node_edge.from_left ^ node_edge.to_right) ^ is_reverse; // The node return should follow the "flow" of the walk
        if (node_edge.from_left == on_left && !iteratee(get_handle(node_edge.scc2, flow))) {
            return false;
        }
    }
    return true;
}
        

bool SCCGraph::for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel) const {
    for (const auto& node_edges : edges) {
        if (!iteratee(get_handle(node_edges.first, false))) {
            return false;
        }
    }
    return true;
}

#endif /* VG_SCCGRAPH_HPP_INCLUDED */
