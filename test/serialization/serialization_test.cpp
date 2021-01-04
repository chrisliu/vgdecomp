#include <iostream>
#include <string>
#include <fstream>

#include "../../src/BidirectedGraph.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string filename = argv[argc - 1];
    ifstream json_file(filename, ifstream::binary);

    BidirectedGraph g;
    bool ret = g.deserialize(json_file);

    cout << (ret ? "Is" : "Isn't") << " a success!" << endl;

    g.for_each_handle([&](const handle_t& handle) {
        bool printed = false;
        cout << "Node " << g.get_id(handle) << " left nodes: ";
        g.follow_edges(handle, true, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;

        printed = false;
        cout << "Node " << g.get_id(handle) << " right nodes: ";
        g.follow_edges(handle, false, [&](const handle_t& child_handle) {
            cout << g.get_id(child_handle) << ", ";
            printed = true;
        });
        if (printed) cout << "\b\b  ";
        cout << endl;
    });

    ofstream out_file("test.json");
    g.serialize(out_file);
    out_file.close();

    return 0;    
}