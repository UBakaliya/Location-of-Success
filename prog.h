/**
 * @file prog.cpp
 * @author Uvaish Bakaliya
 * @date 2022-12-13
 * @copyright Copyright (c)UB 2022
 */
#pragma once
#include "dist.h"
#include "graph.h"
#include "osm.h"
#include "tinyxml2.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip> /*setprecision*/
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
using namespace std;
using namespace tinyxml2;

// Use for making 'priority_queue'
struct prioritize
{
    bool operator()(
        const pair<long long, double> &p1, const pair<long long, double> &p2) const
    {
        return p1.second > p2.second;
    }
};

// Finding a shortest path using 'Dijkstra' algorithm
map<long long, double> dijkstraAlgo(
    graph<long long, double> &G, long long startV, map<long long, long long> &pred)
{
    const double INF = numeric_limits<double>::max(); // max number (infinity)
    // to store distance
    map<long long, double> dist;
    // to make the visited vertices
    vector<long long> visited;
    // min heap to store the distance lowest to max
    priority_queue<pair<long long, double>,
                   vector<pair<long long, double>>, prioritize>
        unvisitedPQ;
    // get all the vertices from our graph
    vector<long long> vertices = G.getVertices();
    // mark all the vertices inside of dist map, pred map, and unvisitedPQ to
    // infinity, with the vertex as a key and infinity(max number) as an value
    for (long long &vertex : vertices)
    {
        unvisitedPQ.push({vertex, INF});
        dist[vertex] = INF;
        pred[vertex] = 0;
    }
    // make the very first one:
    unvisitedPQ.push({startV, 0});
    dist[startV] = 0;
    // get the shortest path:
    while (!unvisitedPQ.empty())
    {
        long long topV = unvisitedPQ.top().first;
        double topW = unvisitedPQ.top().second;

        unvisitedPQ.pop();
        // reached end then:
        if (topW == INF)
            break;
        // already visited then
        else if (count(visited.begin(), visited.end(), topV) > 0)
            continue;
        else // unvisited then mark it as visited and find a path
            visited.push_back(topV);

        // get all the neighbors of an topV
        set<long long> neighbors = G.neighbors(topV);
        // find the path:
        for (auto neigh : neighbors)
        {
            double edgeWeight;
            G.getWeight(topV, neigh, edgeWeight);
            double altPathDistance = topW + edgeWeight;
            // if less then make at as next shorted distance vertex
            if (altPathDistance < dist[neigh])
            {
                dist[neigh] = altPathDistance;
                edgeWeight = altPathDistance;
                pred[neigh] = topV;
                unvisitedPQ.push({neigh, altPathDistance});
            }
        }
    }
    return dist;
}

// Trace the path and get the vector of all the vertices
vector<long long> tracePath(
    map<long long, long long> &pred, long long start, long long dest)
{
    stack<long long> revOrder; // for get the path in reverse order
    vector<long long> path;

    // Return empty stack if
    if (start == dest)
        return path;
    revOrder.push(dest);
    long long end = pred[dest];
    // get all the vertices till the declination vertex
    while (end != 0)
    {
        revOrder.push(end);
        end = pred[end];
    }
    // inset the path into vector
    while (!revOrder.empty())
    {
        path.push_back(revOrder.top());
        revOrder.pop();
    }
    return path;
}

// Display the shortest distance path
bool distToDest(long long src, long long dest, graph<long long, double> G, char personNum,
                map<long long, double> &dist, map<long long, long long> pred)
{
    vector<long long> shortestPath = tracePath(pred, src, dest); // get the shorted path
    // if declinations building is same as start then:
    if (shortestPath.size() == 0)
    {
        cout << "\nPerson " << personNum << "'s Distance to dest: " << dist[dest] << " miles"
             << "\nPath: " << dest << endl;
        return true;
    }
    // if there is not path then:
    if (shortestPath[0] == dest)
    {
        cout << "\nSorry, destination unreachable" << endl;
        return false;
    }
    // if there is a path then display it:
    cout << "\nPerson " << personNum << "'s Distance to dest: " << dist[dest] << " miles"
         << "\nPath: ";
    // Print all nodes from start to dest
    for (unsigned i = 0; i < shortestPath.size() - 1; i++)
        cout << shortestPath[i] << "->";
    cout << dest << endl;
    return true;
}

// Find the building inside of Buildings vector and return the index where it found
int searchPersonsBuilding(
    const vector<BuildingInfo> &Buildings, string personBuilding)
{
    // loop thought the Buildings vector and find it
    for (unsigned i = 0; i < Buildings.size(); i++)
    {
        if (Buildings[i].Abbrev == personBuilding)
            return i;
        else if (Buildings[i].Fullname == personBuilding)
            return i;
    }
    // if the input wasn't abbreviation then split the each build full names
    // and check if it is the building person wants entered
    for (unsigned i = 0; i < Buildings.size(); i++)
    {
        if (Buildings[i].Fullname.find(personBuilding) != string::npos)
            return i;
    }
    return -1;
}

// Find the Destination building
int searchDestBuild(
    const Coordinates &center, const vector<BuildingInfo> &Buildings)
{
    // get the first lowest and find the distance
    double prevDist = distBetween2Points(
        center.Lat, center.Lon, Buildings[0].Coords.Lat, Buildings[0].Coords.Lon);
    int lowestDistIdx = 0; // to store the index of the lowest distance found

    for (unsigned i = 1; i < Buildings.size(); i++)
    {
        // find the distance
        double currDist = distBetween2Points(
            center.Lat, center.Lon, Buildings[i].Coords.Lat, Buildings[i].Coords.Lon);
        // if the currant distance is less the the previous then make it the new lowest distance
        if (currDist < prevDist)
        {
            prevDist = currDist;
            lowestDistIdx = i;
        }
    }
    cout << "Destination Building:\n " << Buildings[lowestDistIdx].Fullname << "\n ("
         << Buildings[lowestDistIdx].Coords.Lat << ", " << Buildings[lowestDistIdx].Coords.Lon
         << ')' << endl;
    return lowestDistIdx;
}

