#include "BidirectedGraphBuilder.hpp"

BidirectedGraph* buildGraph(std::string filename) { // No error checks
    std::ifstream graph_file(filename, std::ifstream::binary);

    Json::Reader reader;
    Json::Value graph_json;
    bool resp = reader.parse(graph_file, graph_json);
    graph_file.close();

    if (!resp) {
#ifdef DEBUG
        std::cout << reader.getFormatedErrorMessages() << std::endl;
#endif
        return nullptr;
    }

    if (graph_json["graph"] == nullptr || 
        graph_json["graph"]["nodes"] == nullptr || graph_json["graph"]["edges"] == nullptr) {
#ifdef DEBUG
        std::cout << "Improper input format" << std::endl;
#endif
        return nullptr;
    }

    BidirectedGraph* graph = new BidirectedGraph();

    Json::Value nodes = graph_json["graph"]["nodes"];
    for (int i = 0; i < nodes.size(); i++) {
        graph->insertVertex(nodes[i]["id"].asInt64);
    }

    Json::Value edges = graph_json["graph"]["edges"];
    for (int i = 0; i < edges.size(); i++) {
        nid_t id1 = edges[i]["id1"].asInt64;
        nid_t id2 = edges[i]["id2"].asInt64;
        bool from_left = edges[i]["from_left"].asBool;
        bool to_right  = edges[i]["to_left"].asBool;

        graph->insertEdge(id1, id2, from_left, to_right);
    }

    return graph;
}

bool BidirectedGraphBuilder::saveGraph(std::string filename, BidirectedGraph graph) {
    /** To be completed when BidirectedGraph is fully complete
     *  Needs access to a list of vertices and edges in graph
     *  See link below to remove duplicate edges
     *  https://stackoverflow.com/questions/1041620/whats-the-most-efficient-way-to-erase-duplicates-and-sort-a-vector
     */
    return true;
}