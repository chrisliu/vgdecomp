#ifndef VG_FIND_PATH_CONNECTED_DEV_HPP_INCLUDED
#define VG_FIND_PATH_CONNECTED_DEV_HPP_INCLUDED

#include <unordered_set>
#include <unordered_map>

#include "handle.hpp"
#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/handlegraph/types.hpp"

std::unordered_map<nid_t, std::unordered_set<nid_t>> find_path_connected_dev(const HandleGraph& g);


#endif /* VG_FIND_PATH_CONNECTED_DEV_HPP_INCLUDED */
