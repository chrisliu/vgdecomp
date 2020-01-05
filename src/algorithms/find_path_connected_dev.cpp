#include "find_path_connected_dev.hpp"

#include "strongly_connected_components.hpp"
#include "sccgraph.hpp"
#include "topological_sort.hpp"
#include "sccgraph.hpp"

using namespace std;
using namespace handlegraph;

template<typename T>
unordered_set<T> get_union(const unordered_set<T>& s1, const unordered_set<T>& s2) {
    unordered_set<T> result(s1);
    result.insert(s2.begin(), s2.end());
    return result;
}

/// See: https://stackoverflow.com/questions/21864857/find-all-connected-nodes-in-adjacency-list

unordered_map<nid_t, unordered_set<nid_t>> find_path_connected_dev(const HandleGraph& g) {
    /// Construct a SCC graph
    auto sccs = strongly_connected_components(&g);
    SCCGraph scc_graph(g, sccs);    

    /// Find topological order of each SCC
    auto topo_order = topological_order(&scc_graph);

    /// Find follow set for each whole SCC
    unordered_map<nid_t, unordered_set<nid_t>> scc_follow;
    
    /// Follow set of last one is nothing
    scc_follow[scc_graph.get_id(topo_order[sccs.size() - 1])] = unordered_set<nid_t>();

    /// Determine the rest of the follow sets in descending order
    for (int i = topo_order.size() - 2; i >= 0; i--) {
        nid_t cur_scc_id = scc_graph.get_id(topo_order[i]);
        scc_follow[cur_scc_id] = unordered_set<nid_t>();
        scc_graph.follow_edges(topo_order[i], false, [&](const handle_t& child_handle) {
            nid_t child_scc_id = scc_graph.get_id(child_handle);

            /// Early exit if the child SCC hasn't bee traversed
            if (!scc_follow.count(child_scc_id)) return;

            /// Add all nodes child SCC leads
            scc_follow[cur_scc_id] = get_union(scc_follow[cur_scc_id], scc_follow[child_scc_id]);
            /// Add all nodes in child SCC
            scc_follow[cur_scc_id] = get_union(scc_follow[cur_scc_id], sccs[child_scc_id]);
        });
    }

    /// Assign connected nodes to each node
    unordered_map<nid_t, unordered_set<nid_t>> connected_nodes;
    for (size_t i = 0; i < sccs.size(); i++) {
        /// Add all nodes in this SCC if there's more than one node in this SCC
        if (sccs[i].size() > 1) scc_follow[i] = get_union(scc_follow[i], sccs[i]);

        for (auto& id : sccs[i]) {
            connected_nodes[id] = scc_follow[i];
        }
    }

    return connected_nodes;
}