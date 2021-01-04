# VG Balanced Bundle Decomposition
Algorithms to find balanced bundles and use them to further decompose a variation graph.

## Project Objectives and Progress
### Main Objectives
 - [x] Bidirected graph with libhandlegraph.
 - [x] Balanced bundle finding algorithm described in *Describing the Local Structure of Sequence Graphs*.
 - [x] Bundle side adjancency relationship (none, weak, strong) algorithm.
 - [x] Path connectedness algorithm that finds all path connected nodes *(optimization needed)*.
 - [x] Port and test vg's acyclicity algorithm.
 - [x] Deserialize vg graph formats.
 - [x] Add algorithm to perform the 3 reduction actions.
 - [x] Improve underlying BidirectedGraph architecture (efficient mutability).
### Side Objectives
 - [ ] Deprecate BidirectedGraphBuilder and add `serialize(ofstream&)` and ~~`deserialize(ifstream&)`~~(COMPLETED) to BidirectedGraph. Also support vg JSON format instead of legacy JSON format.
 - [x] Improve graph_assistant.py (add command line options for various tasks and improve code legibility).

## Tools
 1. Graph Assistant [(Python3)](test/graph_assistant.py)  
 Create and convert custom test graphs. Supports custom vg-inspired JSON format, vg compliant JSON format, and old legacy JSON format.

## Dependencies
- catch2 @ [6c9a255dc2973e97b2d6a155d36f47e145382d32](https://github.com/catchorg/Catch2/commit/6c9a255dc2973e97b2d6a155d36f47e145382d32)
- libhandlegraph @ [9afe2df80fe60dab71d4121091dc5c9e80ca2b8d](https://github.com/vgteam/libhandlegraph/commit/c0deca121be29d8141a65509c40c9f8b872b36f1)
- jsoncpp @ [c0deca121be29d8141a65509c40c9f8b872b36f1](https://github.com/open-source-parsers/jsoncpp/commit/5b91551f3944d69e0090d6b6528852207de78078)
- sparsepp @ [209cb772c28272eb6b2157b79afe554c0102fb61](https://github.com/greg7mdp/sparsepp/commit/209cb772c28272eb6b2157b79afe554c0102fb61)


## Contributors
- Christopher Liu (Undergraduate researcher)
- Ryan Nishimoto (Undergraduate researcher)
- Yohei Rosen (PHD candidate)

## Works Referenced
1. (2017) Describing the Local Structure of Sequence Graphs - Yohei Rosen, Jordan Eizenga, and Benedict Paten [[link]](https://link.springer.com/chapter/10.1007/978-3-319-58163-7_2)