// Search the footways for the nearest start and dest nodes
Coordinates searchNearsFootways(
    const vector<FootwayInfo> &Footways, const map<long long, Coordinates> &Nodes,
    int idx, const vector<BuildingInfo> &Buildings)
{
    Coordinates nearestBuilding;
    // get the very first node
    auto node = Nodes.find(Footways[0].Nodes[0]);
    // and get the distance
    double prevDist = distBetween2Points(
        node->second.Lat, node->second.Lon, Buildings[idx].Coords.Lat, Buildings[idx].Coords.Lon);

    // find the footways and get the footways lat and lon by there id
    for (const auto &i : Footways)
    {
        for (unsigned j = 0; j < i.Nodes.size(); j++)
        {
            node = Nodes.find(i.Nodes[j]);
            double currDist = distBetween2Points(
                node->second.Lat, node->second.Lon, Buildings[idx].Coords.Lat, Buildings[idx].Coords.Lon);
            // if smallest one is found then make the the very smallest so far
            if (currDist < prevDist)
            {
                prevDist = currDist;
                nearestBuilding.ID = i.Nodes[j];
                nearestBuilding.Lat = node->second.Lat;
                nearestBuilding.Lon = node->second.Lon;
            }
        }
    }
    return nearestBuilding; // return the id and there Coordinates
}

// Add edges to graphs by looping thought the footways and computing distance
void addEdges(
    vector<FootwayInfo> Footways, graph<long long, double> &G,
    const map<long long, Coordinates> &Nodes)
{
    // loop through the footways vector and find the distance between to pointers
    for (const auto &it : Footways)
    {
        for (size_t i = 0; i < it.Nodes.size() - 1; i++)
        {
            auto n1 = Nodes.find(it.Nodes[i]);     // get the current node
            auto n2 = Nodes.find(it.Nodes[i + 1]); // get the next node
            // find the distance
            double foundDist = distBetween2Points(
                n1->second.Lat, n1->second.Lon, n2->second.Lat, n2->second.Lon);
            G.addEdge(n1->first, n2->first, foundDist); // add the edge
            G.addEdge(n2->first, n1->first, foundDist);
        }
    }
}

// Implement your standard application here
void application(
    map<long long, Coordinates> &Nodes, vector<FootwayInfo> &Footways,
    vector<BuildingInfo> &Buildings, graph<long long, double> &G)
{
    string person1Building, person2Building;
    cout << "\nEnter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);

    while (person1Building != "#")
    {
        int p1BuildIdx = searchPersonsBuilding(Buildings, person1Building);

        cout << "Enter person 2's building (partial name or abbreviation)> ";
        getline(cin, person2Building);
        // search for the person 2 building
        int p2BuildIdx = searchPersonsBuilding(Buildings, person2Building);

        if (p1BuildIdx == -1)
            cout << "Person 1's building not found" << endl;
        else if (p2BuildIdx == -1)
            cout << "Person 2's building not found" << endl;
        else
        {
            cout << "\nPerson 1's point:\n"
                 << Buildings[p1BuildIdx].Fullname << "\n (" << Buildings[p1BuildIdx].Coords.Lat << ", "
                 << Buildings[p1BuildIdx].Coords.Lon << ")\n"
                 << "Person 2's point:\n"
                 << Buildings[p2BuildIdx].Fullname << "\n (" << Buildings[p2BuildIdx].Coords.Lat << ", "
                 << Buildings[p2BuildIdx].Coords.Lon << ')'
                 << endl;
            // find the center from a person1 building lat and lon & also from a person2 building as well
            Coordinates center = centerBetween2Points(
                Buildings[p1BuildIdx].Coords.Lat, Buildings[p1BuildIdx].Coords.Lon,
                Buildings[p2BuildIdx].Coords.Lat, Buildings[p2BuildIdx].Coords.Lon);

            int destBuild = searchDestBuild(center, Buildings); // find the destination

            // Search the footways for the nearest for person 1, 2, and for destination building
            Coordinates nearestP1 = searchNearsFootways(Footways, Nodes, p1BuildIdx, Buildings);
            Coordinates nearestP2 = searchNearsFootways(Footways, Nodes, p2BuildIdx, Buildings);
            Coordinates nearestDest = searchNearsFootways(Footways, Nodes, destBuild, Buildings);

            // display the nearest buildings with there there id and ther coordinates
            cout << "\nNearest P1 node:\n"
                 << nearestP1.ID << "\n (" << nearestP1.Lat
                 << ", " << nearestP1.Lon << ')' << "\nNearest P2 node:\n"
                 << nearestP2.ID << "\n (" << nearestP2.Lat << ", " << nearestP2.Lon << ")\n"
                 << "Nearest destination node: \n " << nearestDest.ID << "\n ("
                 << nearestDest.Lat << ", " << nearestDest.Lon << ')' << endl;
            map<long long, long long> pred1, pred2;
            map<long long, double> dist1 = dijkstraAlgo(G, nearestP1.ID, pred1); // find the path
            map<long long, double> dist2 = dijkstraAlgo(G, nearestP2.ID, pred2); // find the path
            // find the shorted path and display it
            if (distToDest(nearestP1.ID, nearestDest.ID, G, '1', dist1, pred1))
                distToDest(nearestP2.ID, nearestDest.ID, G, '2', dist2, pred2);
        }
        cout << "\nEnter person 1's building (partial name or abbreviation), or #> ";
        getline(cin, person1Building);
    }
}
