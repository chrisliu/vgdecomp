#!/usr/bin/env python3
import argparse
import json
import re
from typing import Callable, Optional

def main():
    parser = setup_parser()
    parsed = parser.parse_args()
    process_args(parsed)

def setup_parser():
    parser = argparse.ArgumentParser(
        prog = 'graph_assistant_dev',
        description = 'A tool to generate custom graphs in vg\'s json format.'
    )
    parser.add_argument('--tool-help', 
        action = 'store_true',
        help = 'explains format for adding nodes and edges (and bundles)'
    )
    parser.add_argument('-b', '--bundles',
        action = 'store_true',
        help = 'enable bundle declaration'
    )
    parser.add_argument('-d', '--default-nodes',
        action = 'store_true',
        help = 'don\'t ask for node sequences'
    )
    parser.add_argument('-c', '--vg-compliant',
        action = 'store_true',
        help = 'output another vg compliant version'
    )
    return parser

def process_args(parsed: argparse.Namespace):
    ## Tool help has highest precedence and exits when done
    if parsed.tool_help:
        display_tool_help()
        return

    ## Graph dictionary
    graph = dict()
    
    ## If not default nodes, ask to initialize node sequences
    if not parsed.default_nodes:
        prompt_nodes(graph)
    
    prompt_edges(graph)

    ## If bundles are needed, prompt
    if parsed.bundles:
        prompt_bundles(graph)

    ## If default nodes was asked, initialize nodes
    if parsed.default_nodes:
        init_nodes(graph)

    ## Prompt for a filename
    filename = prompt_name()

    ## Prompt for a description
    prompt_desc(graph)

    ## Reorder graph (aesthetic)
    graph = reorder(graph)

    ## Serialize non-vg compliant version
    serialize(graph, filename)

    ## If vg compliant version is asked, serialize one
    if parsed.vg_compliant:
        serialize_vg(graph, filename)

def display_tool_help():
    tool_help_msg = """    Node declaration stage:
    | Node: int
    | Sequence: str of ACTG (upper or lower case)
    | Type 'q' (upper or lower case) anytime to quit; the last complete node will be saved.

    Edge declaration stage:
    | Node1: node side format (from node side)
    | Node2: node side format (to node side)
    | 
    | Node side format: An integer followed by an optional l or r (upper or lower case)
    | | l indicates left side of a node and r indicates right side of a node
    | | By default node 1 is right side and node 2 is left side.
    | | ie.
    | | | 15l = left side of node 15
    |
    | Type 'q' (upper or lower case) anytime to quit; the last complete edge will be saved.

    Bundle declaration stage:
    | Left: bundleside node format
    | > bundleside node format
    | ...
    | Right: bundleside node format
    | > bundleside node format
    | ...
    |
    | Bundleside node format: An integer followed by an optional r (upper or lower case)
    | | r indicates if the handle will be reversed relative to the rest of the bundle
    | | By default the handles are in the same orientation
    | | ie.
    | | | 15 = node 15 oriented in the same direction as the rest of the bundle
    | | | 15r = node 15 oriented in the opposite direction as the rest of the bundle
    |
    | Type 'q' (upper or lower case) to stop declaring left or right side nodes.
    | Type 'qq' (upper or lower case) anytime to quit; the lats complete bundle will be saved.

    Filename stage:
    | Filenames will be reformatted such that spaces are replaced with underscores.
    | The custom vg styled json format will be outputted as (filename).json.
    | A vg compliant json format will be outputted as (filename)_vg.json.

    Description stage:
    | The custom vg styled json format allows for a description of the graph put directly in
    | the json file.
    """
    print(tool_help_msg)

def prompt_nodes(graph: dict):
    """Prompts for nodes in the graph and inserts them."""
    graph['node'] = list()

    node_pattern = re.compile('\d+')
    sequence_pattern = re.compile('[ACGTacgt]*')

    def valid_node(response: str) -> bool:
        return node_pattern.fullmatch(response) is not None

    def valid_sequence(response: str) -> bool:
        return sequence_pattern.fullmatch(response) is not None

    def add_node() -> bool:
        ## Prompt for id and sequence but allow for early exits
        ret, node_id = proper_input("| Node: ", valid_node, def_exit)
        if not ret: return False
        ret, sequence = proper_input("| Sequence: ", valid_sequence, def_exit)
        if not ret: return False

        ## Add node
        graph['node'].append(
            {
                'id': int(node_id),
                'sequence': sequence.upper()
            }
        )

        return True
    
    ## Keep adding nodes until exit
    print("Nodes:")
    while add_node(): pass

