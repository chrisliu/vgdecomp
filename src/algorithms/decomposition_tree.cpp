#include "decomposition_tree.hpp"
#include <unordered_set>
#include <deque>

#ifdef DEBUG_DECOMP_TREE
#include <iostream>
#endif /* DEBUG_DECOMP_TREE */

DecompositionNode::DecompositionNode(nid_t nid_, decomp_node_t type_, 
    bool is_reverse_)
    : nid(nid_)
    , type(type_)
    , is_reverse(is_reverse_)
{}

void DecompositionNode::reverse() {
    // If this is a derived node with a chain, perform a linked-list reverse.
    if (type == decomp_node_t::Chain) {
        DecompositionNode* previous = nullptr;
        DecompositionNode* conductor = child_head;
        DecompositionNode* temp; 
        while (conductor != nullptr) {
            temp = conductor->sibling;     // Keep track of the next node.
            conductor->sibling = previous; // Reverse the conductor's direction.
            previous = conductor;          // Set the new previous to conductor.
            conductor = temp;              // Shift conductor forward. 
        }
        // Flip head and tail pointers.
        temp = child_head;
        child_head = child_tail;
        child_tail = temp;
    }

    // If this is derived node, the children must be reversed too.
    if (type == decomp_node_t::Chain || type == decomp_node_t::Split) {
        for (auto& child : children) {
            child->reverse();
        }
    }

    // Flip the orientation status of this decomposition node.
    is_reverse = !is_reverse;
}

void DecompositionNode::add_child(DecompositionNode* child) {
    // Add child to unordered list of children.
    children.push_back(child);

    child->parent = this;
}

void DecompositionNode::push_front(DecompositionNode* child) {
    // Add child to the unordered list of children.
    add_child(child);

    // If there aren't any children, set new node as head and tail.
    if (child_head == nullptr) {
        child_head = child;
        child_tail = child;
        return;
    }
    
    // Set new node as head.
    child->sibling = child_head;
    child_head = child;
}

void DecompositionNode::push_back(DecompositionNode* child) {
    // Add child to the unordered list of children.
    add_child(child);

    // If there aren't any chlidren, set new node as head and tail.
    if (child_tail == nullptr) {
        child_head = child;
        child_tail = child;
        return;
    }

    // Set new node as tail.
    child_tail->sibling = child;
    child_tail = child;
}

DecompositionNode* create_chain_node(nid_t nid, DecompositionNode* first_node,
    DecompositionNode* second_node
) {
    // Get new derived chain node.
    DecompositionNode* new_node = new DecompositionNode(nid, decomp_node_t::Chain);

    // If the first node is a chain node, it could be used to create a longer chain.
    if (first_node->type == decomp_node_t::Chain) {
        // Copy children chain over to the new node.
        DecompositionNode* conductor = first_node->child_head;
        DecompositionNode* next;
        while (conductor != nullptr) {
            next = conductor->sibling;
            new_node->push_back(conductor);
            conductor = next;
        }
        // Free the original parent node.
        delete first_node;
    // Otherwise just put the split/source node as the first child.
    } else {
        new_node->push_back(first_node);
    }

    // If the second node is a chain node ...
    if (second_node->type == decomp_node_t::Chain) {
        // Copy children over to the new node
        DecompositionNode* conductor = second_node->child_head;
        DecompositionNode* next;
        while (conductor != nullptr) {
            next = conductor->sibling;
            new_node->push_back(conductor);
            conductor = next;
        }
        // Free the original parent node.
        delete second_node;
    // Otherwise just put the split/source node as the second child.
    } else {
        new_node->push_back(second_node);
    }
    
    return new_node;
}

DecompositionNode* find_lca(DecompositionNode* n1, DecompositionNode* n2) {
    /* TODO: Implement efficient LCA for trees.
    // The set of ancestors of n1.
    std::unordered_set<DecompositionNode*> n1ancestors;
    
    // The queue for BFS.
    std::deque<DecompositionNode*> queue = {n1};

    // Perform BFS on node 1 to find all ancestors.
    while (!queue.empty()) {
        DecompositionNode* next = queue.front();
        queue.pop_front();
        
        // For each parent of this node, only push to queue if it hasn't been
        // added.
        for (auto& parent : next->parents) {
            if (n1ancestors.insert(parent).second) queue.push_back(parent);
        }
    }

    // Perform BFS on node 2 to find all ancestors.
    queue.push_back(n2);
    while (!queue.empty()) {
        DecompositionNode* next = queue.front();
        queue.pop_front();

        // For each parent of this node, check if it's a shared ancestor.
        // If yes, return since it's the first*. Otherwise, continue BFS.
        // * May not be the "first" if searching a tree with R1 nodes. Then the
        // "least" ancestor has yet to be determined. 
        // TODO: Isn't implemented least in terms of LCA for a acyclic digraph. 
        // Could be an option though. See
        // https://en.wikipedia.org/wiki/Lowest_common_ancestor
        for (auto& parent : next->parents) {
            if (n1ancestors.count(parent)) return parent;
            queue.push_back(parent);
        }
    }
    */

    return nullptr;
}

void free_tree(DecompositionNode *node) {
    switch (node->type) {
        case Source: 
        case Epsilon: {
            delete node;
            break;
        }
        case Chain: {
            DecompositionNode* conductor = node->child_head;
            DecompositionNode* next;
            while (conductor != nullptr) {
                next = conductor->sibling;
                free_tree(conductor);
                conductor = next;
            }
            delete node;
            break;
        }
        case Split: {
            for (auto& child : node->children) {
                delete child;
            }
            delete node;
            break;
        }
    }
}

#ifdef DEBUG_DECOMP_TREE
inline void print_depth(int depth) {
    for (int i = 0; i < depth; i++) {
        std::cout << "| ";
    }
}

void DecompositionTreePrinter::print_node(DecompositionNode* node) {
    switch (node->type) {
        case Source:
            std::cout << "Source Node: " << node->nid 
                << (node->is_reverse ? "r" : "") << std::endl;
            break;
        case Epsilon:
            std::cout << "Epsilon Node: " << node->nid 
                << (node->is_reverse ? "r" : "") << std::endl;
            break;
        case Chain:
            std::cout << "Chain Node: " << node->nid 
                << (node->is_reverse ? "r" : "") << std::endl;
            break;
        case Split:
            std::cout << "Split Node: " << node->nid 
                << (node->is_reverse ? "r" : "") << std::endl;
            break;
    }
}

void DecompositionTreePrinter::print_tree(DecompositionNode* node) {
    print_tree(node, 0);
}

void DecompositionTreePrinter::print_tree(DecompositionNode* node, int depth) {
    // Print color corresponding to each node type.
    switch (node->type) {
        case Source: // No special color for source node.
            break;
        case Epsilon:
            std::cout << "\033[33m";
            break;
        case Chain:
            std::cout << "\033[34m";
            break;
        case Split:
            std::cout << "\033[35m";
            break;
    }

    print_depth(depth);
    print_node(node);

    std::cout << "\033[0m";

    // Print the node's information.
    switch (node->type) {
        case Source:
        case Epsilon: {
            break;
        }
        case Chain: {
            DecompositionNode* conductor = node->child_head;
            while (conductor != nullptr) {
                print_tree(conductor, depth + 1);
                conductor = conductor->sibling;
            }
            break;
        }
        case Split: {
            for (auto& child : node->children) {
                print_tree(child, depth + 1);
            }
            break;
        }
    }
}
#endif /* DEBUG_DECOMP_TREE */

