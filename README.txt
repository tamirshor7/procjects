The following projects support the management and maintenance of two course related functionallities:

1. Course interaction and scheduling system-
    This system holds a database of faculties represented as directed graphs, with each node being
    a specific course. A directed edge from node 'v' to node 'u' would exist in any case v represents
    a course that is a preliminary requierement for the course represented by 'u', and only in such case.

    Supported Features
        - Calculating Intersecting, Unifying, Subtracting, Multiplying and Complementing graphs.
        - Mechanism for finding circular dependecies between courses
        - Saving and loading graphs as binaries to be used for calculations. Format specified later on in this document.
        - Swig based Python interface. Project can be fully operated from Python code.

    Interface:
        To define a new graph, describing a ceratin schedule of a certain facutly, use the following syntax:
        <Name> = {Course1_num, Course2_num, ... ,Course_n_num | <orig_node_edge1, dest_node_edge1>, ...,<orig_node_edge_n, dest_node_edge_n>}.
        Example : G1 = {1,2,3 | <1,2>} is a graph with nodes 1,2,3, and a single edge 1->2.
        The following signs - {+, ^, *, /, !} would be used to perform union, intersection, multiplication, subtraction 
                              and Complementing graph calculations respecitevely (in CLI), as described in the Graph.h header files.
        Example: G3 = G1 + G2. G3 would be the union of G1 and G2.

        use save(G) and load(bin_file) to save and load a graph G from said binary format (also described is Graph.h).
        use print(G) to display all nodes and edges of graph G (separated with '$' sign).

        *complex calculations, such as print(!G5 + load(file)*G3+(G6/G7)) , are supported. 
        Use parentheses to define calculation order.
        
        *Run executable with input and output files as arguments to run in non-interactive mode (file I/O).

2. Course classes management and viewing time analysis-
    This system holds a CLI user enterd database of courses and their classes, and also various ways to update and display
    lecture view time across various recorded courses. The main emphasis is efficient data handling - The application uses
    Hash tables, Dynamic arrays and binary search trees to acheive solid performances, as detailed in Library2.h header file.,
    along with full Functionallity.
    CLI API:
    Init - call init method. Must be called at the start of every viewing sesion.
    To use any other functionallity, type the function name exactly as it appears in the interface file (case-sensitive).
    Afterwards type in any argument the function takes, BESIDES POINTERS -  ptrs are internal parameters.

    Example Session:
    Init
    AddCourse 2 //add a new Course, with ID of 2.
    AddClass 2  // add a new lecture to Course with ID 2 - lecture id would be 0.
    AddClass 2  // add a new lecture to Course with ID 2 - lecture id would be 1.
    WatchClass 2 1 5 //add watch time of 5 hours to lecture 1 in course 2.
    Quit
    






    
