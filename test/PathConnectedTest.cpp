#include "../src/BidirectedGraph.hpp"
#include "../src/BidirectedGraphBuilder.hpp"
#include "../src/algorithms/find_path_connected.hpp"

using namespace std;

int main(int argv, char* argc[]){
    BidirectedGraphBuilder builder;
    BidirectedGraph graph = builder.build_graph("test/test.json");
    unordered_map<const handle_t, vector<handle_t> > map = find_path_connected(&graph);
    print_path_connected(&graph, map);
}
