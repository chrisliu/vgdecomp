#include "NodeTraversal.hpp"

NodeTraversal::NodeTraversal(Node* node, bool isBackwards) 
: node(node), isBackwards(isBackwards) {

}

int64_t NodeTraversal::getId() {
    return node->id;
}

bool NodeTraversal::reverse() {
    isBackwards = !isBackwards;
    return isBackwards;
}

vector<Edge*> NodeTraversal::getTraversedEdges(vector<Edge*> edges) {
    // Should be optimized to use <algorithms> as much as possible

    vector<Edge*> traversed;
    for (Edge* edge : edges) { // Make compatible for older versions of C++
        if (isBackwards) {
            edge = edge->reverse();
        }
        
        if (edge->traverse(node->id) != -1) {
            traversed.push_back(edge);
        }
    }

    return traversed;
}