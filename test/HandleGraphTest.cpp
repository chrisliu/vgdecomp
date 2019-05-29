#include "../src/BidirectedGraph.hpp"
#include "../src/BidirectedGraphBuilder.hpp"

using namespace std;

int main(int argv, char* argc[]) {
    BidirectedGraphBuilder builder;
    BidirectedGraph* graph = builder.build_graph("test.json");

    bool ret = graph->for_each_handle([graph](const handle_t& h) {
        cout << graph->get_id(h) << " regular direction" << endl;
        graph->follow_edges(h, false, [graph] (const handle_t& n_h) {
            cout << graph->get_id(n_h) << endl;
        });

        cout << graph->get_id(h) << " opposite direction" << endl;
        graph->follow_edges(h, true, [graph] (const handle_t& n_h) {
            cout << graph->get_id(n_h) << endl;
        });
    });

    cout << "Exit: " << ((ret) ? "True" : "False") << endl;

    return 0;
}