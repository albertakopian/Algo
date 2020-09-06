/*
Требуется отыскать самый выгодный маршрут между городами.
Требуемое время работы O((N+M)logN), где N-количество городов,
M-известных дорог между ними.
Оптимизируйте ввод
Формат входных данных.
Первая строка содержит число N – количество городов.
Вторая строка содержит число M - количество дорог.
Каждая следующая строка содержит описание дороги (откуда, куда, время в пути).
Последняя строка содержит маршрут (откуда и куда нужно доехать).
Формат выходных данных.
Вывести длину самого выгодного маршрута.
*/

#include <iostream>
#include <numeric>
#include <set>
#include <vector>

using Vertex = size_t;

class Graph {
 public:
  Graph(size_t vertex_count, size_t edge_count);

  size_t GetMinDistance(Vertex from, Vertex to);

 private:
  struct TargetVertex;

  class VertexComparator;

  void InputEdges(size_t edge_count);

  static constexpr size_t infinity = std::numeric_limits<size_t>::max();

  size_t vertex_count_;
  std::vector<std::vector<TargetVertex>> adjacency_lists_;
};

Graph::Graph(const size_t vertex_count, const size_t edge_count)
    : vertex_count_(vertex_count), adjacency_lists_(vertex_count) {
  InputEdges(edge_count);
}

struct Graph::TargetVertex {
 public:
  TargetVertex(const Vertex vertex, const size_t edge_weight)
      : vertex(vertex), edge_weight(edge_weight) {}

  Vertex vertex;
  size_t edge_weight;
};

void Graph::InputEdges(const size_t edge_count) {
  Vertex vertex1;
  Vertex vertex2;
  size_t edge_weight;
  for (int i = 0; i < edge_count; ++i) {
    std::cin >> vertex1 >> vertex2 >> edge_weight;
    adjacency_lists_[vertex1].emplace_back(vertex2, edge_weight);
    adjacency_lists_[vertex2].emplace_back(vertex1, edge_weight);
  }
}

class Graph::VertexComparator {
 public:
  VertexComparator(const std::vector<size_t>& distance) : distance_(distance) {}

  bool operator()(const Vertex lhs, const Vertex rhs) const {
    return distance_[lhs] < distance_[rhs] ||
           distance_[lhs] == distance_[rhs] && lhs < rhs;
  }

 private:
  const std::vector<size_t>& distance_;
};

size_t Graph::GetMinDistance(const Vertex from, const Vertex to) {
  std::vector<size_t> distances(vertex_count_, infinity);
  VertexComparator comparator(distances);
  std::set<Vertex, VertexComparator> wip_vertices(comparator);
  distances[from] = 0;
  wip_vertices.insert(from);
  while (!wip_vertices.empty()) {
    Vertex current_vertex = *wip_vertices.begin();
    wip_vertices.erase(wip_vertices.begin());
    if (current_vertex == to) {
      return distances[to];
    }
    for (const auto& target : adjacency_lists_[current_vertex]) {
      size_t new_distance = distances[current_vertex] + target.edge_weight;
      if (new_distance < distances[target.vertex]) {
        wip_vertices.erase(target.vertex);
        distances[target.vertex] = new_distance;
        wip_vertices.insert(target.vertex);
      }
    }
  }
  return distances[to];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  size_t city_count = 0;
  size_t road_count = 0;
  std::cin >> city_count >> road_count;
  Graph map(city_count, road_count);
  size_t from = 0;
  size_t to = 0;
  std::cin >> from >> to;
  std::cout << map.GetMinDistance(from, to) << std::endl;
  return 0;
}