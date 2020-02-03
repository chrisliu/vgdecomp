#ifndef VG_ALGORITHMS_HANDLE_HPP_INCLUDED
#define VG_ALGORITHMS_HANDLE_HPP_INCLUDED

#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/handlegraph/mutable_handle_graph.hpp"
#include "../../deps/handlegraph/deletable_handle_graph.hpp"

using handle_t = handlegraph::handle_t;
using nid_t = handlegraph::nid_t;
using edge_t = handlegraph::edge_t;

using HandleGraph = handlegraph::HandleGraph;
using MutableHandleGraph = handlegraph::MutableHandleGraph;
using DeletableHandleGraph = handlegraph::DeletableHandleGraph;

#endif /* VG_ALGORITHMS_HANDLE_HPP_INCLUDED */