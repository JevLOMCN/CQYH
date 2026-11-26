#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H
#pragma warning (disable:4786)

//------------------------------------------------------------------------
//
//  Name:   GraphSearches.h
//
//  Desc:   classes to implement graph algorithms, including DFS, BFS,
//          Dijkstra's, A*, Prims etc. (based upon the code created
//          by Robert Sedgewick in his book "Algorithms in C++")
//
//          Any graphs passed to these functions must conform to the
//          same interface used by the SparseGraph
//          
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <list>
#include <queue>
#include <stack>

#include "SparseGraph.h"
#include "PriorityQueue.h"

//----------------------- Graph_SearchDijkstra --------------------------------
//
//  Given a graph, source and optional target this class solves for
//  single source shortest paths (without a target being specified) or 
//  shortest path from source to target.
//
//  The algorithm used is a priority queue implementation of Dijkstra's.
//  note how similar this is to the algorithm used in Graph_MinSpanningTree.
//  The main difference is in the calculation of the priority in the line:
//  
//  double NewCost = m_CostToThisNode[best] + pE->Cost;
//------------------------------------------------------------------------
template <class graph_type>
class Graph_SearchDijkstra
{
private:

  //create a typedef for the edge type used by the graph
  typedef typename graph_type::EdgeType Edge;
  
private:

  const graph_type&             m_Graph;

  //this vector contains the edges that comprise the shortest path tree -
  //a directed subtree of the graph that encapsulates the best paths from 
  //every node on the SPT to the source node.
  std::vector<const Edge*>      m_ShortestPathTree;

  //this is indexed into by node index and holds the total cost of the best
  //path found so far to the given node. For example, m_CostToThisNode[5]
  //will hold the total cost of all the edges that comprise the best path
  //to node 5, found so far in the search (if node 5 is present and has 
  //been visited)
  std::vector<double>            m_CostToThisNode; 

  //this is an indexed (by node) vector of 'parent' edges leading to nodes 
  //connected to the SPT but that have not been added to the SPT yet. This is
  //a little like the stack or queue used in BST and DST searches.
  std::vector<const Edge*>     m_SearchFrontier;

  int                           m_iSource;
  int                           m_iTarget;

  void Search();

public:

  Graph_SearchDijkstra(const graph_type   &graph,
                       int           source,
                       int           target = -1):m_Graph(graph),
                                       m_ShortestPathTree(graph.NumNodes()),                              
                                       m_SearchFrontier(graph.NumNodes()),
                                       m_CostToThisNode(graph.NumNodes()),
                                       m_iSource(source),
                                       m_iTarget(target)
  {                                           
    Search();     
  }
 
  //returns the vector of edges that defines the SPT. If a target was given
  //in the constructor then this will be an SPT comprising of all the nodes
  //examined before the target was found, else it will contain all the nodes
  //in the graph.
  std::vector<const Edge*> GetSPT()const{return m_ShortestPathTree;}

  //returns a vector of node indexes that comprise the shortest path
  //from the source to the target. It calculates the path by working
  //backwards through the SPT from the target node.
  std::list<int> GetPathToTarget()const;

  //returns the total cost to the target
  double GetCostToTarget()const{return m_iTarget >= 0 && (size_t)m_iTarget < m_CostToThisNode.size() ? m_CostToThisNode[m_iTarget] : 0.0;}

  //returns the total cost to the given node
  double GetCostToNode(unsigned int nd)const{return nd < m_CostToThisNode.size() ? m_CostToThisNode[nd] : 0.0;}
};


//-----------------------------------------------------------------------------
template <class graph_type>
void Graph_SearchDijkstra<graph_type>::Search()
{
  if (m_iSource < 0 || (size_t)m_iSource >= m_SearchFrontier.size() ||
      m_iTarget < -1 || (m_iTarget >= 0 && (size_t)m_iTarget >= m_SearchFrontier.size())) return;
  //create an indexed priority queue that sorts smallest to largest
  //(front to back).Note that the maximum number of elements the iPQ
  //may contain is N. This is because no node can be represented on the 
  //queue more than once.
  IndexedPriorityQLow<double> pq(m_CostToThisNode, m_Graph.NumNodes());

  //put the source node on the queue
  pq.insert(m_iSource);

  //while the queue is not empty
  while(!pq.empty())
  {
    //get lowest cost node from the queue. Don't forget, the return value
    //is a *node index*, not the node itself. This node is the node not already
    //on the SPT that is the closest to the source node
    int NextClosestNode = pq.Pop();

    //move this edge from the frontier to the shortest path tree
    m_ShortestPathTree[NextClosestNode] = m_SearchFrontier[NextClosestNode];

    //if the target has been found exit
	if (NextClosestNode == m_iTarget)
	{
		return;
	}

    //now to relax the edges.
    graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, NextClosestNode);

    //for each edge connected to the next closest node
    for (const Edge* pE=ConstEdgeItr.begin();
        !ConstEdgeItr.end();
        pE=ConstEdgeItr.next())
    {
      if (!pE || pE->To() < 0 || (size_t)pE->To() >= m_SearchFrontier.size()) continue;

      //the total cost to the node this edge points to is the cost to the
      //current node plus the cost of the edge connecting them.
      double NewCost = m_CostToThisNode[NextClosestNode] + pE->Cost();

      //if this edge has never been on the frontier make a note of the cost
      //to get to the node it points to, then add the edge to the frontier
      //and the destination node to the PQ.
      if (m_SearchFrontier[pE->To()] == 0)
      {
        m_CostToThisNode[pE->To()] = NewCost;

        pq.insert(pE->To());

        m_SearchFrontier[pE->To()] = pE;
      }

      //else test to see if the cost to reach the destination node via the
      //current node is cheaper than the cheapest cost found so far. If
      //this path is cheaper, we assign the new cost to the destination
      //node, update its entry in the PQ to reflect the change and add the
      //edge to the frontier
      else if ( (NewCost < m_CostToThisNode[pE->To()]) &&
                (m_ShortestPathTree[pE->To()] == 0) )
      {
        m_CostToThisNode[pE->To()] = NewCost;

        //because the cost is less than it was previously, the PQ must be
        //re-sorted to account for this.
        pq.ChangePriority(pE->To());

        m_SearchFrontier[pE->To()] = pE;
      }
    }
  }
}

