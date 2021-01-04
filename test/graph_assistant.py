#!/usr/bin/env python3
import argparse
import json
import re
from typing import List, Tuple, Callable, Optional

def main():
    parser = setup_parser()
    args = parser.parse_args()
    try:
        args.process(args)
    except:
        pass

def setup_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog = 'graph_assistant_dev',
        description = 'A tool to manipulate custom graphs in vg\'s json format.'
    )
    
    ## Add subparsers
    subparsers = parser.add_subparsers(help = "graph manipulation options")
    setup_create_parser(subparsers)
    setup_convert_parser(subparsers)

    return parser

def setup_create_parser(subparser: argparse._SubParsersAction):
    parser_create = subparser.add_parser(
        'create', 
        description = 'A tool to create graphs in a custom vg inspired format (and optionally a vg compliant format).',
        help = 'create json graphs'
    )
    parser_create.add_argument('--tool-help', 
        action = 'store_true',
        help = 'explains format for adding nodes and edges (and bundles)'
    )
    parser_create.add_argument('-b', '--bundles',
        action = 'store_true',
        help = 'enable bundle declaration'
    )
    parser_create.add_argument('-d', '--default-nodes',
        action = 'store_true',
        help = 'don\'t ask for node sequences'
    )
    parser_create.add_argument('-c', '--vg-compliant',
        action = 'store_true',
        help = 'output another vg compliant version'
    )
    parser_create.set_defaults(process = process_create)

def setup_convert_parser(subparser: argparse._SubParsersAction):
    parser_convert = subparser.add_parser(
        'convert',
        description = 'A tool to convert between json formats.',
        help = 'convert json formats'
    )
    input_group = parser_convert.add_argument_group('input formats', 'Supported input formats.')
    input_group.add_argument('-C', '--custom-in',
        action = 'store_const',
        help = 'input custom JSON format (default)',
        dest = 'input_format',
        const = 'custom',
        default = 'custom'
    )
    input_group.add_argument('-L', '--legacy-in',
        action = 'store_const',
        help = 'input legacy JSON format',
        dest = 'input_format',
        const = 'legacy',
    )
    input_group.add_argument('-D', '--dot-in',
        action = 'store_const',
        help = 'input DOT format',
        dest = 'input_format',
        const = 'dot',
    )
    output_group = parser_convert.add_argument_group('output formats', 'Supported output formats.')
    output_group.add_argument('-v', '--vg',
        action = 'store_const',
        help = 'output vg compliant JSON format (default)',
        dest = 'output_format',
        const = 'vg',
        default = 'vg'
    )
    output_group.add_argument('-c', '--custom',
        action = 'store_const',
        help = 'output custom JSON format',
        dest = 'output_format',
        const = 'custom'
    )
    parser_convert.add_argument('infile', 
        help = 'input filename'
    )
    parser_convert.add_argument('-o', '--outfile',
        help = 'output filename',
        default = None
    )
    parser_convert.set_defaults(process = process_convert)

def process_create(args: argparse.Namespace):
    """Creates a custom (and optionally a vg) JSON format graph."""
    ## Tool help has highest precedence and exits when done
    if args.tool_help:
        display_tool_help()
        return

    ## Graph dictionary
    graph = dict()
    
    ## If not default nodes, ask to initialize node sequences
    if not args.default_nodes:
        prompt_nodes(graph)
    
    prompt_edges(graph)

    ## If bundles are needed, prompt
    if args.bundles:
        prompt_bundles(graph)

    ## If default nodes was asked, initialize nodes
    if args.default_nodes:
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
    if args.vg_compliant:
        serialize_vg(graph, filename)

