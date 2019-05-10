# CGL Data Structures
Data structures used in preparation for VG modifications

## Graph Generator Script Usage
Tested with Python 3.6 (should be backward compatible with Python 2.7 if print function is imported from future)  
From root directory, run generator with  
```
python3 test/GraphGenerator.py <node count> <output filename>
```  
For help with parameters use
```
python3 test/GraphGenerator.py --help
```
Default values:  
Split Factor    = 0.25  # Percentage of nodes that will try to split  
Max Ultrabubble = 15    # Maximum length of ultrabubbles  
Acyclic         = True  # True if graph is acyclic  
Bundle          = False # False if bundles aren't present in graph  
Improper        = False # False if nodes aren't allowed to be traversed illegally  