def prompt_edges(graph: dict):
    """Prompts for edges in the graph and inserts them."""
    graph['edge'] = list()

    node_side_pattern = re.compile('(\d+)([lLrR]?)')

    def valid_node_side(response: str) -> bool:
        return node_side_pattern.fullmatch(response) is not None
    
    def add_edge() -> bool:
        ## Prompt for node sides but allow for early exits
        ret, node1 = proper_input("| Node 1: ", valid_node_side, def_exit)
        if not ret: return False
        ret, node2 = proper_input("| Node 2: ", valid_node_side, def_exit)
        if not ret: return False

        ## Get edge information
        node1_match = node_side_pattern.fullmatch(node1)
        node2_match = node_side_pattern.fullmatch(node2)
        
        node1_id = int(node1_match.group(1))
        node2_id = int(node2_match.group(1))
        from_left = False if node1_match.group(2) in ['', 'r'] else True
        to_right = False if node2_match.group(2) in ['', 'l'] else True

        edge = {'from': node1_id, 'to': node2_id}
        if from_left: edge['from_start'] = True
        if to_right: edge['to_end'] = True
        graph['edge'].append(edge)

        return True

    ## Keep adding edges until exit
    print("Edges:")
    while add_edge(): pass

def prompt_bundles(graph: dict):
    graph['bundle'] = list()

    node_pattern = re.compile('(\d+)([rR]?)')

    def valid_node(response: str) -> bool:
        return node_pattern.fullmatch(response) is not None

    def extended_exit(response: str) -> bool:
        return def_exit(response) or response.lower() == 'qq'
    
    def add_bundle() -> bool:
        bundle = {'left': list(), 'right': list()}

        ## Left side
        is_first = True
        while True:
            ret, node = proper_input("| Left: " if is_first else "| > ",
                valid_node, extended_exit
            )
            if not ret:
                ## Quit if no more bundles are to be added
                if len(node) == 2:
                    return False
                ## Else end left side bundle nodes initialization
                break
            
            ## Add node to bundleside
            node_match = node_pattern.fullmatch(node)
            bundle['left'].append(
                {
                    'id': int(node_match.group(1)),
                    'is_reverse': False if node_match.group == '' else True
                }
            )

            ## Not first anymore
            is_first = False            
        
        ## Right side
        is_first = True
        while True:
            ret, node = proper_input("| Right: " if is_first else "| > ",
                valid_node, extended_exit
            )
            if not ret:
                ## Quit if no more bundles are to be added
                if len(node) == 2:
                    return False
                ## Else end left side bundle nodes initialization
                break
            
            ## Add node to bundleside
            node_match = node_pattern.fullmatch(node)
            bundle['right'].append(
                {
                    'id': int(node_match.group(1)),
                    'is_reverse': False if node_match.group == '' else True
                }
            )

            ## Not first anymore
            is_first = False 
        
        ## Added bundle to list of bundles
        graph['bundle'].append(bundle)
        
        return True

    print("Bundles:")
    while add_bundle(): pass

def init_nodes(graph: dict):
    """Gets all nodes in defined edges and assigns an empty sequence."""
    ## Get all nodes defined in 'edge'
    nodes = set()
    for edge in graph['edge']:
        nodes.add(edge['from'])
        nodes.add(edge['to'])    
    nodes = sorted(nodes)

    graph['node'] = [{'id': node_id, 'sequence': ''} for node_id in nodes]

def prompt_name() -> str:
    """Prompts and returns a formatted filename."""
    _, filename = proper_input("Filename: ", lambda s: s[-5:] != '.json')
    filename = filename.replace(' ', '_')
    filename = filename.strip('_')
    return filename

def prompt_desc(graph: dict):
    """Prompts and adds description to graph dictionary."""
    _, description = proper_input("Description: ")
    graph['description'] = description

def reorder(graph: dict) -> dict:
    """Reorder dictionary so graph info can be seen first."""
    order = ['description', 'node', 'edge', 'bundle']

    new_graph = dict()
    for key in order:
        if key in graph:
            new_graph[key] = graph[key]
    
    return new_graph

def serialize(graph: dict, filename: str):
    """Serializes a custom vg inspired json format."""
    with open('{}.json'.format(filename), 'w') as out:
        json.dump(graph, out, indent = 4)

def serialize_vg(graph: dict, filename: str):
    """Serializes a vg compliant json format."""
    vg_graph = dict()
    vg_graph['node'] = graph['node']
    vg_graph['edge'] = graph['edge']
    with open('{}_vg.json'.format(filename), 'w') as out:
        json.dump(vg_graph, out, indent = 4)

def proper_input(prompt: str, 
    check_fn: Optional[Callable[[str], bool]] = lambda _ : True, 
    exit_fn: Optional[Callable[[str], bool]] = lambda _ : False) -> str:
    """Keeps prompting until condition or exit is met."""
    while True:
        response = input(prompt)
        ## If exit, return None
        if exit_fn(response):
            return False, response
        
        ## If valid response, return response
        if check_fn(response):
            return True, response

def def_exit(response: str) -> bool:
    return response in ['q', 'Q']

if __name__ == '__main__':
    main()