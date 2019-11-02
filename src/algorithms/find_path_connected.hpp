#ifndef VG_ALGORITHMS_FIND_PATH_CONNECTED_HPP
#define VG_ALGORITHMS_FIND_PATH_CONNECTED_HPP

#include "../BidirectedGraph.hpp"
#include <unordered_map>
#include <vector>

std::unordered_map<const handle_t, vector<handle_t> > find_path_connected(const HandleGraph* g);
void print_path_connected(const HandleGraph* g, unordered_map<const handle_t, vector<handle_t> > map);

#endif