//-----------------------------------------------------------------------------
template <class graph_type>
std::list<int> Graph_SearchDijkstra<graph_type>::GetPathToTarget()const
{
  std::list<int> path;
  if (m_iSource < 0 || (size_t)m_iSource >= m_ShortestPathTree.size() ||
      m_iTarget < 0 || (size_t)m_iTarget >= m_ShortestPathTree.size()) return path;
  int nd = m_iTarget;
  for (size_t steps = 0; nd != m_iSource; ++steps)
  {
    if (nd < 0 || (size_t)nd >= m_ShortestPathTree.size() ||
        steps >= m_ShortestPathTree.size() || !m_ShortestPathTree[nd])
      return std::list<int>();
    path.push_front(nd);
    nd = m_ShortestPathTree[nd]->From();
  }
  path.push_front(m_iSource);
  return path;
}

//------------------------------- Graph_SearchAStar --------------------------
//
//  this searchs a graph using the distance between the target node and the 
//  currently considered node as a heuristic.
//
//  This search is more commonly known as A* (pronounced Ay-Star)
//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
class Graph_SearchAStar
{
private:

  //create a typedef for the edge type used by the graph
  typedef typename graph_type::EdgeType Edge;

private:

  const graph_type&              m_Graph;

  //indexed into my node. Contains the 'real' accumulative cost to that node
  std::vector<double>             m_GCosts; 

  //indexed into by node. Contains the cost from adding m_GCosts[n] to
  //the heuristic cost from n to the target node. This is the vector the
  //iPQ indexes into.
  std::vector<double>             m_FCosts;

  std::vector<const Edge*>       m_ShortestPathTree;
  std::vector<const Edge*>       m_SearchFrontier;

  int                            m_iSource;
  int                            m_iTarget;

  //the A* search algorithm
  void Search();

public:

  Graph_SearchAStar(graph_type &graph,
                    int   source,
                    int   target):m_Graph(graph),
                                  m_ShortestPathTree(graph.NumNodes()),                              
                                  m_SearchFrontier(graph.NumNodes()),
                                  m_GCosts(graph.NumNodes(), 0.0),
                                  m_FCosts(graph.NumNodes(), 0.0),
                                  m_iSource(source),
                                  m_iTarget(target)
  {
    Search();   
  }
 
  //returns the vector of edges that the algorithm has examined
  std::vector<const Edge*> GetSPT()const{return m_ShortestPathTree;}

  //returns a vector of node indexes that comprise the shortest path
  //from the source to the target
  std::list<int> GetPathToTarget()const;

  //returns the total cost to the target
  double GetCostToTarget()const{return m_iTarget >= 0 && (size_t)m_iTarget < m_GCosts.size() ? m_GCosts[m_iTarget] : 0.0;}
};

