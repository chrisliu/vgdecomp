#ifndef BidirectedGraph_hpp
#define BidirectedGraph_hpp
#include "BidirectedEdge.hpp"

// #define DEBUG_BIDIRECTED_GRAPH

/* Data structures for internal representation */
#include <vector>
#include <unordered_map>
#include <fstream>

/* Handlegraph includes */
#include "../deps/handlegraph/handle_graph.hpp"
#include "../deps/handlegraph/types.hpp"

using namespace std;
using namespace handlegraph;

typedef unordered_map<nid_t, vector<const handle_t>> node_map; 
typedef unordered_map<nid_t, vector<BidirectedEdge>> edge_map;
typedef pair<nid_t, vector<BidirectedEdge>>          edge_map_pair;

class BidirectedGraph : public HandleGraph {
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
        std::string get_sequence(const handle_t& handle) const;
        
        /// Return the number of nodes in the graph
        size_t get_node_count() const;
        
        /// Return the smallest ID in the graph, or some smaller number if the
        /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t min_node_id() const;
        
        /// Return the largest ID in the graph, or some larger number if the
        /// largest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t max_node_id() const;

#ifdef DEBUG_BIDIRECTED_GRAPH
        void print_edges() const;
#endif /* DEBUG_BIDIRECTED_GRAPH */
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
#endif /* BidirectedGraph_hpp */