#include <set>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>


/* 	«Восьминашки» – упрощенный вариант известной головоломки «Пятнашки». Восемь костяшек,
	пронумерованных от 1 до 8, расставлены по ячейкам игровой доски 3 на 3, одна ячейка при этом
	остается пустой. За один ход разрешается передвинуть одну из костяшек, расположенных рядом с
	пустой ячейкой, на свободное место. Цель игры – для заданной начальной конфигурации игровой
	доски за минимальное число ходов получить выигрышную конфигурацию (пустая ячейка обозначена
	нулем):

				1 2 3
				4 5 6
				7 8 0

	Формат входного файла
	Во входном файле содержится начальная конфигурация головоломки – 3 строки по 3 числа в
	каждой.
	Формат выходного файла
	Если решение существует, то в первой строке выходного файла выведите минимальное число
	перемещений костяшек, которое нужно сделать, чтобы достичь выигрышной конфигурации, а во
	второй строке выведите соответствующую последовательность ходов: L означает, что в результате
	перемещения костяшки пустая ячейка сдвинулась влево, R – вправо, U – вверх, D – вниз. Если таких
	последовательностей несколько, то выведите любую из них. Если же выигрышная конфигурация
	недостижима, то выведите в выходной файл одно число −1.
*/

unsigned long long get_number(unsigned long long table, int index) {
  return 0xF & (table >> 4 * index);
}

void put_number(unsigned long long& table, int index, unsigned long long number) {
  table = (table & ~(static_cast<unsigned long long>(0xF) << 4 * index)) | (number << 4 * index);
}

int get_manhattan_distance(unsigned long long table) {
  int d = 0;
  for (int i = 0; i < 9; ++i) {
    int number = get_number(table, i);
    if (number != 0) {
      d += std::abs(number / 3 - i / 3) + std::abs(number % 3 - i % 3);
    }
  }
  return d;
}

bool is_solvable(unsigned long long table) {
  int inversions_number = 0;
  for (int i = 0; i < 9; ++i) {
    if (get_number(table, i) != 0) {
      for (int j = 0; j < i; ++j) {
        if (get_number(table, j) != 0 &&
          get_number(table, i) < get_number(table, j)) {
          ++inversions_number;
        }
      }
    }
  }
  return inversions_number % 2 == 0;
}

char get_zero_move_direction(char zero_index, char new_zero_index) {
  if (new_zero_index == zero_index + 1) {
    return 'R';
  }
  if (new_zero_index + 1 == zero_index) {
    return 'L';
  }
  if (new_zero_index == zero_index + 3) {
    return 'D';
  }
  return 'U';
}

void update_manhattan_distance(unsigned long long current_table, char zero_index,
  unsigned long long new_table, char new_zero_index,
  std::unordered_map<unsigned long long, int>& manhattan_distances) {
  int number = get_number(new_table, zero_index);
  manhattan_distances[new_table] = manhattan_distances[current_table] +
    std::abs(number / 3 - new_zero_index / 3) +
    std::abs(number % 3 - new_zero_index % 3) -
    std::abs(number / 3 - zero_index / 3) -
    std::abs(number % 3 - zero_index % 3);
}

struct compare {
  compare(std::unordered_map<unsigned long long, int>& distances,
    std::unordered_map<unsigned long long, int>& manhattan_distances)
    : distances(distances), manhattan_distances(manhattan_distances) {}

  bool operator()(unsigned long long a, unsigned long long b) const {
    int first_heuristic_function = distances[a] + manhattan_distances[a];
    int second_heuristic_function = distances[b] + manhattan_distances[b];
    return first_heuristic_function < second_heuristic_function ||
      first_heuristic_function == second_heuristic_function && a < b;
  }

  std::unordered_map<unsigned long long, int>& distances;
  std::unordered_map<unsigned long long, int>& manhattan_distances;
};

void update_new_table_stats(
  unsigned long long current_table, char zero_index,
  char new_zero_index,
  std::set<unsigned long long, compare>& proceeded_tables,
  std::unordered_map<unsigned long long, int>& distances,
  std::unordered_map<unsigned long long, int>& manhattan_distances,
  std::unordered_map<unsigned long long, int>& zero_positions,
  std::unordered_map<unsigned long long, std::pair<unsigned long long, char>>& parents) {
  unsigned long long new_table = current_table;
  put_number(new_table, new_zero_index, 0);
  put_number(new_table, zero_index, get_number(current_table, new_zero_index));

  if (distances.find(new_table) == distances.end() ||
    distances[new_table] > distances[current_table] + 1) {
    zero_positions[new_table] = new_zero_index;
    proceeded_tables.erase(new_table);
    distances[new_table] = distances[current_table] + 1;
    update_manhattan_distance(current_table, zero_index, new_table, new_zero_index, manhattan_distances);
    parents[new_table] = { current_table,
      get_zero_move_direction(zero_index, new_zero_index) };
    proceeded_tables.insert(new_table);
  }
}

std::pair<std::string, bool> solve(std::vector<int>& start) {
  unsigned long long start_table = 0;
  for (int i = 0; i < 9; ++i) {
    put_number(start_table, i, start[i]);
  }
  if (is_solvable(start_table)) {
    std::unordered_map<unsigned long long, int> distances;
    std::unordered_map<unsigned long long, int> manhattan_distances;
    compare comparator(distances, manhattan_distances);
    std::set<unsigned long long, compare> proceeded_tables(comparator);
    std::unordered_map<unsigned long long, int> zero_positions;
    std::unordered_map<unsigned long long, std::pair<unsigned long long, char>> parents;
    distances[start_table] = 0;
    manhattan_distances[start_table] = get_manhattan_distance(start_table);
    int i = 0;
    while (get_number(start_table, i) != 0) {
      ++i;
    }
    zero_positions[start_table] = i;
    unsigned long long current_table = start_table;
    while (current_table != 0x087654321) {
      int zero_index = zero_positions[current_table];
      if (zero_index >= 3) {
        update_new_table_stats(current_table, zero_index, zero_index - 3,
          proceeded_tables, distances, manhattan_distances, zero_positions, parents);
      }
      if (zero_index < 6) {
        update_new_table_stats(current_table, zero_index, zero_index + 3,
          proceeded_tables, distances, manhattan_distances, zero_positions, parents);
      }
      if (zero_index % 3 != 0) {
        update_new_table_stats(current_table, zero_index, zero_index - 1,
          proceeded_tables, distances, manhattan_distances, zero_positions, parents);
      }
      if ((zero_index + 1) % 3 != 0) {
        update_new_table_stats(current_table, zero_index, zero_index + 1,
          proceeded_tables, distances, manhattan_distances, zero_positions, parents);
      }
      current_table = *proceeded_tables.begin();
      proceeded_tables.erase(proceeded_tables.begin());
    }
    std::string answer;
    while (current_table != start_table) {
      answer.push_back(parents[current_table].second);
      current_table = parents[current_table].first;
    }
    std::reverse(answer.begin(), answer.end());
    return { answer,true };
  }
  return { std::string(), false };
}

int main() {
  std::vector<int> start_table(9);
  for (int i = 0; i < 9; ++i) {
    std::cin >> start_table[i];
  }
  std::pair<std::string, bool> result = solve(start_table);
  if (result.second) {
    std::cout << result.first.size() << std::endl;
    std::cout << result.first << std::endl;
  }
  else {
    std::cout << -1 << std::endl;
  }
  return 0;
}
