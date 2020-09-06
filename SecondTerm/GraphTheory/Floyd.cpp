/*
Полный ориентированный взвешенный граф задан матрицей смежности.
Постройте матрицу кратчайших путей между его вершинами.
Гарантируется, что в графе нет циклов отрицательного веса.
Формат входного файла
В первой строке вводится единственное число N
(1 ≤ N ≤ 100) — количество вершин графа.
В следующих N строках по N чисел задается матрица смежности графа
(j-ое число в i-ой строке — вес ребра из вершины i в вершину j).
Все числа по модулю не превышают 100.
На главной диагонали матрицы — всегда нули.
Формат выходного файла
Выведите N строк по N чисел — матрицу расстояний между парами вершин,
где j-ое число в i-ой строке равно весу кратчайшего пути из вершины i в j.
*/

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using Vertex = size_t;

class Graph {
 public:
  Graph(size_t vertex_count);

  const std::vector<std::vector<int>>& GetMinDistances() const;

 private:
  void InputGraph();

  void CalculateMinDistances();

  size_t vertex_count_;
  std::vector<std::vector<int>> adjacency_matrix_;
  std::vector<std::vector<int>> distances_;
};

Graph::Graph(const size_t vertex_count)
    : vertex_count_(vertex_count),
      adjacency_matrix_(vertex_count, std::vector<int>(vertex_count)) {
  InputGraph();
  CalculateMinDistances();
}

void Graph::InputGraph() {
  for (int i = 0; i < vertex_count_; ++i) {
    for (int j = 0; j < vertex_count_; ++j) {
      std::cin >> adjacency_matrix_[i][j];
    }
  }
}

void Graph::CalculateMinDistances() {
  distances_ = adjacency_matrix_;
  for (int k = 0; k < vertex_count_; ++k) {
    for (int i = 0; i < vertex_count_; ++i) {
      for (int j = 0; j < vertex_count_; ++j) {
        distances_[i][j] =
            std::min(distances_[i][j], distances_[i][k] + distances_[k][j]);
      }
    }
  }
}

const std::vector<std::vector<int>>& Graph::GetMinDistances() const {
  return distances_;
}

void PrintMatrix(const std::vector<std::vector<int>>& matrix) {
  for (const auto& vector : matrix) {
    for (int value : vector) {
      std::cout << value << ' ';
    }
    std::cout << std::endl;
  }
}

int main() {
  size_t vertex_count = 0;
  std::cin >> vertex_count;
  Graph graph(vertex_count);
  PrintMatrix(graph.GetMinDistances());
  return 0;
}