#include <iostream>

#include "../../src/algorithms/decomposition_tree.hpp"

int main(int argc, char* argv[]) {
    // Construct source nodes 1, 2r chained by 3.
    DecompositionNode* src1 = new DecompositionNode(1, Source);
    DecompositionNode* src2 = new DecompositionNode(2, Source, true);
    DecompositionNode* chain1 = create_chain_node(3, src1, src2);

    // Print chained source nodes.
    print_tree(chain1);

    // Print reversed chained source nodes.
    chain1->reverse();
    print_tree(chain1);

    // Construct source nodes 4, 5r, 6 chained by 7.
    DecompositionNode* src4 = new DecompositionNode(4, Source);
    DecompositionNode* src5 = new DecompositionNode(5, Source, true);
    DecompositionNode* src6 = new DecompositionNode(6, Source);
    DecompositionNode* chain2 = create_chain_node(7, src4, src5);
    chain2 = create_chain_node(7, chain2, src6);

    // Print chain2.
    print_tree(chain2);

    // Construct source nodes 8, 9 split by 10.
    DecompositionNode* src8 = new DecompositionNode(8, Source);
    DecompositionNode* src9 = new DecompositionNode(9, Source);
    DecompositionNode* split1 = new DecompositionNode(10, Split); 
    split1->add_child(src8);
    split1->add_child(src9);

    // Print split1.
    print_tree(split1);

    // Construct derived nodes 3r, 7 chained by 11.
    DecompositionNode* chain3 = create_chain_node(11, chain1, chain2);

    // Print chain3.
    print_tree(chain3);

    // Construct derived nodes 11r, 10 chained by 12
    chain3->reverse();
    DecompositionNode* chain4 = create_chain_node(12, chain3, split1);

    // Print chain4
    print_tree(chain4);

    // Reverse chain4.
    chain4->reverse();
    print_tree(chain4);

    free_tree(chain4);

    return EXIT_SUCCESS;
}
