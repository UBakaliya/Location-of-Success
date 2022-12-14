/**
 * @file graph.h
 * @author Uvaish Bakaliya
 * @date 2022-12-13
 * @copyright Copyright (c)UB 2022
 */
#pragma once

#include <iostream>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename VertexT, typename WeightT>
class graph
{
private:
  // unordered_map to store the neighbors or a vertex
  typedef unordered_map<VertexT, WeightT> edge;
  // key will be the vertex and value will be the edge Data (struct)
  unordered_map<VertexT, edge> adjList;
  // to keep track of edges being added to graph
  int edgeCounter;

public:
  // Default-constructor
  graph() { this->edgeCounter = 0; }

  // Destructor
  ~graph()
  {
    this->adjList.clear();
    this->edgeCounter = 0;
  }

  // Will return the size of vertices
  int NumVertices() const { return this->adjList.size(); }

  // Will return the size of edges in graph
  int NumEdges() const { return this->edgeCounter; }

  // Add vertex to our graph
  bool addVertex(VertexT v)
  {
    // check if the vertex is inside of our map
    if (this->adjList.find(v) != this->adjList.end())
      return false;
    // if not then add it:
    this->adjList[v] = {};
    return true;
  }

  // Add edge to between our vertices
  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    // check if vertex exits
    if (this->adjList.find(from) == this->adjList.end() ||
        this->adjList.find(to) == this->adjList.end())
      return false;
    // if 'to' vertex is already been as a edge to 'from' then update the weight only
    if (this->adjList[from].find(to) != this->adjList[from].end())
    {
      this->adjList[from][to] = weight;
      return true;
    }
    // else if it new edge then:
    this->adjList[from][to] = weight;
    this->edgeCounter++;
    return true;
  }

  // Return the weight of given from and to vertexes
  bool getWeight(VertexT from, VertexT to, WeightT &weight)
  {
    // check if vertex from and to exits and also check if there is an edge between them
    if (this->adjList.find(from) == this->adjList.end() ||
        this->adjList.find(to) == this->adjList.end() ||
        this->adjList[from].find(to) == this->adjList[from].end())
      return false;

    weight = this->adjList[from][to];
    return true;
  }

  // Return all teh neighbors of a given vertex
  set<VertexT> neighbors(VertexT v)
  {
    set<VertexT> neighbors;
    for (const auto &neighbor : this->adjList[v])
      neighbors.insert(neighbor.first);
    return neighbors;
  }

  // Return the all the vertices
  vector<VertexT> getVertices()
  {
    vector<VertexT> vertices;
    for (const auto &vertex : this->adjList)
      vertices.push_back(vertex.first);
    return vertices;
  }

  // Dumps the internal state of the graph for debugging purposes.
  void dump(ostream &output)
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

    output << endl;
    output << "**Vertices:" << endl;
    vector<VertexT> dumpVertex = getVertices();
    sort(dumpVertex.begin(), dumpVertex.end());
    for (int i = 0; i < dumpVertex.size(); i++)
      output << " " << i << ". " << dumpVertex[i] << endl;
    output << endl;
    output << "**Edges:" << endl;
    for (int row = 0; row < this->NumVertices(); ++row)
    {
      output << " row " << dumpVertex[row] << ": ";
      for (int col = 0; col < this->NumVertices(); col++)
      {
        WeightT weight = 0.0;
        this->getWeight(dumpVertex[row], dumpVertex[col], weight);
        if (weight == 0.0)
          output << "F ";
        else
          output << "(T," << weight << ") ";
      }
      output << endl;
    }
    output << "**************************************************" << endl;
  }
};