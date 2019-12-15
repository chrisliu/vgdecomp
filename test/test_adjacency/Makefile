# A modified version of Wesley Mackey's Makefile

# Relative path of this directory to the source
RELPATH    = ../../

WARNING    = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
COMPILECPP = g++ -std=c++17 -g -O0 ${WARNING}

# Main program
MAIN_PRG   = adjacency_test.cpp
# Bidirected graph sources
BG_SRCS    = ${RELPATH}src/BidirectedEdge.cpp ${RELPATH}src/BidirectedNode.cpp \
	${RELPATH}src/BidirectedGraph.cpp ${RELPATH}src/BidirectedGraphBuilder.cpp 
# Algorithm sources
ALGO_SRCS  = ${RELPATH}src/algorithms/find_balanced_bundles.cpp \
	${RELPATH}src/algorithms/bundle.cpp 
# Handlegraph sources
HG_SRCS    = ${RELPATH}src/handlegraph/handle_graph.cpp
# JSON library sources
JSON_SRCS  = ${RELPATH}src/json/jsoncpp.cpp 
# Compiled sources and objects
SOURCES    = ${MAIN_PRG} ${BG_SRCS} ${ALGO_SRCS} ${HG_SRCS} ${JSON_SRCS}
OBJECTS    = ${SOURCES:.cpp=.o}
# Executable binary
EXECBIN    = AdjacencyTest 

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o${EXECBIN} ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $< -o $@

# Removes all intermediate object files but keeps the executable binary
clean :
	- rm ${OBJECTS}

# Removes all generated files including the executable binary
spotless : clean
	- rm ${EXECBIN}