#include <vector>
#include "NodeTraversal.hpp"
#include "Node.hpp"
#include "Edge.hpp"


using namespace std;

class Graph{
    private:
        vector<NodeTraversal*> vertices;
        vector<Edge*> edges;
        inline Node newNode(int64_t id);
        inline void addEdge(int64_t upstream_id, int64_t downstream_id, bool isDirected);
    
    public:
        void addVertex(int64_t id);
        void addDirectedEdge(int64_t upstream_id, int64_t downstream_id);
        void addUndirectedEdge(int64_t id1, int64_t id2);
        vector<Node*> connectedNodes(int64_t parentId);


};