#ifndef SPARSEGRAPH_H
#define SPARSEGRAPH_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SparseGraph.h
//
//  Desc:   Graph class using the adjacency list representation.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <list>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <climits>
#include <stdexcept>


//#include "Vector2D.h"
//#include "utils.h" 
#include "NodeTypeEnumerations.h"



template <class node_type, class edge_type>   
class SparseGraph                                 
{
public:

	//enable easy client access to the edge and node types used in the graph
	typedef edge_type                EdgeType;
	typedef node_type                NodeType;

	//a couple more typedefs to save my fingers and to help with the formatting
	//of the code on the printed page
	typedef std::vector<node_type>   NodeVector;
	typedef std::list<edge_type>     EdgeList;
	typedef std::vector<EdgeList>    EdgeListVector;


private:

	//the nodes that comprise this graph
	NodeVector      m_Nodes;

	//a vector of adjacency edge lists. (each node index keys into the 
	//list of edges associated with that node)
	EdgeListVector  m_Edges;

	//is this a directed graph?
	bool            m_bDigraph;

	//the index of the next node to be added
	int             m_iNextNodeIndex;


	//returns true if an edge is not already present in the graph. Used
	//when adding edges to make sure no duplicates are created.
	bool  UniqueEdge(int from, int to)const;

	//iterates through all the edges in the graph and removes any that point
	//to an invalidated node
	void  CullInvalidEdges();

public:

	//ctor
	SparseGraph(bool digraph): m_iNextNodeIndex(0), m_bDigraph(digraph){}

	//returns the node at the given index
	const NodeType&  GetNode(int idx)const;

	//non const version
	NodeType&  GetNode(int idx);

	//const method for obtaining a reference to an edge
	const EdgeType& GetEdge(int from, int to)const;

	//non const version
	EdgeType& GetEdge(int from, int to);


	//retrieves the next free node index
	int   GetNextFreeNodeIndex()const{return m_iNextNodeIndex;}

	//adds a node to the graph and returns its index
	int   AddNode(node_type node);

	//removes a node by setting its index to invalid_node_index
	void  RemoveNode(int node);

	//Use this to add an edge to the graph. The method will ensure that the
	//edge passed as a parameter is valid before adding it to the graph. If the
	//graph is a digraph then a similar edge connecting the nodes in the opposite
	//direction will be automatically added.
	void  AddEdge(EdgeType edge);

	//removes the edge connecting from and to from the graph (if present). If
	//a digraph then the edge connecting the nodes in the opposite direction 
	//will also be removed.
	void  RemoveEdge(int from, int to);

	//sets the cost of an edge
	void  SetEdgeCost(int from, int to, double cost);

	//returns the number of active + inactive nodes present in the graph
	int   NumNodes()const{return m_Nodes.size();}

	//returns the number of active nodes present in the graph (this method's
	//performance can be improved greatly by caching the value)
	int   NumActiveNodes()const
	{
		int count = 0;

		for (unsigned int n=0; n<m_Nodes.size(); ++n) if (m_Nodes[n].Index() != invalid_node_index) ++count;

		return count;
	}

	//returns the total number of edges present in the graph
	int   NumEdges()const
	{
		int tot = 0;

		for (EdgeListVector::const_iterator curEdge = m_Edges.begin();
			curEdge != m_Edges.end();
			++curEdge)
		{
			tot += curEdge->size();
		}

		return tot;
	}

	//returns true if the graph is directed
	bool  isDigraph()const{return m_bDigraph;}

	//returns true if the graph contains no nodes
	bool	isEmpty()const{return m_Nodes.empty();}

	//returns true if a node with the given index is present in the graph
	bool isNodePresent(int nd)const;

	//returns true if an edge connecting the nodes 'to' and 'from'
	//is present in the graph
	bool isEdgePresent(int from, int to)const;

	//methods for loading and saving graphs from an open file stream or from
	//a file name 
	bool  Save(const char* FileName)const;
	bool  Save(std::ofstream& stream)const;

	bool  Load(const char* FileName);
	bool  Load(std::ifstream& stream);

