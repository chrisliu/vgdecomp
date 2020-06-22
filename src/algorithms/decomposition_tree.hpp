#ifndef VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED
#define VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED

#include "handle.hpp"
#include <vector>

#define DEBUG_DECOMP_TREE

enum decomp_node_t {
    Source, // This node represents a node in the original graph.
    Chain,  // This type of derived node represents a chain of children 
    Split   // This type of derived node is split into multiple children that 
            // are independent of each other.
};

// A POD that represents a node in the decompositon tree.
struct DecompositionNode {
    // Id of the source/derived node this decomposition node represents.
    nid_t nid; 
    // Direction this decomposition node is oriented in.
    // This is important to determine if the blue edges need to be reversed
    // (only relevant for intermediate nodes).
    bool is_reverse; 
    // The type of decompostion node. 
    decomp_node_t type; 

    /* Id's of the source and sink nodes (edge nodes). May be relevant for
     * rule 1 reduction.
    std::vector<handle_t> left_bounds;  // Ids of source nodes on the left.
    std::vector<handle_t> right_bounds; // Ids of source nodes on the right.
    */

    // Decomposition node's relationship with other nodes.
    // The parent(s) of a this node. Only Rule 1 node will have multiple parents.
    std::vector<DecompositionNode*> parents; 
    // A blue edge. A pointer to the sibling to the "right".
    DecompositionNode* sibling = nullptr; 
    // Red edges. An unordered list of children nodes in the tree.
    std::vector<DecompositionNode*> children; 
    
    // Head and tail of children chain.
    DecompositionNode* child_head = nullptr;
    DecompositionNode* child_tail = nullptr;

    // Node id, Node type, is_reverse (optional)
    DecompositionNode(nid_t nid_, decomp_node_t type_, bool is_reverse_ = false);
    
    // Reverses the chain connecting the children and also the children themselves.
    void reverse();

    // Add unordered child (assumes this will only be run once per child per parent).
    void add_child(DecompositionNode* child);

    // Pushes a child node to the beginning of the chain.
    void push_front(DecompositionNode* child);

    // Pushes a child node to the end of the chain.
    void push_back(DecompositionNode* child); 
};

// Assigns the two ordered child nodes to a parent chain node.
// These child nodes are assumed to be the root node of their respective "trees".
// If a child node is also a chain node, its children will be moved over to the
// new node and the original parent will be freed.
DecompositionNode* create_chain_node(nid_t nid, DecompositionNode* first_node,
    DecompositionNode* second_node);

// Frees decomposition tree given root.
void free_tree(DecompositionNode* node);

#ifdef DEBUG_DECOMP_TREE
// Prints decomposition tree with "| " indicating depth.
void print_tree(DecompositionNode* node, int depth = 0);
#endif /* DEBUG_DECOMP_TREE */
#endif /* VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED */
