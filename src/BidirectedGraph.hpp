#ifndef BidirectedGraph_hpp
#define BidirectedGraph_hpp
#include "BidirectedEdge.hpp"
#include <vector>
#include <unordered_map>
#include <set>
using namespace std;
class BidirectedGraph {
    private:
        unordered_map<uint64_t, vector<uint64_t> > reachable_nodes;
        unordered_map<uint64_t, vector<BidirectedEdge> > edges;
        void mod_BFS(uint64_t);
    public:
        vector<uint64_t> get_reachable_nodes(uint64_t id);
        void add_edge(uint64_t id1, uint64_t id2, bool from_left, bool to_right);
        bool is_acyclic();
        void populate_reachable_nodes();
        void print_reachable_nodes();
};
#endif