	//clears the graph ready for new node insertions
	void Clear(){m_iNextNodeIndex = 0; m_Nodes.clear(); m_Edges.clear();}

	void RemoveEdges()
	{
		for (EdgeListVector::iterator it = m_Edges.begin(); it != m_Edges.end(); ++it)
		{
			it->clear();
		}
	}


	//non const class used to iterate through all the edges connected to a specific node. 
	class EdgeIterator
	{
	private:                                                                

		typename EdgeList::iterator         curEdge;

		SparseGraph<node_type, edge_type>&  G;

		const int                           NodeIndex;
		bool ValidNode;

	public:

		EdgeIterator(SparseGraph<node_type, edge_type>& graph,
			int                                node): G(graph),
			NodeIndex(node), ValidNode(node >= 0 && (size_t)node < graph.m_Edges.size())
		{
			/* we don't need to check for an invalid node index since if the node is
			invalid there will be no associated edges
			*/

			if (!ValidNode || (size_t)NodeIndex >= G.m_Edges.size()) return;
			curEdge = G.m_Edges[NodeIndex].begin();
		}

		EdgeType*  begin()
		{ 
			if (!ValidNode || (size_t)NodeIndex >= G.m_Edges.size()) return NULL;
			curEdge = G.m_Edges[NodeIndex].begin();

			return end() ? NULL : &(*curEdge);
		}

		EdgeType*  next()
		{
			if(end())
			{
				return 0;
			}
			else
			{
				++curEdge;

				if(!end())
				{
					return &(*curEdge);
				}

				return 0;
			}
		}

		//return true if we are at the end of the edge list
		bool end()
		{
			return !ValidNode || (size_t)NodeIndex >= G.m_Edges.size() ||
				(curEdge == G.m_Edges[NodeIndex].end());
		}
	};

	friend class EdgeIterator;

	//const class used to iterate through all the edges connected to a specific node. 
	class ConstEdgeIterator
	{
	private:                                                                

		typename EdgeList::const_iterator        curEdge;

		const SparseGraph<node_type, edge_type>& G;

		const int                                NodeIndex;
		bool ValidNode;

	public:

		ConstEdgeIterator(const SparseGraph<node_type, edge_type>& graph,
			int                           node): G(graph),
			NodeIndex(node), ValidNode(node >= 0 && (size_t)node < graph.m_Edges.size())
		{
			/* we don't need to check for an invalid node index since if the node is
			invalid there will be no associated edges
			*/

			if (!ValidNode || (size_t)NodeIndex >= G.m_Edges.size()) return;
			curEdge = G.m_Edges[NodeIndex].begin();
		}

		const EdgeType*  begin()
		{        
			if (!ValidNode || (size_t)NodeIndex >= G.m_Edges.size()) return NULL;
			curEdge = G.m_Edges[NodeIndex].begin();

			// return &(*curEdge);	// 0 sized begin() == end();// zhaojun@snda
			return (end() ? NULL : (&(*curEdge)));	// zhaojun@snda
		}

		const EdgeType*  next()
		{
			if(end())
			{
				return 0;
			}
			else
			{
				++curEdge;

				if(!end())
				{
					return &(*curEdge);
				}

				return 0;
			}
		}

		//return true if we are at the end of the edge list
		bool end()
		{
			return !ValidNode || (size_t)NodeIndex >= G.m_Edges.size() ||
				(curEdge == G.m_Edges[NodeIndex].end());
		}
	};

	friend class ConstEdgeIterator;

	//non const class used to iterate through the nodes in the graph
	class NodeIterator
	{
	private:

		typename NodeVector::iterator         curNode;

		SparseGraph<node_type, edge_type>&    G;

		//if a graph node is removed, it is not removed from the 
		//vector of nodes (because that would mean changing all the indices of 
		//all the nodes that have a higher index). This method takes a node
		//iterator as a parameter and assigns the next valid element to it.
		void GetNextValidNode(typename NodeVector::iterator& it)
		{
			while (it != G.m_Nodes.end() && it->Index() == invalid_node_index) ++it;
		}

	public:

