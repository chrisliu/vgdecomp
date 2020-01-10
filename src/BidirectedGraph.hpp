#ifndef BidirectedGraph_hpp
#define BidirectedGraph_hpp
#include "BidirectedEdge.hpp"

// #define DEBUG_BIDIRECTED_GRAPH

/* Data structures for internal representation */
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>

/* Handlegraph includes */
#include "../deps/handlegraph/deletable_handle_graph.hpp"
#include "../deps/handlegraph/types.hpp"

using namespace std;
using namespace handlegraph;

typedef unordered_map<nid_t, vector<const handle_t>> node_map; 
typedef unordered_map<nid_t, vector<BidirectedEdge>> edge_map;
typedef pair<nid_t, vector<BidirectedEdge>>          edge_map_pair;

class BidirectedGraph : public DeletableHandleGraph {
    private:
        node_map reachable_nodes;
        edge_map edges;
    public:
        bool deserialize(ifstream& infile);        

        vector<const handle_t> get_reachable_nodes(handle_t node);
        void add_edge(nid_t id1, nid_t id2, bool from_left, bool to_right);
        bool is_acyclic();
        void populate_reachable_nodes();
        void print_reachable_nodes();

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
        string get_sequence(const handle_t& handle) const;
        
        /// Return the number of nodes in the graph
        size_t get_node_count() const;
        
        /// Return the smallest ID in the graph, or some smaller number if the
        /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t min_node_id() const;
        
        /// Return the largest ID in the graph, or some larger number if the
        /// largest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t max_node_id() const;

        /// Create a new node with the given sequence and return the handle.
        handle_t create_handle(const string& sequence);

        /// Create a new node with the given id and sequence, then return the handle.
        handle_t create_handle(const string& sequence, const nid_t& id);
        
        /// Create an edge connecting the given handles in the given order and orientations.
        /// Ignores existing edges.
        void create_edge(const handle_t& left, const handle_t& right);
        
        /// Alter the node that the given handle corresponds to so the orientation
        /// indicated by the handle becomes the node's local forward orientation.
        /// Rewrites all edges pointing to the node and the node's sequence to
        /// reflect this. Invalidates all handles to the node (including the one
        /// passed). Returns a new, valid handle to the node in its new forward
        /// orientation. Note that it is possible for the node's ID to change.
        /// Does not update any stored paths. May change the ordering of the underlying
        /// graph.
        handle_t apply_orientation(const handle_t& handle);
        
        /// Split a handle's underlying node at the given offsets in the handle's
        /// orientation. Returns all of the handles to the parts. Other handles to
        /// the node being split may be invalidated. The split pieces stay in the
        /// same local forward orientation as the original node, but the returned
        /// handles come in the order and orientation appropriate for the handle
        /// passed in.
        /// Updates stored paths.
        vector<handle_t> divide_handle(const handle_t& handle, const vector<size_t>& offsets);
        
        /// Adjust the representation of the graph in memory to improve performance.
        /// Optionally, allow the node IDs to be reassigned to further improve
        /// performance.
        /// Note: Ideally, this method is called one time once there is expected to be
        /// few graph modifications in the future.
        void optimize(bool allow_id_reassignment = true);

        /// Reorder the graph's internal structure to match that given.
        /// This sets the order that is used for iteration in functions like for_each_handle.
        /// Optionally may compact the id space of the graph to match the ordering, from 1->|ordering|.
        /// This may be a no-op in the case of graph implementations that do not have any mechanism to maintain an ordering.
        void apply_ordering(const vector<handle_t>& order, bool compact_ids = false);

        /// Set a minimum id to increment the id space by, used as a hint during construction.
        /// May have no effect on a backing implementation.
        void set_id_increment(const nid_t& min_id);

        /// Renumber all node IDs using the given function, which, given an old ID, returns the new ID.
        /// Modifies the graph in place. Invalidates all outstanding handles.
        /// If the graph supports paths, they also must be updated.
        /// The mapping function may return 0. In this case, the input ID will
        /// remain unchanged. The mapping function should not return any ID for
        /// which it would return 0.
        void reassign_node_ids(const function<nid_t(const nid_t&)>& get_new_id);

        /// Remove the node belonging to the given handle and all of its edges.
        /// Does not update any stored paths.
        /// Invalidates the destroyed handle.
        /// May be called during serial for_each_handle iteration **ONLY** on the node being iterated.
        /// May **NOT** be called during parallel for_each_handle iteration.
        /// May **NOT** be called on the node from which edges are being followed during follow_edges.
        void destroy_handle(const handle_t& handle);
        
        /// Remove the edge connecting the given handles in the given order and orientations.
        /// Ignores nonexistent edges.
        /// Does not update any stored paths.
        void destroy_edge(const handle_t& left, const handle_t& right);
        
        /// Remove all nodes and edges.
        void clear();

    protected:
        
        /// Loop over all the handles to next/previous (right/left) nodes. Passes
        /// them to a callback which returns false to stop iterating and true to
        /// continue. Returns true if we finished and false if we stopped early.
        bool follow_edges_impl(const handle_t& handle, bool go_left, const function<bool(const handle_t&)>& iteratee) const;
        
        /// Loop over all the nodes in the graph in their local forward
        /// orientations, in their internal stored order. Stop if the iteratee
        /// returns false. Can be told to run in parallel, in which case stopping
        /// after a false return value is on a best-effort basis and iteration
        /// order is not defined. Returns true if we finished and false if we 
        /// stopped early.
        bool for_each_handle_impl(const function<bool(const handle_t&)>& iteratee, bool parallel = false) const;
};
#endif /* BidirectedGraph_hpp */