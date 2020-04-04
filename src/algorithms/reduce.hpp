#ifndef VG_ALGORITHMS_REDUCE_HPP_INCLUDED
#define VG_ALGORITHMS_REDUCE_HPP_INCLUDED

#include "../../deps/handlegraph/deletable_handle_graph.hpp"
#include "bundle.hpp"
#include "handle.hpp"
#include "astree.h"

#define DEBUG_REDUCE

astree* reduce_graph(DeletableHandleGraph& g);

#endif /* VG_ALGORITHMS_REDUCE_HPP_INCLUDED */