#ifndef VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED
#define VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED

#include "handle.hpp"
#include <vector>

enum decomp_node_t {
    SourceNode, // Node from the original graph.
    DerivedNode // Node that's derived from the original graph via reduction operations.
};

struct DecompositionNode {
    nid_t id; // Id of the source/derived node this decomposition node represents.
    decomp_node_t type; // TODO: A bit of a hack. Could use classes in the future
    std::vector<handle_t> left_bounds; // Ids of source nodes on the left.
    std::vector<handle_t> right_bounds; // Ids of source nodes on the right.
    DecompositionNode* parent; // For easy traversal.
    DecompositionNode* sibling; // A blue edge. A pointer to the sibling to the "right".
    std::vector<DecompositionNode*> children; // Red edges. Children nodes in the tree.
    
    // TODO: Reorder children vector to be in order.
    // void optimize();
};

#endif /* VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED */