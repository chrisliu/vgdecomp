#ifndef Graph_hpp
#define Node_hpp
#include <vector>
#include "NodeTraversal.hpp"
#include "Node.hpp"
#include "Edge.hpp"


using namespace std;

class Graph{
    private:
        vector<NodeTraversal*> vertices;
        vector<Edge*> edges;
        inline Node new_node(int64_t id);
        inline void add_edge(int64_t upstream_id, int64_t downstream_id, bool isDirected);
    
    public:
        void add_vertex(int64_t id);
        void add_directed_edge(int64_t upstream_id, int64_t downstream_id);
        void add_undirected_edge(int64_t id1, int64_t id2);
        vector<Edge*> edges_on_start(int64_t id);
        vector<Edge*> edges_on_end(int64_t id);


};

#endif /* Graph_hpp */