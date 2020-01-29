#ifndef VG_ALGORITHMS_REDUCE_HPP_INCLUDED
#define VG_ALGORITHMS_REDUCE_HPP_INCLUDED

#include "../../deps/handlegraph/deletable_handle_graph.hpp"
#include "find_balanced_bundles.hpp"
#include "bundle.hpp"

/// Temporarily exposing reduction actions
void reduce_degree_one_node(handlegraph::DeletableHandleGraph& g, handlegraph::handle_t& node); // R1 
handlegraph::handle_t reduce_trivial_bundle(handlegraph::DeletableHandleGraph& g, Bundle& bundle); // R2
Bundle* reduce_bundle(handlegraph::DeletableHandleGraph& g, Bundle& bundle); // R3

#endif /* VG_ALGORITHMS_REDUCE_HPP_INCLUDED */