		NodeIterator(SparseGraph<node_type, edge_type> &graph):G(graph)
		{
			curNode = G.m_Nodes.begin();
		}


		node_type* begin()
		{      
			curNode = G.m_Nodes.begin();

			GetNextValidNode(curNode);
			
			if(end())
			{
				return 0;
			}

			return &(*curNode);
		}

		node_type* next()
		{
			if (end()) return NULL;
			++curNode;

			GetNextValidNode(curNode);

			if(end())
			{
				return 0;
			}

			return &(*curNode);
		}

		bool end()
		{
			return (curNode == G.m_Nodes.end());
		}
	};


	friend class NodeIterator;

	//const class used to iterate through the nodes in the graph
	class ConstNodeIterator
	{
	private:

		typename NodeVector::const_iterator			curNode;

		const SparseGraph<node_type, edge_type>&      G;

		//if a graph node is removed or switched off, it is not removed from the 
		//vector of nodes (because that would mean changing all the indices of 
		//all the nodes that have a higher index. This method takes a node
		//iterator as a parameter and assigns the next valid element to it.
		void GetNextValidNode(typename NodeVector::const_iterator& it)
		{
			while (it != G.m_Nodes.end() && it->Index() == invalid_node_index) ++it;
		}

	public:

		ConstNodeIterator(const SparseGraph<node_type, edge_type> &graph):G(graph)
		{
			curNode = G.m_Nodes.begin();
		}


		const node_type* begin()
		{      
			curNode = G.m_Nodes.begin();

			GetNextValidNode(curNode);

			if(end())
			{
				return 0;
			}

			return &(*curNode);
		}

		const node_type* next()
		{
			if (end()) return NULL;
			++curNode;

			GetNextValidNode(curNode);
			
			if(end())
			{
				return 0;
			}

			return &(*curNode);
		}

		bool end()
		{
			return (curNode == G.m_Nodes.end());
		}
	};

	friend class ConstNodeIterator;
};


//--------------------------- isNodePresent --------------------------------
//
//  returns true if a node with the given index is present in the graph
//--------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::isNodePresent(int nd)const
{
	return nd >= 0 && (size_t)nd < m_Nodes.size() && m_Nodes[nd].Index() != invalid_node_index;
}

//--------------------------- isEdgePresent --------------------------------
//
//  returns true if an edge with the given from/to is present in the graph
//--------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::isEdgePresent(int from, int to)const
{
	if (isNodePresent(from) && isNodePresent(to))
	{
		for (EdgeList::const_iterator curEdge = m_Edges[from].begin();
			curEdge != m_Edges[from].end();
			++curEdge)
		{
			if (curEdge->To() == to) return true;
		}

		return false;
	}
	else return false;
}
//------------------------------ GetNode -------------------------------------
//
//  const and non const methods for obtaining a reference to a specific node
//----------------------------------------------------------------------------
template <class node_type, class edge_type>
const node_type&  SparseGraph<node_type, edge_type>::GetNode(int idx)const
{
	if (idx < 0 || (size_t)idx >= m_Nodes.size())
		throw std::out_of_range("SparseGraph::GetNode: invalid index");
	return m_Nodes[idx];
}

//non const version
template <class node_type, class edge_type>
node_type&  SparseGraph<node_type, edge_type>::GetNode(int idx)
{
	if (idx < 0 || (size_t)idx >= m_Nodes.size())
		throw std::out_of_range("SparseGraph::GetNode: invalid index");
	return m_Nodes[idx];
}

//------------------------------ GetEdge -------------------------------------
//
//  const and non const methods for obtaining a reference to a specific edge
//----------------------------------------------------------------------------
template <class node_type, class edge_type>
const edge_type& SparseGraph<node_type, edge_type>::GetEdge(int from, int to)const
{
	if (!isNodePresent(from) || !isNodePresent(to))
		throw std::out_of_range("SparseGraph::GetEdge: invalid index");
	for (typename EdgeList::const_iterator edge = m_Edges[from].begin(); edge != m_Edges[from].end(); ++edge)
		if (edge->To() == to) return *edge;
	throw std::runtime_error("SparseGraph::GetEdge: edge does not exist");
}

