#include <iostream>

#include "../../src/algorithms/decomposition_tree.hpp"

int main(int argc, char* argv[]) {
    // Construct source nodes 1, 2r chained by 3.
    DecompositionNode* src1 = new DecompositionNode(1, Source);
    DecompositionNode* src2 = new DecompositionNode(2, Source, true);
    DecompositionNode* chain1 = create_chain_node(3, src1, src2);

    // Print chained source nodes.
    std::cout << "** Chain 1 **" << std::endl;
    print_tree(chain1);

    // Print reversed chained source nodes.
    chain1->reverse();
    std::cout << "** Chain 1 Reverse **" << std::endl;
    print_tree(chain1);

    // Construct source nodes 4, 5r, 6 chained by 7.
    DecompositionNode* src4 = new DecompositionNode(4, Source);
    DecompositionNode* src5 = new DecompositionNode(5, Source, true);
    DecompositionNode* src6 = new DecompositionNode(6, Source);
    DecompositionNode* chain2 = create_chain_node(7, src4, src5);
    chain2 = create_chain_node(7, chain2, src6);

    // Print chain2.
    std::cout << "** Chain 2 **" << std::endl;
    print_tree(chain2);

    // Construct source nodes 8, 9 split by 10.
    DecompositionNode* src8 = new DecompositionNode(8, Source);
    DecompositionNode* src9 = new DecompositionNode(9, Source);
    DecompositionNode* split1 = new DecompositionNode(10, Split); 
    split1->add_child(src8);
    split1->add_child(src9);

    // Print split1.
    std::cout << "** Split 1 **" << std::endl;
    print_tree(split1);

    // Construct derived nodes 3r, 7 chained by 11.
    DecompositionNode* chain3 = create_chain_node(11, chain1, chain2);

    // Print chain3.
    std::cout << "** Chain 3 **" << std::endl;
    print_tree(chain3);

    // Construct derived nodes 11r, 10 chained by 12
    chain3->reverse();
    DecompositionNode* chain4 = create_chain_node(12, chain3, split1);

    // Print chain4
    std::cout << "** Chain 4 **" << std::endl;
    print_tree(chain4);

    // Reverse chain4.
    chain4->reverse();
    std::cout << "** Chain 4 Reverse **" << std::endl;
    print_tree(chain4);

    // Build decomp tree to test LCA
    DecompositionNode* test_src1 = new DecompositionNode(1, Source);
    DecompositionNode* test_src2 = new DecompositionNode(2, Source);
    DecompositionNode* test_src3 = new DecompositionNode(3, Source);
    DecompositionNode* test_src4 = new DecompositionNode(4, Source);
    DecompositionNode* test_src5 = new DecompositionNode(5, Source);
    DecompositionNode* test_src6 = new DecompositionNode(6, Source);

    DecompositionNode* test_chain1 = create_chain_node(7, test_src4, test_src5);
    DecompositionNode* test_split1 = new DecompositionNode(8, Split);
    test_split1->add_child(test_chain1);
    test_split1->add_child(test_src6);
    DecompositionNode* test_chain2 = new DecompositionNode(9, Chain);
    test_chain2->push_back(test_src1);
    test_chain2->push_back(test_src2);
    test_chain2->push_back(test_split1);
    test_chain2->push_back(test_src3);

    std::cout << "** Test Chain 2 **" << std::endl;
    print_tree(test_chain2);

    DecompositionNode* lca1 = find_lca(test_src4, test_src5);
    std::cout << "(lca1) ";
    print_node(lca1);
    DecompositionNode* lca2 = find_lca(test_src4, test_src6);
    std::cout << "(lca2) ";
    print_node(lca2);
    DecompositionNode* lca3 = find_lca(test_src6, test_src2);
    std::cout << "(lca3) ";
    print_node(lca3);
    DecompositionNode* lca4 = find_lca(test_src4, test_src1);
    std::cout << "(lca4) ";
    print_node(lca4);
    DecompositionNode* lca5 = find_lca(test_src1, test_src4);
    std::cout << "(lca5) ";
    print_node(lca5);

    // Free trees
    free_tree(chain4);
    free_tree(test_chain2);

    return EXIT_SUCCESS;
}
