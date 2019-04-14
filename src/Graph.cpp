#include "Graph.hpp"

inline Node Graph::newNode(int64_t id){
    Node* node = new Node(id);
    return *node;
}

inline void Graph::addEdge(int64_t upstream_id, int64_t downstream_id, bool isDirected){
    Edge* edge = new Edge(upstream_id, downstream_id, isDirected, false);
    edges.push_back(edge);
}

void Graph::addVertex(int64_t id){
    Node node = newNode(id);
    NodeTraversal* nodeT = new NodeTraversal(&node, false);
    vertices.push_back(nodeT);
}
void Graph::addDirectedEdge(int64_t upstream_id, int64_t downstream_id){
    addEdge(upstream_id, downstream_id, true);
}
void Graph::addUndirectedEdge(int64_t upstream_id, int64_t downstream_id){
    addEdge(upstream_id, downstream_id, false);
}
vector<Node*> Graph::connectedNodes(int64_t parentId){
    vector<Node*> toReturn;
    NodeTraversal* nt;
    for(int i=0; i<vertices.size(); i++ ){
        nt = vertices[i];

        if((nt->getNode())->id==parentId){
            toReturn.push_back(nt->getNode());
        }
    }
    return toReturn;
}
