
// Solves hackerrank problem:
// https://www.hackerrank.com/challenges/torque-and-development

// My approach:

// Each city must either have a library, or be connected to some city that has one.
// It doesn't matter how far apart they are... sounds like an MST, doesn't it!

// My solution is to augment the graph with a 'library' node, connect to each city
// with cost Clib. Then, generate an MST starting at this node.

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>
#include <sstream>
#include <queue>

using namespace std;

class AdjListNode
{
  
public:

  AdjListNode(uint32_t in_index)
    :
    index(in_index),
    visited(false)
  { }
  
  // Add a ONE-WAY edge to adjacent_node. Does not check if an edge already exists.
  void add_edge_oneway(AdjListNode& adjacent_node, uint32_t weight)
  {
    adjacent_nodes.push_back(&adjacent_node);
    weights.push_back(weight);
  }

  // Add a two-way edge to adjacent_node. Does not check if an edge already exists.
  // adjacent_node will be modified by adding an edge to this node.
  void add_edge_bidirectional(AdjListNode& adjacent_node, uint32_t weight)
  {
    adjacent_nodes.push_back(&adjacent_node);
    weights.push_back(weight);

    adjacent_node.adjacent_nodes.push_back(this);
    adjacent_node.weights.push_back(weight);
  }

  string to_string()
  {
    stringstream sstream;
    sstream << index << ":";
    auto nedges = adjacent_nodes.size();
    
    for (uint32_t i = 0; i < nedges; ++i)
      {
	sstream << " (" << adjacent_nodes[i]->index << " " << weights[i] << ")";
      }
    
    return sstream.str();
  }

  uint32_t index;
  bool visited;
  vector<uint32_t> weights;
  vector<AdjListNode*> adjacent_nodes;
};

// Graph data structure using adjacency list representation on top of STL vectors.
// You must specify the number of nodes in the graph on construction.
// Then, add each edge using the add_edge functions.
class AdjListGraph
{

public:

  AdjListGraph(uint32_t numnodes)
  {
    
    for(uint32_t i = 0; i < numnodes; ++i)
      {
	nodes.emplace_back(i);
      }
  }

  /*
  AdjListGraph(const AdjListGraph& other)
  {
    *this = other;
  }

  AdjListGraph& operator= (const AdjListGraph& other)
  {
    cout << "copied!" << endl;
    nodes = other.nodes;
    return* this;
  }
  */

  AdjListGraph(const AdjListGraph& other);
  AdjListGraph& operator=(const AdjListGraph& other);
   
  AdjListGraph(AdjListGraph&& other)
    :
    nodes(other.nodes)
  { }

  AdjListGraph& operator= (AdjListGraph&& other)
  {
    cout << "moved!" << endl;
    swap(nodes, other.nodes);
    return *this;
  }

  // add a ONE-WAY edge from start to end
  void add_edge_oneway(uint32_t start, uint32_t end, uint32_t weight)
  {
    nodes[start].add_edge_oneway(nodes[end], weight);
  }

  void add_edge_bidirectional(uint32_t n1, uint32_t n2, uint32_t weight)
  {
    nodes[n1].add_edge_bidirectional(nodes[n2], weight);
  }


  // represent as a string for debugging
  string to_string()
  {
    uint32_t nnodes = nodes.size();
    stringstream sstream;
    
    for (uint32_t i = 0; i < nnodes; ++i)
      {
	sstream << nodes[i].to_string() << endl;
      }
    
    return sstream.str();
  }

  // an edge to consider while building Prim's mst
  class prim_edge
  {
    
  public:
    
    prim_edge(uint32_t in_dest, uint32_t in_weight)
      :
      dest(in_dest),
      weight(in_weight)
    { }

    // Warning -- this operator is reversed!
    // This is for compatibility with STL priority queue, which is a max heap (we need a min heap)
    bool operator< (const prim_edge& other) const 
    {
      return other.weight < weight;
    }

    // destination node index
    uint32_t dest;

    // weight of the edge
    uint32_t weight;
    
  };

  void push_edges(AdjListNode& node, priority_queue<prim_edge>& pq)
  {
    uint32_t nedges = node.adjacent_nodes.size();
    for(uint32_t i = 0; i < nedges; ++i)
      {
	pq.emplace(node.adjacent_nodes[i]->index, node.weights[i]);
      }
  }
  
  // Create an MST using prim's algorithm; return the total weight of the MST
  uint32_t mst_totalweight_prims(uint32_t startnode)
  {
    uint32_t total = 0;
    
    // Reset visited flags
    for(auto it = nodes.begin(); it != nodes.end(); ++it)
      {
	it->visited=false;
      }

    priority_queue<prim_edge> pq;

    // add starting node
    nodes[startnode].visited = true;
    push_edges(nodes[0], pq);

    // repeatedly take the lightest edge so far and attempt to add it to the tree.
    // continue until there are no more visitable edges.
    while(!pq.empty())
      {
	prim_edge cheapest = pq.top();
	pq.pop();
	
	if(!nodes[cheapest.dest].visited)
	  {
	    nodes[cheapest.dest].visited = true;
	    total += cheapest.weight;
	    push_edges(nodes[cheapest.dest], pq);
	  }
      }

    return total;
  }
 
protected:
  vector<AdjListNode> nodes;
};


// Read the highly-specific hackerrank input in a total unsafe way
AdjListGraph readinput(uint32_t& ncities,
		       uint32_t& nroads,
		       uint32_t& clib,
		       uint32_t& croad)
{
  cin >> ncities;
  cin >> nroads;
  cin >> clib;
  cin >> croad;
  cin.ignore(512, '\n');
 
  // Graph is augmented with library node; hence, ncities + 1 total nodes.
  // The library node is at index 0.
  AdjListGraph graph(ncities + 1);

  // Add edge from library node to each other city; these edges represent building a library in a city
  for(uint32_t i = 1; i < ncities+1; ++i)
    {
      graph.add_edge_oneway(0, i, clib);
    }

  // Add roads
  for(uint32_t r = 0; r < nroads; ++r)
    {
      uint32_t c1, c2;
      cin >> c1;
      cin >> c2;
      cin.ignore(512, '\n');

      graph.add_edge_bidirectional(c1, c2, croad);
    }
      
  return graph;
}


int main()
{
  uint32_t nqueries, ncities, nroads, clib, croad;
  
  cin >> nqueries;
  cin.ignore(512, '\n');

  for (uint32_t q = 0; q < nqueries; ++q)
    {
      AdjListGraph graph = readinput(ncities, nroads, clib, croad);
      cout << graph.mst_totalweight_prims(0) << endl;
    }
  
  return 0;
}