def process_convert(args: argparse.Namespace):
    """Converts input file format into output file format.

    Supported Conversions:
         To  Custom  VG  Legacy DOT
    From
    Custom     N     Y     N     N
    VG         N     N     N     N
    Legacy     Y     Y     N     N
    DOT        Y     Y     N     N
    """

    ## Verify input/output filenames
    if args.infile[-5:] != '.json':
        print("Error: Unsupported input file format (input file must be a json file).")
        return
    if args.outfile is not None and args.outfile[-5:] != '.json':
        print("Error: Unsupported output file format (output file must be a json file).")
        return

    ## Deserialize graph json
    try:
        with open(args.infile) as infile:
            if args.input_format == 'dot':
                graph = load_dot(infile)
            else:
                graph = json.load(infile)
    except FileNotFoundError:
        print("Error: Missing input file (input file not found).")
        return
    except json.decoder.JSONDecodeError:
        print("Error: JSON parse error (input file has malformed JSON).")
        return
    except:
        print("Error: Read error (some error occured when reading the input file).")
        return
    
    ## Convert to final output
    conv_graph = dict()
    # try:
    if args.input_format == 'legacy':
        if args.output_format in ['custom', 'vg']:
            ## Always convert to custom
            conv_graph = legacy_to_custom(graph)
            if args.output_format == 'vg':
                ## Convert custom to VG
                conv_graph = custom_to_vg(conv_graph)
        else:
            ## Raise exception
            print("Error: Unsupported operation (can't convert from {} to {}).".format(args.input_format, args.output_format))
            return
    elif args.input_format == 'custom':
        if args.output_format == 'vg':
            ## Convert to VG format
            conv_graph = custom_to_vg(graph)
        else:
            ## Raise exception
            print("Error: Unsupported operation (can't convert from {} to {}).".format(args.input_format, args.output_format))
            return
    elif args.input_format == 'dot':
        if args.output_format in ['custom', 'vg']:
            ## Always convert to custom
            conv_graph = dot_to_custom(graph)
            if args.output_format == 'vg':
                conv_graph = custom_to_vg(conv_graph)                
        else:
            ## Raise exception
            print("Error: Unsupported operation (can't convert from {} to {}).".format(args.input_format, args.output_format))
            return
    # except Exception as e:
    #     print(e)
    #     print("Error: Improper format (input file is not in the {} JSON format).".format(args.input_format))
    #     return
    
    ## Serialize
    ## Set output filename if needed
    outfile = args.outfile
    if outfile is None:
        outfile = args.infile[:-5] + '_' + args.output_format + args.infile[-5:]

    try:
        with open(outfile, 'w') as out:
            json.dump(conv_graph, out, indent = 4)
    except:
        print("Error: Dump error (some error occured when dumping the file).")
        return

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
    """Prompts for bundles in the graph and inserts them."""
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
    vg_graph = custom_to_vg(graph)
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
    """Default exit: Exits when 'q' (upper/lower) is given."""
    return response in ['q', 'Q']

## Conversion function
def load_dot(infile):
    """Converts DOT "json" into proper json outputs dictionary."""
    raw: str = infile.read()
    raw = raw.replace('"', '\\\"').replace('\'', '"').replace('True', 'true').replace('False', 'false')
    return json.loads(raw)

def custom_to_vg(graph: dict) -> dict:
    """Converts the custom JSON format to the vg JSON format."""
    vg_graph = dict()
    vg_graph['node'] = graph['node']
    vg_graph['edge'] = graph['edge']
    return vg_graph

def legacy_to_custom(graph: dict) -> dict:
    """Converts the legacy JSON format to the custom JSON format."""
    def conv_edge(edge: dict) -> dict:
        custom_edge = {'from': edge['id1'], 'to': edge['id2']}
        if edge['from_left']:
            custom_edge['from_start'] = True
        if edge['to_right']:
            custom_edge['to_end'] = True
        return custom_edge

    custom_graph = dict()
    custom_graph['description'] = graph['comment']
    custom_graph['node'] = [
        {'id': node['id'], 'sequence': ''}
         for node in graph['graph']['nodes']
    ]
    custom_graph['edge'] = [conv_edge(edge) for edge in graph['graph']['edges']]
    custom_graph['bundle'] = graph['bundles']
    return custom_graph

def dot_to_custom(graph: dict) -> dict:
    """Converts DOT format into the custom JSON format."""
    def parse_tooltips(tooltip: str) -> Tuple[Tuple[int, bool], Tuple[int, bool]]:
        nodes = tooltip.replace('"', '').split(' ')
        edges = list()
        for node in nodes:
            nid, is_reversed = node.split('-')
            edges.append((int(nid), False if is_reversed == '0' else '1'))
        return tuple(edges)

    def create_edge(handle1: Tuple[int, bool], handle2: Tuple[int, bool]):
        edge = {'from': handle1[0], 'to': handle2[0]}
        if handle1[1]:
            edge['from_start'] = True
        if handle2[1]:
            edge['to_end'] = True
        return edge
    
    ## For each edge
    nodes = set()
    edges = list()
    for edge in graph['links']:
        handle1, handle2 = parse_tooltips(edge['tooltip'])
        nodes.add(handle1[0])
        nodes.add(handle2[0])
        edges.append(create_edge(handle1, handle2))

    custom_graph = dict()
    custom_graph['description'] = ''
    custom_graph['node'] = [{'id': nid, 'sequence': ''} for nid in sorted(nodes)]
    custom_graph['edge'] = edges

    return custom_graph

if __name__ == '__main__':
    main()