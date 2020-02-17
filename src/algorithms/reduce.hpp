#ifndef VG_ALGORITHMS_REDUCE_HPP_INCLUDED
#define VG_ALGORITHMS_REDUCE_HPP_INCLUDED

#include "../../deps/handlegraph/deletable_handle_graph.hpp"
#include "bundle.hpp"
#include "handle.hpp"

#define DEBUG_REDUCE

void test(DeletableHandleGraph& g);
bool reduce_graph(DeletableHandleGraph& g);

#endif /* VG_ALGORITHMS_REDUCE_HPP_INCLUDED */