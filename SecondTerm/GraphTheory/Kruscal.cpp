#include <algorithm>
#include <iostream>


/* Задание 3A2. Минимальное остовное дерево 2018
 * Дан неориентированный связный граф.
 * Требуется найти вес минимального остовного дерева в этом графе с помощью алгоритма Крускала.
 */


class DSU{
 public:

  DSU(int number_of_vertices){
    parent = std::vector<int> (number_of_vertices);
    rank = std::vector<int> (number_of_vertices);
  }
  void make_set(int vertex){
    parent[vertex] = vertex;
    rank[vertex] = 0;
  }

  int find_set(int vertex){
    if (vertex == parent[vertex]){
      return vertex;
    } else {
      return parent[vertex] = find_set((parent[vertex]));
    }
  }

  void union_sets(int vertex_1, int vertex_2){
    vertex_1 = find_set(vertex_1);
    vertex_2 = find_set(vertex_2);
    if (vertex_1 != vertex_2) {
      if (rank[vertex_1] < rank[vertex_2]) {
        std::swap(vertex_1, vertex_2);
      }
      parent[vertex_2] = vertex_1;
      if (rank[vertex_1] == rank[vertex_2]) {
        ++rank[vertex_1];
      }
    }
  }

 private:
  std::vector<int> parent;
  std::vector<int> rank;
};

int Kruscal(int number_of_vertices, std::vector<std::pair<int, std::pair<int, int> > > &graph){

  int cost = 0;
  sort (graph.begin(), graph.end());
  DSU dsu(number_of_vertices);
  for (int i = 0; i < number_of_vertices; ++i){
    dsu.make_set(i);
  }

  for (int i=0; i < graph.size(); ++i) {
    int vertex_1 = graph[i].second.first,  vertex_2 = graph[i].second.second,  weight = graph[i].first;
    if (dsu.find_set(vertex_1) != dsu.find_set(vertex_2)) {
      cost += weight;
      dsu.union_sets(vertex_1, vertex_2);
    }
  }

  return  cost;
}

int main(){
  int number_of_vertices = 0;
  int number_of_edges = 0;
  std::cin>>number_of_vertices;
  std::cin>>number_of_edges;
  std::vector < std::pair < int, std::pair<int,int> > > graph;
  for (int i = 0; i < number_of_edges; ++i){
    int from = 0;
    int to = 0;
    int wt = 0;
    std::cin>>from;
    std::cin>>to;
    std::cin>>wt;
    graph.push_back({wt, {from - 1, to - 1}});
    graph.push_back({wt, {to - 1, from - 1}});
  }
  std::cout<< Kruscal(number_of_vertices, graph);
  return 0;
}

