#ifndef VG_ALGORITHMS_IS_SINGLE_STRANDED_HPP_INCLUDED
#define VG_ALGORITHMS_IS_SINGLE_STRANDED_HPP_INCLUDED

/**
 * \file single_stranded.hpp
 *
 * Defines algorithms for deciding if a graph contains reversing edges.
 */

#include "../../deps/handlegraph/handle_graph.hpp"

#include <unordered_set>
#include <unordered_map>
#include <vector>

using HandleGraph = handlegraph::HandleGraph;
using handle_t = handlegraph::handle_t;

/// Returns a vector of handles where the orientation of each handle indicates an
/// orientation that could be used to convert the graph into a single-stranded graph.
/// That is, if all of the reverse handles (or all of the forward handles) were swapped
/// in orientation, the graph would contain no reversing edges. Returns an empty vector
/// if there is no such combination of node orientations (also if graph has no nodes).
std::vector<handle_t> single_stranded_orientation(const HandleGraph* graph);

#endif
