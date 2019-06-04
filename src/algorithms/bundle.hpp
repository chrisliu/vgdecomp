#ifndef VG_ALGORITHMS_BUNDLE_HPP_INCLUDED
#define VG_ALGORITHMS_BUNDLE_HPP_INCLUDED

#include "../BidirectedGraph.hpp"

/// Locates all bundles in a given graph.
/// Walks in both directions of a node to check. Nodes that have been
/// walked in a particular direction will be cached. 
/// Returns all bundles that have been found. The return format will
/// be a vector of pairs of vectors containing handles. Each pair
/// will denote the left side and the right side of a bundle.

typedef std::pair<std::vector<handle_t>, std::vector<handle_t>> bundle_t;

std::vector<bundle_t> find_bundles(const HandleGraph* g);

#endif /* VG_ALGORITHMS_BUNDLE_HPP_INCLUDED */