#ifndef VG_ALGORITHMS_FIND_BALANCED_BUNDLE_HPP_INCLUDED
#define VG_ALGORITHMS_FIND_BALANCED_BUNDLE_HPP_INCLUDED

// #define DEBUG_FIND_BALANCED_BUNDLES

#include <vector>
#include <unordered_set>

#include "../BidirectedGraph.hpp"
#include "bundle.hpp"

/// Locates all bundles in a given graph.
/// Walks in both directions of a node to check. Nodes that have been
/// walked in a particular direction will be cached. 
/// Returns all bundles that have been found. The return format will
/// be a vector of pairs of vectors containing handles. Each pair
/// will denote the left side and the right side of a bundle.
std::vector<Bundle> find_balanced_bundles(const HandleGraph& g);

#endif /* VG_ALGORITHMS_FIND_BALANCED_BUNDLE_HPP_INCLUDED */