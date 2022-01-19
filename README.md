


# Object ID referencing storage solutions


## Solution 1

Memory between all Nodes is unified. Each Node has it's own range of addresses
(object IDs). Object cannot be moved between Nodes - similar behaviour can be
obtained by allocating new object as a copy of old one and then changing all
references from old object to new object.

#### Pros:
- very low memory footprint
- very fast

#### Cons:
- cannot move objects between Nodes

#### Memory usage:
```cpp
array<Node*> nodes;

map<Pointer, Object*> localObjects;
map<PointersRange, Node*> mappingPointerRangesToNodes;
```

localObjects object should be made as an array, or array of arrays for
optimization. It is possible because local Pointers should be have continous
values.

PointersRange can cover milions or bilions of pointers.





## Solution 2

Each object has it's own id. Objects can be moved freely between Nodes. When
sending request to a node that does not have an object but has metadata to it,
then that Node forwads received request.

To find an object it is requeired to send a multicast message that circulates
through Node and looks for an object.

#### Pros:
- free movement of objects between Nodes

#### Cons:
- massive memory footprint (each object must have metadata in a lot of Nodes)
- bandwith overhead - a lot of packets needs to be sent to find a Node that owns
an object.

#### Memory usage:
```cpp
array<Node*> nodes;

map<Pointer, Object*> localObjects;
map<Pointer, Node*> mappingPointerRangesToNodes;
```




## Solution 3:

Merged first and second solutions. Each object can be moved freely between
Nodes. Each object has it's original owner Node, which always has it's full
metadata. Original owner Node should take care of moving objects between other
Nodes.

#### Pros:
- free movement of objects between Nodes
- mostly low memory usage
- fast - low (but existent) bandwidth overhead for discovering object owning
Nodes.

#### Cons:
- high memory usage for Origin Node - only when a Node allocates and moves them
away without clearing (or long living objects).

#### Memory usage:
```cpp
array<Node*> Nodes;

struct ObjectReference {
	union {
		Object* itIsLocalObject;
		Node* itIsDelegatedObjectToOtherNode;
	};
	bool isDelegated;
};

map<Pointer, ObjectReference> localObjects;
map<PointersRange, Node*> mappingPointerRangesToNodes;
```

localObjects object should be made as an array, or array of arrays for
optimization. It is possible because local Pointers should be have continous
values.

PointersRange can cover milions or bilions of pointers.












## Application structure

#### Solution 1:

 Layer | Module | Mechanisms
-------|--------|------------
1 | Basic networking (passing messages - byte arrays of given data size) | TCP(TLS) / UDP(DTLS)
2 | Network mesh grid with routing to indirectly connected nodes | sending messages to nodes by NodeID
3 | RPC/RMI | Based on data serialisation (with correct type assumption for optimisation)




#### Solution 2:

 Layer | Module | Mechanisms
-------|--------|------------
1 | Basic networking (passing messages - byte arrays of given data size) | TCP(TLS) / UDP(DTLS)
2 | serialization |
3 | Routing ; automatic establishing/selecting leader | leader provides nodes with theirs id's ; some uniform data
4 | RPC (by NodeID)
5 | Finding location of distributed objects
6 | objects management | allocating ; transfering between nodes ; freeing ; storing in database (?)
7 | RMI (by object uniform pointer/URI/UUID/SnowflakeID))





