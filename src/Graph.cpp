#include "Graph.hpp"

inline Node Graph::newNode(int64_t id){
    Node* node = new Node(id);
    return *node;
}

inline void Graph::addEdge(int64_t upstream_id, int64_t downstream_id, bool isDirected){

}