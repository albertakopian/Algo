/*
Дан невзвешенный неориентированный граф. Найдите цикл минимальной длины.
Формат ввода
В первой строке вводятся два натуральных числа N и M, не превосходящих 10000.
Далее идут M строк по 2 числа (от 0 до N-1) - индексы вершин
между которыми есть ребро.
Формат вывода
Требуется вывести одно целое число равное длине минимального цикла.
Если цикла нет, то вывести -1.
*/

#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>
#include <vector>

using Vertex = size_t;

class Graph {
 public:
  Graph(size_t vertices_count);

  void AddEdge(Vertex vertex1, Vertex vertex2);

  size_t GetMinCycleLenght();

 private:
  size_t vertices_count_;
  size_t min_cycle_lenght_;
  std::vector<std::vector<Vertex>> adjacency_list_;

  void TryFindMinCycle(Vertex vertex, std::vector<bool>& is_visited,
                       std::vector<size_t>& depth);
};

Graph::Graph(size_t vertices_count) {
  vertices_count_ = vertices_count;
  min_cycle_lenght_ = std::numeric_limits<size_t>::max();
  adjacency_list_.resize(vertices_count_);
}

void Graph::AddEdge(Vertex vertex1, Vertex vertex2) {
  min_cycle_lenght_ = std::numeric_limits<size_t>::max();
  adjacency_list_[vertex1].push_back(vertex2);
  adjacency_list_[vertex2].push_back(vertex1);
}

size_t Graph::GetMinCycleLenght() {
  if (min_cycle_lenght_ == std::numeric_limits<size_t>::max()) {
    std::vector<bool> is_visited(vertices_count_, false);
    std::vector<size_t> depth(vertices_count_, 0);
    for (int i = 0; i < vertices_count_; ++i) {
      TryFindMinCycle(i, is_visited, depth);
    }
    if (min_cycle_lenght_ == std::numeric_limits<size_t>::max()) {
      min_cycle_lenght_ = 0;
    }
  }
  return min_cycle_lenght_;
}

void Graph::TryFindMinCycle(Vertex vertex, std::vector<bool>& is_visited,
                            std::vector<size_t>& depth) {
  std::queue<Vertex> queue;
  is_visited.assign(vertices_count_, false);
  depth.assign(vertices_count_, 0);
  queue.push(vertex);
  is_visited[vertex] = true;
  const int path_in_cycle_count = 2;
  while (!queue.empty() &&
         path_in_cycle_count * depth[vertex] < min_cycle_lenght_) {
    vertex = queue.front();
    queue.pop();
    for (Vertex adjacent_vertex : adjacency_list_[vertex]) {
      if (!is_visited[adjacent_vertex]) {
        is_visited[adjacent_vertex] = true;
        depth[adjacent_vertex] = depth[vertex] + 1;
        queue.push(adjacent_vertex);
      } else if (depth[adjacent_vertex] >= depth[vertex]) {
        min_cycle_lenght_ = std::min(
            min_cycle_lenght_, depth[adjacent_vertex] + depth[vertex] + 1);
      }
    }
  }
}

int main() {
  size_t vertices_count = 0;
  size_t edges_count = 0;
  std::cin >> vertices_count >> edges_count;
  Graph graph(vertices_count);
  Vertex vertex1;
  Vertex vertex2;
  for (int i = 0; i < edges_count; ++i) {
    std::cin >> vertex1 >> vertex2;
    graph.AddEdge(vertex1, vertex2);
  }
  if (graph.GetMinCycleLenght() != 0) {
    std::cout << graph.GetMinCycleLenght() << std::endl;
  } else {
    std::cout << -1 << std::endl;
  }
  return 0;
}