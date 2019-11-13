#include "../src/BidirectedGraph.hpp"
#include "../src/BidirectedGraphBuilder.hpp"
#include "../src/algorithms/find_path_connected.hpp"

using namespace std;
vector<string> files;
int main(int argv, char* argc[]){
    files.push_back("test/test.json");
    files.push_back("test/path_connected_tests/bubble.json");
    files.push_back("test/path_connected_tests/bubble_reversals.json");
    files.push_back("test/path_connected_tests/chain.json");
    files.push_back("test/path_connected_tests/chain_reversals.json");
    files.push_back("test/path_connected_tests/nested_bubbles.json");
    files.push_back("test/path_connected_tests/two_bubbles_adjacent.json");

    BidirectedGraphBuilder builder;
    vector<string>::iterator iter = files.begin();
    while(iter!=files.end()){
        BidirectedGraph graph = builder.build_graph(*iter);
        unordered_map<const handle_t, vector<handle_t> > map = find_path_connected(&graph);
        cout << *iter << "\n";
        print_path_connected(&graph, map);
    }

}
