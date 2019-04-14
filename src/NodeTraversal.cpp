#include "NodeTraversal.hpp"

NodeTraversal::NodeTraversal(Node* node, bool isBackwards = false) 
: node(node), isBackwards(isBackwards) {

}

vector<Edge*> NodeTraversal::getTraversedEdges(vector<Edge*> edges) {
    return vector<Edge*>();
}

Node* NodeTraversal::getNode(){
    return node;
}