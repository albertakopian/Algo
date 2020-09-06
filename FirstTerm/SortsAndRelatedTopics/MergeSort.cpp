/*
Дана очень длинная последовательность целых чисел длины n.
Требуется вывести в отсортированном виде её наименьшие k элементов.
Последовательность может не помещаться в память.
Время работы O(n * log(k)). Доп. память O(k). Использовать слияние.
*/

#include <iostream>

template <typename T, typename Comparator = std::less<>>
void Merge(const T* first_array, const size_t first_array_size,
           const T* second_array, const size_t second_array_size, T* new_array,
           const size_t new_array_size, Comparator comparator = Comparator()) {
  for (int i = 0, j = 0, k = 0; k < new_array_size; ++k) {
    if (i < first_array_size && j < second_array_size) {
      if (comparator(first_array[i], second_array[j])) {
        new_array[k] = first_array[i];
        ++i;
      } else {
        new_array[k] = second_array[j];
        ++j;
      }
    } else if (i < first_array_size) {
      new_array[k] = first_array[i];
      ++i;
    } else {
      new_array[k] = second_array[j];
      ++j;
    }
  }
}

template <typename T, typename Comparator = std::less<>>
void MergeSort(T* initial_array, const size_t array_size,
               Comparator comparator = Comparator()) {
  if (array_size > 1) {
    MergeSort<T, Comparator>(initial_array, array_size / 2, comparator);
    MergeSort<T, Comparator>(initial_array + array_size / 2,
                             (array_size + 1) / 2, comparator);
    T* temp_buffer = new T[array_size];
    Merge<T, Comparator>(initial_array, array_size / 2,
                         initial_array + array_size / 2, (array_size + 1) / 2,
                         temp_buffer, array_size, comparator);
    for (int i = 0; i < array_size; ++i) {
      initial_array[i] = temp_buffer[i];
    }
    delete[] temp_buffer;
  }
}

void GetLeastSequenceElements(int* least_elements,
                              const size_t least_element_count,
                              const size_t sequence_length) {
  size_t whole_block_count = sequence_length / least_element_count;
  int* temp_buffer1 = new int[least_element_count];
  int* temp_buffer2 = new int[least_element_count];
  for (int i = 0; i < least_element_count; ++i) {
    std::cin >> temp_buffer1[i];
  }
  MergeSort<int, std::less<int>>(temp_buffer1, least_element_count,
                                 std::less<int>());
  for (int i = 0; i < whole_block_count - 1; ++i) {
    for (int j = 0; j < least_element_count; ++j) {
      std::cin >> temp_buffer2[j];
    }
    MergeSort<int, std::less<int>>(temp_buffer2, least_element_count,
                                   std::less<int>());
    Merge<int, std::less<int>>(temp_buffer1, least_element_count, temp_buffer2,
                               least_element_count, least_elements,
                               least_element_count, std::less<int>());
    for (int i = 0; i < least_element_count; ++i) {
      temp_buffer1[i] = least_elements[i];
    }
  }
  size_t remainder_element_count =
      sequence_length - whole_block_count * least_element_count;
  for (int i = 0; i < remainder_element_count; ++i) {
    std::cin >> temp_buffer2[i];
  }
  MergeSort<int, std::less<int>>(temp_buffer2, remainder_element_count,
                                 std::less<int>());
  Merge<int, std::less<int>>(temp_buffer1, least_element_count, temp_buffer2,
                             remainder_element_count, least_elements,
                             least_element_count, std::less<int>());
  delete[] temp_buffer1;
  delete[] temp_buffer2;
}

int main() {
  size_t sequence_length = 0;
  size_t least_element_count = 0;
  std::cin >> sequence_length >> least_element_count;
  int* least_elements = new int[least_element_count];
  GetLeastSequenceElements(least_elements, least_element_count,
                           sequence_length);
  for (int i = 0; i < least_element_count; ++i) {
    std::cout << least_elements[i] << ' ';
  }
  std::cout << '\n';
  delete[] least_elements;
  return 0;
}