#ifndef VG_ALGORITHMS_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
#define VG_ALGORITHMS_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <unordered_set>
#include "handle.hpp"
#include "dfs.hpp"

using namespace std;

/// Find all of the nodes with no edges on their left sides.
vector<unordered_set<nid_t>> strongly_connected_components(const HandleGraph* g);
    
#endif