//non const version
template <class node_type, class edge_type>
edge_type& SparseGraph<node_type, edge_type>::GetEdge(int from, int to)
{
	if (!isNodePresent(from) || !isNodePresent(to))
		throw std::out_of_range("SparseGraph::GetEdge: invalid index");
	for (typename EdgeList::iterator edge = m_Edges[from].begin(); edge != m_Edges[from].end(); ++edge)
		if (edge->To() == to) return *edge;
	throw std::runtime_error("SparseGraph::GetEdge: edge does not exist");
}

//-------------------------- AddEdge ------------------------------------------
//
//  Use this to add an edge to the graph. The method will ensure that the
//  edge passed as a parameter is valid before adding it to the graph. If the
//  graph is a digraph then a similar edge connecting the nodes in the opposite
//  direction will be automatically added.
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::AddEdge(EdgeType edge)
{
	//first make sure the from and to nodes exist within the graph 
	if (!isNodePresent(edge.From()) || !isNodePresent(edge.To())) return;

	//make sure both nodes are active before adding the edge
	if ( (m_Nodes[edge.To()].Index() != invalid_node_index) && 
		(m_Nodes[edge.From()].Index() != invalid_node_index))
	{
		//add the edge, first making sure it is unique
		if (UniqueEdge(edge.From(), edge.To()))
		{
			m_Edges[edge.From()].push_back(edge);
		}

		//if the graph is undirected we must add another connection in the opposite
		//direction
		if (!m_bDigraph)
		{
			//check to make sure the edge is unique before adding
			if (UniqueEdge(edge.To(), edge.From()))
			{
				EdgeType NewEdge = edge;

				NewEdge.SetTo(edge.From());
				NewEdge.SetFrom(edge.To());

				m_Edges[edge.To()].push_back(NewEdge);
			}
		}
	}
}


//----------------------------- RemoveEdge ---------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::RemoveEdge(int from, int to)
{
	if (from < 0 || to < 0 || (size_t)from >= m_Edges.size() || (size_t)to >= m_Edges.size()) return;

	EdgeList::iterator curEdge;

	if (!m_bDigraph)
	{
		for (curEdge = m_Edges[to].begin();
			curEdge != m_Edges[to].end();
			++curEdge)
		{
			if (curEdge->To() == from){curEdge = m_Edges[to].erase(curEdge);break;}
		}
	}

	for (curEdge = m_Edges[from].begin();
		curEdge != m_Edges[from].end();
		++curEdge)
	{
		if (curEdge->To() == to){curEdge = m_Edges[from].erase(curEdge);break;}
	}
}

//-------------------------- AddNode -------------------------------------
//
//  Given a node this method first checks to see if the node has been added
//  previously but is now innactive. If it is, it is reactivated.
//
//  If the node has not been added previously, it is checked to make sure its
//  index matches the next node index before being added to the graph
//------------------------------------------------------------------------
template <class node_type, class edge_type>
int SparseGraph<node_type, edge_type>::AddNode(node_type node)
{
	const int index = node.Index();
	if (index < 0) return invalid_node_index;
	if ((size_t)index < m_Nodes.size())
	{
		if (isNodePresent(index)) return invalid_node_index;
		m_Nodes[index] = node;
		return index;
	}
	if (index != m_iNextNodeIndex || m_iNextNodeIndex == INT_MAX) return invalid_node_index;
	m_Nodes.push_back(node);
	m_Edges.push_back(EdgeList());
	return m_iNextNodeIndex++;
}

//----------------------- CullInvalidEdges ------------------------------------
//
//  iterates through all the edges in the graph and removes any that point
//  to an invalidated node
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::CullInvalidEdges()
{
	for (typename EdgeListVector::iterator list = m_Edges.begin(); list != m_Edges.end(); ++list)
	{
		for (typename EdgeList::iterator edge = list->begin(); edge != list->end();)
		{
			if (!isNodePresent(edge->To()) || !isNodePresent(edge->From()))
				edge = list->erase(edge);
			else
				++edge;
		}
	}
}


