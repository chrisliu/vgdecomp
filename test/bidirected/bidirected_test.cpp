#include <iostream>
#include <string>
#include <fstream>

#include "../../src/BidirectedGraph.hpp"

using namespace std;

bool remove_node = false;

void print_edges(BidirectedGraph& g) {
    g.for_each_handle([&](const handle_t& handle) {
        cout << "Node " << g.get_id(handle) << endl;
        bool printed = false;
        cout << "  > Left nodes: ";
        g.follow_edges(handle, true, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << (g.get_is_reverse(child_handle) ? "" : "r") << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
        printed = false;
        cout << "  > Right nodes: ";
        g.follow_edges(handle, false, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << (g.get_is_reverse(child_handle) ? "r" : "") << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
    });
}

int main(int argc, char* argv[]) {
    string filename = argv[argc - 1];
    ifstream json_file(filename, ifstream::binary);

    BidirectedGraph g;
    bool ret = g.deserialize(json_file);

    cout << "Serialization is a " << (ret ? "Success" : "Failure") << endl;

    // Check edges
    cout << "Original edges" << endl;
    print_edges(g);

    cout << "Min id: " << g.min_node_id() << endl;
    cout << "Max id: " << g.max_node_id() << endl;

    if (remove_node) {
        cout << "Removed node 1" << endl;
        g.destroy_handle(g.get_handle(1));
        print_edges(g);
    } else {
        cout << "Removed edge 1-2" << endl;
        g.destroy_edge(g.get_handle(1), g.get_handle(2));
        print_edges(g);
    }


    return 0;
}