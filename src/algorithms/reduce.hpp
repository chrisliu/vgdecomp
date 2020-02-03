#ifndef VG_ALGORITHMS_REDUCE_HPP_INCLUDED
#define VG_ALGORITHMS_REDUCE_HPP_INCLUDED

#include "../../deps/handlegraph/deletable_handle_graph.hpp"
#include "find_balanced_bundles.hpp"
#include "bundle.hpp"
#include "handle.hpp"

#include <unordered_map> // TEMP
using bundle_map_t = std::unordered_map<handle_t, Bundle*>; // TEMP

/// Temporarily exposing reduction actions
void reduce_degree_one_node(DeletableHandleGraph& g, handle_t& node); // R1 
handlegraph::handle_t reduce_trivial_bundle(DeletableHandleGraph& g, Bundle& bundle); // R2
Bundle* reduce_bundle(DeletableHandleGraph& g, Bundle& bundle); // R3

/// Temporarily exposing checking operations
bool is_reduction1(const HandleGraph& g, const nid_t& node_id, handle_t& return_handle);
bool is_reduction2(const HandleGraph& g, const nid_t& node_id, bundle_map_t& bundle_map, handle_t& return_handle);
bool is_reduction3(const HandleGraph& g, const nid_t& node_id, bundle_map_t& bundle_map, handle_t& return_handle);

void mark_bundle(HandleGraph& g, Bundle* bundle, bundle_map_t& bundle_map);


#endif /* VG_ALGORITHMS_REDUCE_HPP_INCLUDED */