//------------------------------- RemoveNode -----------------------------
//
//  Removes a node from the graph and removes any links to neighbouring
//  nodes
//------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::RemoveNode(int node)                                   
{
	if (!isNodePresent(node)) return;
	m_Nodes[node].SetIndex(invalid_node_index);
	m_Edges[node].clear();
	CullInvalidEdges();
}

//-------------------------- SetEdgeCost ---------------------------------
//
//  Sets the cost of a specific edge
//------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::SetEdgeCost(int from, int to, double NewCost)
{
	//make sure the nodes given are valid
	if (!isNodePresent(from) || !isNodePresent(to)) return;

	//visit each neighbour and erase any edges leading to this node
	for (EdgeList::iterator curEdge = m_Edges[from].begin(); 
		curEdge != m_Edges[from].end();
		++curEdge)
	{
		if (curEdge->To() == to)
		{
			curEdge->SetCost(NewCost);
			break;
		}
	}
}

//-------------------------------- UniqueEdge ----------------------------
//
//  returns true if the edge is not present in the graph. Used when adding
//  edges to prevent duplication
//------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::UniqueEdge(int from, int to)const
{
	if (!isNodePresent(from) || !isNodePresent(to)) return false;
	for (EdgeList::const_iterator curEdge = m_Edges[from].begin();
		curEdge != m_Edges[from].end();
		++curEdge)
	{
		if (curEdge->To() == to)
		{
			return false;
		}
	}

	return true;
}

//-------------------------------- Save ---------------------------------------

template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::Save(const char* FileName)const
{
	if (!FileName) return false;
	//open the file and make sure it's valid
	std::ofstream out(FileName);

	if (!out)
	{
		throw std::runtime_error("Cannot open file: " + std::string(FileName));
		return false;
	}

	return Save(out);
}

//-------------------------------- Save ---------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::Save(std::ofstream& stream)const
{
	//save the number of nodes
	stream << m_Nodes.size() << std::endl;

	//iterate through the graph nodes and save them
	NodeVector::const_iterator curNode = m_Nodes.begin();
	for (curNode; curNode!=m_Nodes.end(); ++curNode)
	{
		stream << *curNode;
	}

	//save the number of edges
	stream << NumEdges() << std::endl;


	//iterate through the edges and save them
	for (unsigned int nodeIdx = 0; nodeIdx < m_Nodes.size(); ++nodeIdx)
	{
		for (EdgeList::const_iterator curEdge = m_Edges[nodeIdx].begin();
			curEdge!=m_Edges[nodeIdx].end(); ++curEdge)
		{
			stream << *curEdge;
		}  
	}

	return true;
}

//------------------------------- Load ----------------------------------------
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::Load(const char* FileName)
{
	if (!FileName) return false;
	//open file and make sure it's valid
	std::ifstream in(FileName);

	if (!in)
	{
		throw std::runtime_error("Cannot open file: " + std::string(FileName));
		return false;
	}

	return Load(in);
}

//------------------------------- Load ----------------------------------------
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::Load(std::ifstream& stream)
{
	int nodeCount = 0, edgeCount = 0;
	if (!(stream >> nodeCount) || nodeCount < 0) return false;
	SparseGraph staged(m_bDigraph);
	for (int index = 0; index < nodeCount; ++index)
	{
		NodeType node(stream);
		if (!stream || (node.Index() != index && node.Index() != invalid_node_index)) return false;
		staged.m_Nodes.push_back(node);
		staged.m_Edges.push_back(EdgeList());
		++staged.m_iNextNodeIndex;
	}
	if (!(stream >> edgeCount) || edgeCount < 0) return false;
	for (int index = 0; index < edgeCount; ++index)
	{
		EdgeType edge(stream);
		if (!stream || !staged.isNodePresent(edge.From()) || !staged.isNodePresent(edge.To())) return false;
		staged.m_Edges[edge.From()].push_back(edge);
	}
	m_Nodes.swap(staged.m_Nodes);
	m_Edges.swap(staged.m_Edges);
	m_iNextNodeIndex = staged.m_iNextNodeIndex;
	return true;
}


#endif