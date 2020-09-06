/*
Даны неотрицательные целые числа n, k
и массив целых чисел из диапазона [0..109] размера n.
Требуется найти k-ю порядковую статистику.
т.е. напечатать число, которое бы стояло
на позиции с индексом k ∈[0..n-1] в отсортированном массиве.
Напишите нерекурсивный алгоритм.
Требования к дополнительной памяти: O(n).
Требуемое среднее время работы: O(n).
Функцию Partition следует реализовывать
методом прохода двумя итераторами в одном направлении.
Описание для случая прохода от начала массива к концу:
Выбирается опорный элемент.
Опорный элемент меняется с последним элементом массива.
Во время работы Partition в начале массива содержатся элементы,
не бОльшие опорного. Затем располагаются элементы,
строго бОльшие опорного. В конце массива лежат
нерассмотренные элементы. Последним элементом лежит опорный.
Итератор (индекс) i указывает на начало группы элементов,
строго бОльших опорного.
Итератор j больше i, итератор j указывает на первый нерассмотренный элемент.
Шаг алгоритма. Рассматривается элемент, на который указывает j.
Если он больше опорного, то сдвигаем j. Если он не больше опорного,
то меняем a[i] и a[j] местами, сдвигаем i и сдвигаем j.
В конце работы алгоритма меняем опорный и элемент,
на который указывает итератор i.
*/

#include <iostream>
#include <vector>

template <typename T, typename Comparator = std::less<>>
bool IsBetween(T c, T a, T b, Comparator comparator = Comparator()) {
  return comparator(a, c) && comparator(c, b) ||
         comparator(b, c) && comparator(c, a);
}

template <typename T, typename Comparator = std::less<>>
size_t GetPivotPosition(const std::vector<T>& initial_array, size_t left_bound,
                        size_t right_bound,
                        Comparator comparator = Comparator()) {
  size_t range_middle = (left_bound + right_bound) / 2;
  if (IsBetween<T, Comparator>(initial_array[range_middle],
                               initial_array[left_bound],
                               initial_array[right_bound], comparator)) {
    return range_middle;
  }
  if (IsBetween<T, Comparator>(initial_array[left_bound],
                               initial_array[range_middle],
                               initial_array[right_bound], comparator)) {
    return left_bound;
  }
  return right_bound;
}

template <typename T, typename Comparator = std::less<>>
void Partition(std::vector<T>& initial_array, size_t left_bound,
               size_t right_bound, size_t& pivot_position,
               Comparator comparator = Comparator()) {
  std::swap(initial_array[pivot_position], initial_array[left_bound]);
  pivot_position = left_bound;
  int i = right_bound;
  for (int j = right_bound; j > left_bound; --j) {
    if (!comparator(initial_array[j], initial_array[pivot_position])) {
      std::swap(initial_array[j], initial_array[i]);
      --i;
    }
  }
  std::swap(initial_array[i], initial_array[pivot_position]);
  pivot_position = i;
}

template <typename T, typename Comparator = std::less<>>
T& GetKthOrdinalStatistic(std::vector<T>& initial_array, size_t k,
                          Comparator comparator = Comparator()) {
  size_t left_bound = 0;
  size_t right_bound = initial_array.size() - 1;
  while (left_bound < right_bound) {
    size_t pivot_position = GetPivotPosition<T, Comparator>(
        initial_array, left_bound, right_bound, comparator);
    Partition(initial_array, left_bound, right_bound, pivot_position,
              comparator);
    if (pivot_position - left_bound == k) {
      return initial_array[pivot_position];
    }
    if (pivot_position - left_bound < k) {
      k -= (pivot_position - left_bound + 1);
      left_bound = pivot_position + 1;
    } else {
      right_bound = pivot_position - 1;
    }
  }
  return initial_array[left_bound];
}

int main() {
  size_t initial_array_size = 0;
  size_t k = 0;
  std::cin >> initial_array_size >> k;
  std::vector<int> initial_array(initial_array_size);
  for (int i = 0; i < initial_array_size; ++i) {
    std::cin >> initial_array[i];
  }
  std::cout << GetKthOrdinalStatistic<int, std::less<int>>(initial_array, k,
                                                           std::less<int>())
            << '\n';
  return 0;
}
