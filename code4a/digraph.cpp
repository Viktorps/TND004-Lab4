/*********************************************
 * file:	~\code4a\digraph.cpp              *
 * remark: implementation of directed graphs  *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <limits>  //std::numeric_limits
#include <vector>
#include <cassert>
#include <queue>
#include <format>
#include <iostream>
#include <iomanip>

#include "digraph.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

Digraph::Digraph(int n)
    : table(n + 1)  // slot zero not used
    , size{n}       // number of verices
    , n_edges{0}
    , dist(n + 1)
    , path(n + 1)
    , done(n + 1) {
    assert(n >= 1);
    // Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero
}

// Create a digraph with n vertices and the edges in V
Digraph::Digraph(const std::vector<Edge>& V, int n) : Digraph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert directed edge e = (u, v, w)
// update weight w if edge (u, v) is present
void Digraph::insertEdge(const Edge& e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    // Check if edge e already exists
    if (auto it = std::find_if(begin(table[e.from]), end(table[e.from]),
                               [e](const Edge& ed) { return e.links_same_nodes(ed); });
        it == end(table[e.from])) {
        table[e.from].push_back(e);  // insert new edge e
        ++n_edges;
    } else {
        it->weight = e.weight;  // update the weight
    }
}

// remove directed edge e
void Digraph::removeEdge(const Edge& e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto it = std::find_if(begin(table[e.from]), end(table[e.from]),
                           [e](const Edge& ed) { return e.links_same_nodes(ed); });

    assert(it != end(table[e.from]));
    table[e.from].erase(it);
    --n_edges;
}

// construct unweighted single source shortest path-tree for start vertex s
void Digraph::uwsssp(int s) const {
    assert(s >= 1 && s <= size);
    
    std::fill(dist.begin(), dist.end(), std::numeric_limits<int>::max());
    std::fill(path.begin(), path.end(), 0);
    std::queue<int> Q;
    
    dist[s] = 0;
    Q.push(s);
    
    while(!Q.empty()){
        int v = Q.front();
        Q.pop();
        
        for (const auto& edge : table[v]) {
            int u = edge.to;
            if (dist[u] == std::numeric_limits<int>::max()) {  // u has not been visited
                dist[u] = dist[v] + 1;
                path[u] = v;
                Q.push(u);
            }
        }
    }
}
int Digraph::find_smallest_undone_distance_vertex() const {
    int min_distance = std::numeric_limits<int>::max();
    int vertex_index = 0;

    for (int i = 1; i <= size; i++) {
        if (!done[i] && dist[i] < min_distance) {
            min_distance = dist[i];
            vertex_index = i;
        }
    }
    return vertex_index;
}

// construct positive weighted single source shortest path-tree for start vertex s
// Dijktra’s algorithm
void Digraph::pwsssp(int s) const {
    assert(s >= 1 && s <= size);

    std::fill(dist.begin(), dist.end(), std::numeric_limits<int>::max());
    std::fill(path.begin(), path.end(), 0);
    std::fill(done.begin(), done.end(), false);
    
    dist[s] = 0;
    done[s] = true;
    int v = s;
    
    while(true){
        for (const auto& edge : table[v]) {
            int u = edge.to;
            int w = edge.weight;
            
            if(!done[u] && dist[u] > dist[v] + w ){
                dist[u] = dist[v] + w;
                path[u] = v;
            }
        }
        v = find_smallest_undone_distance_vertex();
        if (v == 0) break; // exit the loop
        done[v] = true;
    }
}

// print graph
void Digraph::printGraph() const {
    std::cout << std::setfill(' ') << std::setw(66) << "" << std::endl;
    std::cout << "Vertex  adjacency lists" << std::endl;
    std::cout << std::setfill(' ') << std::setw(66) << "" << std::endl;

    for (int v = 1; v <= size; ++v) {
        std::cout << std::setw(4) << v << " : ";
        for (const auto& e : table[v]) {
            std::cout << "(" << std::setw(2) << e.to << ", " << std::setw(2) << e.weight << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << std::setfill(' ') << std::setw(66) << "" << std::endl;
}

// Print shortest path tree for s
void Digraph::printTree() const {
    std::cout << std::setfill(' ') << std::setw(22) << "" << std::endl;
    std::cout << "vertex    dist    path" << std::endl;
    std::cout << std::setfill(' ') << std::setw(22) << "" << std::endl;

    for (int v = 1; v <= size; ++v) {
        std::cout << std::setw(4) << v << " : "
                  << std::setw(6) << (dist[v] == std::numeric_limits<int>::max() ? -1 : dist[v])
                  << std::setw(6) << path[v] << std::endl;
    }
    std::cout << std::setfill(' ') << std::setw(22) << "" << std::endl;
}

// print shortest path from s to t and the corresponding path length
// Hint: consider using recursion
void Digraph::printPath(int t) const {
    assert(t >= 1 && t <= size);

    if (dist[t] == std::numeric_limits<int>::max()) {
        std::cout << "No path to vertex " << t << std::endl;
        return;
    }

    std::cout << "Shortest path =";
    printPathHelper(t);
    std::cout << "   (" << dist[t] << ")" << std::endl;
}

void Digraph::printPathHelper(int t) const {
    if (path[t] == 0) {  // Assuming -1 indicates no predecessor
        std::cout << " " << t;
    } else {
        printPathHelper(path[t]);  // Recursive call to trace back the path
        std::cout << "   " << t;
    }
}

