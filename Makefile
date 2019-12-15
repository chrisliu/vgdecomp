# A modified version of Wesley Mackey's Makefile

WARNING    = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
COMPILECPP = g++ -std=c++17 -g -O0 ${WARNING}
MAKEDEPCPP = g++ -std=c++17 -MM
VALGRIND   = valgrind --leak-check=full --show-reachable=yes

BG_SRCS    = src/BidirectedEdge.cpp src/BidirectedNode.cpp src/BidirectedGraph.cpp src/BidirectedGraphBuilder.cpp # Bidirected graph sources
ALGO_SRCS  = src/algorithms/find_balanced_bundles.cpp src/algorithms/bundle.cpp # Algorithm sources
HG_SRCS    = src/handlegraph/handle_graph.cpp # Handlegraph sources
JSON_SRCS  = src/json/jsoncpp.cpp # JSON Library Sources
SOURCES    = ${MAIN_PRG} ${BG_SRCS} ${ALGO_SRCS} ${HG_SRCS} ${JSON_SRCS}
OBJECTS    = ${SOURCES:.cpp=.o}

# Compiles all source files
sources : ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $< -o $@

# Removes all compiled code
clean :
	- rm ${OBJECTS}