//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
void Graph_SearchAStar<graph_type, heuristic>::Search()
{
  if (m_iSource < 0 || (size_t)m_iSource >= m_SearchFrontier.size() ||
      m_iTarget < 0 || (size_t)m_iTarget >= m_SearchFrontier.size()) return;
  //create an indexed priority queue of nodes. The nodes with the
  //lowest overall F cost (G+H) are positioned at the front.
  IndexedPriorityQLow<double> pq(m_FCosts, m_Graph.NumNodes());

  //put the source node on the queue
  pq.insert(m_iSource);

  //while the queue is not empty
  while(!pq.empty())
  {
    //get lowest cost node from the queue
    int NextClosestNode = pq.Pop();

    //move this node from the frontier to the spanning tree
    m_ShortestPathTree[NextClosestNode] = m_SearchFrontier[NextClosestNode];

    //if the target has been found exit
	if (NextClosestNode == m_iTarget)
	{
		return;
	}

    //now to test all the edges attached to this node
    graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, NextClosestNode);

    for (const Edge* pE=ConstEdgeItr.begin();
        !ConstEdgeItr.end(); 
         pE=ConstEdgeItr.next())
    {
      if (!pE || pE->To() < 0 || (size_t)pE->To() >= m_SearchFrontier.size()) continue;

      //calculate the heuristic cost from this node to the target (H)                       
      double HCost = heuristic::Calculate(m_Graph, m_iTarget, pE->To()); 

      //calculate the 'real' cost to this node from the source (G)
      double GCost = m_GCosts[NextClosestNode] + pE->Cost();

      //if the node has not been added to the frontier, add it and update
      //the G and F costs
      if (m_SearchFrontier[pE->To()] == NULL)
      {
        m_FCosts[pE->To()] = GCost + HCost;
        m_GCosts[pE->To()] = GCost;

        pq.insert(pE->To());

        m_SearchFrontier[pE->To()] = pE;
      }

      //if this node is already on the frontier but the cost to get here
      //is cheaper than has been found previously, update the node
      //costs and frontier accordingly.
      else if ((GCost < m_GCosts[pE->To()]) && (m_ShortestPathTree[pE->To()]==NULL))
      {
        m_FCosts[pE->To()] = GCost + HCost;
        m_GCosts[pE->To()] = GCost;

        pq.ChangePriority(pE->To());

        m_SearchFrontier[pE->To()] = pE;
      }
    }
  }
}

//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
std::list<int> Graph_SearchAStar<graph_type, heuristic>::GetPathToTarget()const
{
  std::list<int> path;
  if (m_iSource < 0 || (size_t)m_iSource >= m_ShortestPathTree.size() ||
      m_iTarget < 0 || (size_t)m_iTarget >= m_ShortestPathTree.size()) return path;
  int nd = m_iTarget;
  for (size_t steps = 0; nd != m_iSource; ++steps)
  {
    if (nd < 0 || (size_t)nd >= m_ShortestPathTree.size() ||
        steps >= m_ShortestPathTree.size() || !m_ShortestPathTree[nd])
      return std::list<int>();
    path.push_front(nd);
    nd = m_ShortestPathTree[nd]->From();
  }
  path.push_front(m_iSource);
  return path;
} 



//---------------------- Graph_MinSpanningTree --------------------------------
//
//  given a graph and a source node you can use this class to calculate
//  the minimum spanning tree. If no source node is specified then the 
//  algorithm will calculate a spanning forest starting from node 1 
//
//  It uses a priority first queue implementation of Prims algorithm
//------------------------------------------------------------------------
template <class graph_type>
class Graph_MinSpanningTree
{
private:

  //create a typedef for the edge type used by the graph
  typedef typename graph_type::EdgeType Edge;

  const graph_type&              m_Graph;

  std::vector<double>            m_CostToThisNode; 

  std::vector<const Edge*>  m_SpanningTree;
  std::vector<const Edge*>  m_Fringe;

  void Search(const int source)
  {
    if (source < 0 || (size_t)source >= m_Fringe.size()) return;
    //create a priority queue
    IndexedPriorityQLow<double> pq(m_CostToThisNode, m_Graph.NumNodes());

    //put the source node on the queue
    pq.insert(source);

    //while the queue is not empty
    while(!pq.empty())
    {
      //get lowest cost edge from the queue
      int best = pq.Pop();

      //move this edge from the fringe to the spanning tree
      m_SpanningTree[best] = m_Fringe[best];

      //now to test the edges attached to this node
      graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, best);

      for (const Edge* pE=ConstEdgeItr.begin(); !ConstEdgeItr.end(); pE=ConstEdgeItr.next())
      {
        if (!pE || pE->To() < 0 || (size_t)pE->To() >= m_Fringe.size()) continue;
        double Priority = pE->Cost();

        if (m_Fringe[pE->To()] == 0)
        {
          m_CostToThisNode[pE->To()] = Priority;

          pq.insert(pE->To());

          m_Fringe[pE->To()] = pE;
        }

        else if ((Priority<m_CostToThisNode[pE->To()]) && (m_SpanningTree[pE->To()]==0))
        {
          m_CostToThisNode[pE->To()] = Priority;

          pq.ChangePriority(pE->To());

          m_Fringe[pE->To()] = pE;
        }
      }
    }
  }

public:

  Graph_MinSpanningTree(graph_type &G,
                   int   source = -1):m_Graph(G),
                                   m_SpanningTree(G.NumNodes()),                              
                                   m_Fringe(G.NumNodes()),
                                   m_CostToThisNode(G.NumNodes(), -1)
  {                                                                             
    if (source < 0)
    {
      for (int nd=0; nd<G.NumNodes(); ++nd)
      {
        if (m_SpanningTree[nd] == 0)
        {
          Search(nd);
        }
      }
    }

    else
    {
      Search(source);   
    }
  }

  std::vector<const Edge*> GetSpanningTree()const{return m_SpanningTree;}
  
};



#endif