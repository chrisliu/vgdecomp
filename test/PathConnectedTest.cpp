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
    files.push_back("test/path_connected_tests/trivial_cycle.json");
    files.push_back("test/bundle_test_graphs/30_two_strongly_adjacent_bundles.json");
    files.push_back("test/bundle_test_graphs/40_two_strongly_adjacent_bundles_flipped.json");
    files.push_back("test/bundle_test_graphs/31_two_weakly_adjacent_bundles.json");
    files.push_back("test/bundle_test_graphs/41_two_weakly_adjacent_bundles_flipped.json");
    files.push_back("test/bundle_test_graphs/42_triple_cyclic_bundles.json");
    files.push_back("test/bundle_test_graphs/15_acyclic_useless_bundle.json");
    files.push_back("test/bundle_test_graphs/04_complex_cyclic_useless_bundle.json");
    
    
    BidirectedGraphBuilder builder;
    vector<string>::iterator iter = files.begin();
    while(iter!=files.end()){
        BidirectedGraph graph = builder.build_graph(*iter);
        cout << *iter << "\n";
        unordered_map<const handle_t, vector<handle_t> > map = find_path_connected(&graph);
        print_path_connected(&graph, map);
        iter++;
    }

}
