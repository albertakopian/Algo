/*
Дано число N и последовательность из N целых чисел.
Найти вторую порядковую статистику на заданных диапазонах.
Для решения задачи используйте структуру данных Sparse Table.
Требуемое время обработки каждого диапазона O(1).
Время подготовки структуры данных O(n log n).
Формат входных данных.
В первой строке заданы 2 числа: размер последовательности N
и количество диапазонов M.
Следующие N целых чисел задают последовательность.
Далее вводятся M пар чисел - границ диапазонов.
Гарантируется, что каждый диапазон содержит как минимум 2 элемента.
Формат выходных данных.
Для каждого из M диапазонов напечатать элемент последовательности - 2ю
порядковую статистику. По одному числу в строке.
*/

#include <iostream>
#include <vector>

class SparseTable {
 public:
  SparseTable(const std::vector<int>& vector);

  int GetSecondOrderStatistic(size_t left_bound, size_t right_bound);

 private:
  std::pair<size_t, size_t> GetMinPair(
      const std::pair<size_t, size_t>& left,
      const std::pair<size_t, size_t>& right) const;

  std::vector<std::vector<std::pair<size_t, size_t>>> table_;
  std::vector<int> vector_;
  std::vector<size_t> log_;
};

std::pair<size_t, size_t> SparseTable::GetMinPair(
    const std::pair<size_t, size_t>& left,
    const std::pair<size_t, size_t>& right) const {
  if (vector_[left.second] <= vector_[right.first]) {
    return left;
  }
  if (vector_[left.first] >= vector_[right.second]) {
    return right;
  }
  if (vector_[left.first] <= vector_[right.first]) {
    if (left.first == right.first) {
      if (vector_[left.second] <= vector_[right.second]) {
        return left;
      }
      return right;
    }
    return {left.first, right.first};
  }
  return {right.first, left.first};
}

SparseTable::SparseTable(const std::vector<int>& vector) {
  vector_ = vector;
  log_.resize(vector.size() + 1, 0);
  for (int i = 2; i <= vector.size(); ++i) {
    log_[i] = log_[i / 2] + 1;
  }
  table_.resize(log_[vector.size()] + 1);
  table_[1].resize(vector.size() - 1);
  for (int i = 0; i < table_[1].size(); ++i) {
    table_[1][i] = {vector[i] < vector[i + 1] ? i : i + 1,
                    vector[i] < vector[i + 1] ? i + 1 : i};
  }
  for (int i = 2; i <= log_[vector.size()]; ++i) {
    table_[i].resize(vector.size() - (1 << i) + 1);
    for (int j = 0; j < table_[i].size(); ++j) {
      table_[i][j] =
          GetMinPair(table_[i - 1][j], table_[i - 1][j + (1 << i - 1)]);
    }
  }
}

int SparseTable::GetSecondOrderStatistic(const size_t left,
                                         const size_t right) {
  size_t size_log = log_[right - left + 1];
  return vector_[GetMinPair(table_[size_log][left],
                            table_[size_log][right + 1 - (1 << size_log)])
                     .second];
}

int main() {
  size_t number_count = 0;
  size_t request_count = 0;
  std::cin >> number_count >> request_count;
  std::vector<int> vector(number_count);
  for (int i = 0; i < number_count; ++i) {
    std::cin >> vector[i];
  }
  SparseTable table(vector);
  for (int i = 0; i < request_count; ++i) {
    size_t left = 0;
    size_t right = 0;
    std::cin >> left >> right;
    std::cout << table.GetSecondOrderStatistic(left - 1, right - 1) << '\n';
  }
  return 0;
}