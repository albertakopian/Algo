/*
Дан невзвешенный неориентированный граф.
Определить, является ли он двудольным.
Требуемая сложность O(V+E).
Ввод:
v:кол-во вершин(макс. 50000),
n:кол-во ребер(макс. 200000),
n пар реберных вершин.
Вывод:
YES если граф является двудольным, NO - если не является.
*/

#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

using Vertex = size_t;

class Graph {
 public:
  Graph(size_t vertices_count,
        const std::vector<std::pair<Vertex, Vertex>>& edge_list);

  bool IsBipartite();

 private:
  size_t vertices_count_;
  std::vector<std::vector<Vertex>> adjacency_list_;

  enum class Part { Left, Right };

  bool IsBipartiteComponent(Vertex vertex, std::queue<Vertex>& queue,
                            std::vector<bool>& is_visited,
                            std::vector<Part>& part);
};

Graph::Graph(size_t vertices_count,
             const std::vector<std::pair<Vertex, Vertex>>& edge_list) {
  vertices_count_ = vertices_count;
  adjacency_list_.resize(vertices_count_);
  for (auto& edge : edge_list) {
    adjacency_list_[edge.first].push_back(edge.second);
    adjacency_list_[edge.second].push_back(edge.first);
  }
}

bool Graph::IsBipartiteComponent(Vertex vertex, std::queue<Vertex>& queue,
                                 std::vector<bool>& is_visited,
                                 std::vector<Graph::Part>& part) {
  queue.push(vertex);
  is_visited[vertex] = true;
  part[vertex] = Part::Left;
  while (!queue.empty()) {
    vertex = queue.front();
    queue.pop();
    for (Vertex adjacent_vertex : adjacency_list_[vertex]) {
      if (!is_visited[adjacent_vertex]) {
        queue.push(adjacent_vertex);
        is_visited[adjacent_vertex] = true;
        part[adjacent_vertex] =
            (part[vertex] == Part::Left) ? Part::Right : Part::Left;
      } else if (part[adjacent_vertex] == part[vertex]) {
        return false;
      }
    }
  }
  return true;
}

bool Graph::IsBipartite() {
  std::queue<Vertex> queue;
  std::vector<bool> is_visited(vertices_count_, false);
  std::vector<Part> part(vertices_count_);
  for (int i = 0; i < vertices_count_; ++i) {
    if (!is_visited[i] && !IsBipartiteComponent(i, queue, is_visited, part)) {
      return false;
    }
  }
  return true;
}

int main() {
  size_t vertices_count = 0;
  size_t edges_count = 0;
  std::cin >> vertices_count >> edges_count;
  std::vector<std::pair<Vertex, Vertex>> edges;
  for (int i = 0; i < edges_count; ++i) {
    Vertex vertex1;
    Vertex vertex2;
    std::cin >> vertex1 >> vertex2;
    edges.emplace_back(vertex1, vertex2);
  }
  Graph graph(vertices_count, edges);
  std::cout << (graph.IsBipartite() ? "YES\n" : "NO\n");
  return 0;
}