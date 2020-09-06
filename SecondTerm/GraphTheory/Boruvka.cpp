/*
Дан неориентированный связный граф.
Требуется найти вес минимального остовного дерева в этом графе.
Вариант 1. С помощью алгоритма Прима.
Вариант 2. С помощью алгоритма Крускала.
Вариант 3. С помощью алгоритма Борувки.
Ваш номер варианта прописан в ведомости.
Формат входного файла.
Первая строка содержит два натуральных числа 𝑛 и 𝑚 — количество
вершин и ребер графа соответственно (1 ≤ n ≤ 20000, 0 ≤ m ≤ 100000).
Следующие 𝑚 строк содержат описание ребер по одному на строке.
Ребро номер 𝑖 описывается тремя натуральными числами 𝑏𝑖, 𝑒𝑖 и 𝑤𝑖 — номера
концов ребра и его вес соответственно (1 ≤ 𝑏𝑖, 𝑒𝑖 ≤ 𝑛, 0 ≤ 𝑤𝑖 ≤ 100000).
Формат выходного файла.
Выведите единственное целое число - вес минимального остовного дерева.
*/

#include <iostream>
#include <limits>
#include <vector>

using Vertex = size_t;

class Graph {
 public:
  Graph(size_t vertex_count = 0);

  void AddEdge(Vertex from, Vertex to, size_t weight);

  size_t GetMstWeight() const;

 private:
  struct TargetVertex;

  void AddNextEdgesToForest(
      std::vector<std::vector<TargetVertex>>& mst_adjacency_lists,
      const std::vector<size_t>& tree_number) const;

  void UpdateForest(
      const std::vector<std::vector<TargetVertex>>& mst_adjacency_lists,
      std::vector<size_t>& tree_number, size_t& mst_weight,
      size_t& tree_count) const;

  void UpdateTree(
      std::pair<Vertex, size_t>&& parameters,
      const std::vector<std::vector<TargetVertex>>& mst_adjacency_lists,
      std::vector<size_t>& tree_number, std::vector<bool>& is_visited,
      size_t& mst_weight) const;

  static constexpr size_t infinity = std::numeric_limits<size_t>::max();

  static constexpr size_t null_vertex = std::numeric_limits<size_t>::max();

  size_t vertex_count_;

  std::vector<std::vector<TargetVertex>> adjacency_lists_;
};

Graph::Graph(const size_t vertex_count)
    : vertex_count_(vertex_count), adjacency_lists_(vertex_count) {}

struct Graph::TargetVertex {
 public:
  TargetVertex(const Vertex to, const size_t weight)
      : to_(to), weight_(weight) {}

  Vertex to_;
  size_t weight_;
};

void Graph::AddEdge(const Vertex from, const Vertex to, const size_t weight) {
  adjacency_lists_[from].emplace_back(to, weight);
  adjacency_lists_[to].emplace_back(from, weight);
}

void Graph::AddNextEdgesToForest(
    std::vector<std::vector<TargetVertex>>& mst_adjacency_lists,
    const std::vector<size_t>& tree_number) const {
  struct Edge {
   public:
    Edge(const Vertex from = null_vertex, const Vertex to = null_vertex,
         const size_t weight = infinity)
        : from_(from), to_(to), weight_(weight) {}

    Vertex from_;
    Vertex to_;
    size_t weight_;
  };

  std::vector<Edge> next_edge(vertex_count_);
  for (int i = 0; i < vertex_count_; ++i) {
    for (const auto& target : adjacency_lists_[i]) {
      if (tree_number[i] != tree_number[target.to_] &&
          next_edge[tree_number[i]].weight_ > target.weight_) {
        next_edge[tree_number[i]] = Edge(i, target.to_, target.weight_);
      }
    }
  }
  for (const auto& edge : next_edge) {
    if (edge.weight_ != infinity) {
      mst_adjacency_lists[edge.from_].emplace_back(edge.to_, edge.weight_);
      mst_adjacency_lists[edge.to_].emplace_back(edge.from_, edge.weight_);
    }
  }
}

void Graph::UpdateTree(
    std::pair<Vertex, size_t>&& parameters,
    const std::vector<std::vector<TargetVertex>>& mst_adjacency_lists,
    std::vector<size_t>& tree_number, std::vector<bool>& is_visited,
    size_t& mst_weight) const {
  auto [vertex, current_tree_number] = parameters;
  is_visited[vertex] = true;
  tree_number[vertex] = current_tree_number;
  for (const auto& target : mst_adjacency_lists[vertex]) {
    if (!is_visited[target.to_]) {
      mst_weight += target.weight_;
      UpdateTree({target.to_, current_tree_number}, mst_adjacency_lists,
                 tree_number, is_visited, mst_weight);
    }
  }
}

void Graph::UpdateForest(
    const std::vector<std::vector<TargetVertex>>& mst_adjacency_lists,
    std::vector<size_t>& tree_number, size_t& mst_weight,
    size_t& tree_count) const {
  std::vector<bool> is_visited(vertex_count_, false);
  mst_weight = 0;
  tree_count = 0;
  for (int i = 0; i < vertex_count_; ++i) {
    if (!is_visited[i]) {
      ++tree_count;
      UpdateTree({i, tree_number[i]}, mst_adjacency_lists, tree_number,
                 is_visited, mst_weight);
    }
  }
}

size_t Graph::GetMstWeight() const {
  std::vector<size_t> tree_number(vertex_count_);
  for (int i = 0; i < vertex_count_; ++i) {
    tree_number[i] = i;
  }
  std::vector<std::vector<TargetVertex>> mst_adjacency_lists(vertex_count_);
  size_t tree_count = vertex_count_;
  size_t mst_weight = 0;
  while (tree_count > 1) {
    AddNextEdgesToForest(mst_adjacency_lists, tree_number);
    UpdateForest(mst_adjacency_lists, tree_number, mst_weight, tree_count);
  }
  return mst_weight;
}

int main() {
  size_t vertex_count = 0;
  size_t edge_count = 0;
  std::cin >> vertex_count >> edge_count;
  Graph graph(vertex_count);
  for (int i = 0; i < edge_count; ++i) {
    Vertex from = 0;
    Vertex to = 0;
    size_t weight = 0;
    std::cin >> from >> to >> weight;
    graph.AddEdge(from - 1, to - 1, weight);
  }
  std::cout << graph.GetMstWeight() << std::endl;
  return 0;
}