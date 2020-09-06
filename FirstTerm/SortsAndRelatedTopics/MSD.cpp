/*
Дан массив строк. Количество строк не больше 100000.
Отсортировать массив методом поразрядной сортировки MSD по символам.
Размер алфавита - 256 символов. Последний символ строки = ‘\0’.
*/

#include <cstring>
#include <iostream>
#include <string>

const size_t alphabet_size = 256;

size_t Max(const size_t lhs, const size_t rhs) { return lhs > rhs ? lhs : rhs; }

template <typename T, typename DigitGetter, typename Numerator>
void CountingSort(T* initial_array, const size_t initial_array_size,
                  const size_t digit, size_t* counter,
                  const size_t alphabet_size, DigitGetter digit_getter,
                  Numerator numerator) {
  for (int i = 0; i <= alphabet_size; ++i) {
    counter[i] = 0;
  }
  for (int i = 0; i < initial_array_size; ++i) {
    ++counter[numerator(digit_getter(initial_array[i], digit))];
  }
  size_t sum = 0;
  for (int i = 0; i <= alphabet_size; ++i) {
    size_t temp = counter[i];
    counter[i] = sum;
    sum += temp;
  }
  T* temp_array = new T[initial_array_size];
  for (int i = 0; i < initial_array_size; ++i) {
    temp_array[counter[numerator(digit_getter(initial_array[i], digit))]++] =
        initial_array[i];
  }
  memcpy(initial_array, temp_array, initial_array_size * sizeof(T));
  delete[] temp_array;
}

template <typename T, typename DigitGetter, typename Numerator,
          typename NeedsSort>
void MSDSort(T* initial_array, const size_t initial_array_size,
             const size_t digit, const size_t alphabet_size,
             const size_t max_digit, DigitGetter digit_getter,
             Numerator numerator, NeedsSort needs_sort) {
  if (initial_array_size > 1) {
    size_t* counter = new size_t[alphabet_size + 1];
    CountingSort<T, DigitGetter, Numerator>(initial_array, initial_array_size,
                                            digit, counter, alphabet_size,
                                            digit_getter, numerator);
    if (digit < max_digit) {
      for (int i = 0; i < alphabet_size; ++i) {
        if (counter[i + 1] - counter[i] > 1 &&
            needs_sort(digit_getter(initial_array[counter[i]], digit))) {
          MSDSort<T, DigitGetter, Numerator>(
              initial_array + counter[i], counter[i + 1] - counter[i],
              digit + 1, alphabet_size, max_digit, digit_getter, numerator,
              needs_sort);
        }
      }
    }
    delete[] counter;
  }
}

int main() {
  std::string* strings = new std::string[100000];
  size_t max_length = 0;
  size_t string_count = 0;
  while (std::cin >> strings[string_count]) {
    max_length = Max(max_length, strings[string_count].size());
    ++string_count;
  }
  MSDSort<std::string, char(const std::string&, const size_t),
          size_t(const char), bool(const char)>(
      strings, string_count, 0, alphabet_size, max_length - 1,
      [](const std::string& string, size_t digit) { return string[digit]; },
      [](const char character) { return static_cast<size_t>(character); },
      [](const char character) { return character != '\0'; });
  for (int i = 0; i < string_count; ++i) {
    std::cout << strings[i] << ' ' << '\n';
  }
  return 0;
}
