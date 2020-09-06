/*
Дан ориентированный граф.
Определите, какое минимальное число ребер необходимо добавить,
чтобы граф стал сильносвязным. В графе возможны петли.
Формат ввода
В первой строке указывается число вершин графа V,
во второй – число ребер E,
в последующих – E пар вершин, задающих ребра.
Формат вывода
Минимальное число ребер k.
*/

#include <algorithm>
#include <iostream>
#include <vector>

using Vertex = size_t;

class Graph {
 public:
  Graph(size_t vertices_count);

  void AddEdge(Vertex from, Vertex to);

  size_t GetMinStrongConnectionComplementSize();

 private:
  size_t vertices_count_;
  std::vector<std::vector<size_t>> adjacency_list_;

  std::vector<std::vector<size_t>> transponed_adjacency_list_;
  std::vector<size_t> component_number_;
  std::vector<Vertex> dfs_out_order_;
  std::vector<bool> is_visited_;

  void GetOrder(Vertex vertex);

  bool IsSinkComponent(Vertex vertex);

  bool IsSourceComponent(Vertex vertex);

  size_t GetSinkComponentCount();

  size_t GetSourceComponentCount();

  void GetComponent(Vertex vertex, size_t component_number);
};

Graph::Graph(size_t vertices_count) {
  vertices_count_ = vertices_count;
  adjacency_list_.resize(vertices_count_);
  transponed_adjacency_list_.resize(vertices_count_);
}

void Graph::AddEdge(Vertex from, Vertex to) {
  adjacency_list_[from].push_back(to);
  transponed_adjacency_list_[to].push_back(from);
}

void Graph::GetOrder(Vertex vertex) {
  is_visited_[vertex] = true;
  for (Vertex adjacent_vertex : adjacency_list_[vertex]) {
    if (!is_visited_[adjacent_vertex]) {
      GetOrder(adjacent_vertex);
    }
  }
  dfs_out_order_.push_back(vertex);
}

void Graph::GetComponent(Vertex vertex, size_t component_number) {
  is_visited_[vertex] = true;
  component_number_[vertex] = component_number;
  for (Vertex adjacent_vertex : transponed_adjacency_list_[vertex]) {
    if (!is_visited_[adjacent_vertex]) {
      GetComponent(adjacent_vertex, component_number);
    }
  }
}

bool Graph::IsSinkComponent(Vertex vertex) {
  is_visited_[vertex] = true;
  for (Vertex adjacent_vertex : adjacency_list_[vertex]) {
    if (component_number_[vertex] != component_number_[adjacent_vertex] ||
        !is_visited_[adjacent_vertex] && !IsSinkComponent(adjacent_vertex)) {
      return false;
    }
  }
  return true;
}

bool Graph::IsSourceComponent(Vertex vertex) {
  is_visited_[vertex] = true;
  for (Vertex adjacent_vertex : transponed_adjacency_list_[vertex]) {
    if (component_number_[vertex] != component_number_[adjacent_vertex] ||
        !is_visited_[adjacent_vertex] && !IsSinkComponent(adjacent_vertex)) {
      return false;
    }
  }
  return true;
}

size_t Graph::GetSinkComponentCount() {
  size_t sink_count = 0;
  size_t component_number = 0;
  is_visited_.assign(vertices_count_, false);
  for (Vertex vertex : dfs_out_order_) {
    if (component_number_[vertex] > component_number) {
      if (IsSinkComponent(vertex)) {
        ++sink_count;
      }
      ++component_number;
    }
  }
  return sink_count;
}

size_t Graph::GetSourceComponentCount() {
  size_t source_count = 0;
  size_t component_number = 0;
  is_visited_.assign(vertices_count_, false);
  for (Vertex vertex : dfs_out_order_) {
    if (component_number_[vertex] > component_number) {
      if (IsSourceComponent(vertex)) {
        ++source_count;
      }
      ++component_number;
    }
  }
  return source_count;
}

size_t Graph::GetMinStrongConnectionComplementSize() {
  is_visited_.assign(vertices_count_, false);
  for (int i = 0; i < vertices_count_; ++i) {
    if (!is_visited_[i]) {
      GetOrder(i);
    }
  }
  std::reverse(dfs_out_order_.begin(), dfs_out_order_.end());

  size_t component_count = 0;
  is_visited_.assign(vertices_count_, false);
  component_number_.assign(vertices_count_, 0);  // 0 - undefined
  for (Vertex vertex : dfs_out_order_) {
    if (!is_visited_[vertex]) {
      ++component_count;
      GetComponent(vertex, component_count);
    }
  }
  size_t min_complement_size =
      std::max(GetSinkComponentCount(), GetSourceComponentCount());
  return (component_count == 1) ? 0 : min_complement_size;
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
    graph.AddEdge(vertex1 - 1, vertex2 - 1);
  }
  std::cout << graph.GetMinStrongConnectionComplementSize() << std::endl;
  return 0;
}