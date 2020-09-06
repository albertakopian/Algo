
/* 	Дана строка длины n. Найти количество ее различных подстрок. Используйте суффиксный массив.
	Построение суффиксного массива выполняйте за O(n log n). Вычисление количества различных подстрок выполняйте за O(n).
*/



// Что тут происходит: класс SuffixArrayBuilder выполняет сортировку суффиксов,
// хранит и позволяет обращаться к суффиксному массиву и обратному к нему.
// LcpTwoAdjacentStringsArray считает lcp всех соседних пар в суффиксном
// массиве.
//
// Сортировка разбивается на два этапа. На первом происходит сортировка первых
// букв суффиксов с помощью std::sort. На втором этапе происходит удвоение строк
// до тех пор, пока число классов не станет равно длине строки, и каждый раз
// сортируются пары, состоящие из классов подстрок (каждый класс вычисляется на
// предыдущем этапе и задает половину строки). При этом, так как первые классы
// пар отсортированы на предыдущем этапе (просто потому, что номера классов
// определяются во время последовательного обхода массива в конце предыдущей
// итерации сортировки и всегда увеличиваются), то достаточно пары сортировать
// по вторым классам внутри групп, которые они образуют по первым классам. Это
// делается махинациями с обратным массивом к суффиксному.




#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using std::vector, std::string_view;

class SuffixArray {
 public:
  SuffixArray() = default;

  size_t GetPositionByRank(size_t rank) const { return positions_[rank]; }

  size_t GetRankByPosition(size_t position) const {
    return ranks_[position] - 1;
  }

  size_t Size() const { return positions_.size(); }

 private:
  friend class SuffixArrayBuilder;
  explicit SuffixArray(size_t size) : positions_(size, 0), ranks_(size, 0) {}
  vector<size_t> positions_;
  vector<size_t> ranks_;  // positions_^(-1)
};

class SuffixArrayBuilder {
 public:
  SuffixArrayBuilder() = default;

  // makes counting sort;
  SuffixArray Build(string_view string) {
    string_ = string;
    SuffixArray suffix_array(string_.size());
    suffix_array_ = &suffix_array;
    new_positions_array_.assign(string_.size(), 0);
    classes_.assign(string_.size(), {0, 0});
    position_of_first_unfilled_class_element_.assign(string_.size() + 1, 0);

    SortFirstLetters();
    size_t classes_number = UpdateClasses();
    FillRankArray();

    size_t pow_2 = 1;
    while (classes_number != string_.size()) {
      MakeSortIteration(pow_2);
      classes_number = UpdateClasses();
      FillRankArray();
      pow_2 *= 2;
    }

    return suffix_array;
  }

 private:
  SuffixArray* suffix_array_ = nullptr;
  vector<size_t> new_positions_array_;  // it is filled at each sort iteration,
                                        // then swaps with positions_;
  string_view string_;
  vector<std::pair<size_t, size_t>> classes_;
  vector<size_t>
      position_of_first_unfilled_class_element_;  // stores for each class the
                                                  // position of the first
                                                  // substring of this class,
                                                  // which has not yet been
                                                  // supplemented by the second
                                                  // half at the current
                                                  // iteration;
  void SortFirstLetters() {
    vector<std::pair<char, size_t>> first_letters;
    for (size_t position = 0; position < string_.size(); ++position) {
      first_letters.emplace_back(string_[position], position);
    }
    std::sort(first_letters.begin(), first_letters.end());

    size_t i = 0;
    for (const auto& [ch, position] : first_letters) {
      classes_[i] = {ch, 0};
      suffix_array_->positions_[i] = position;
      ++i;
    }
  }

  void MakeSortIteration(size_t pow_2) {
    /* inserts invisible $ characters to those string suffixes
     * that cannot be doubled; $ has class number 0; */
    for (size_t i = 1; i <= pow_2; ++i) {
      size_t second_class = 0;
      size_t first_class_position = string_.size() - i;
      CreatePairBySecondClassAndPositionFirstLetter(second_class,
                                                    first_class_position);
    }

    /* creates pairs for all other suffixes, skipping those that are at the
     * beginning of the string and cannot be doubled to the left; */
    for (size_t i = 0; i < suffix_array_->positions_.size(); ++i) {
      if (suffix_array_->positions_[i] >= pow_2) {
        size_t second_class = classes_[i].first;
        size_t position = suffix_array_->positions_[i] - pow_2;
        CreatePairBySecondClassAndPositionFirstLetter(second_class, position);
      }
    }

    std::swap(suffix_array_->positions_, new_positions_array_);
  }

  /* inserts a pair at the right place in the
   * classes_; the pair consists of two halves (the
   * first half starts at the position "first_class_position" of
   * the string_ and already stands at the right place
   * in the classes_, and the second half belongs to
   * the class "second_class"; */
  void CreatePairBySecondClassAndPositionFirstLetter(
      size_t second_class, size_t first_class_position) {
    size_t first_class = GetClassByPosition(first_class_position);
    size_t unfilled_position =
        position_of_first_unfilled_class_element_[first_class];
    ++position_of_first_unfilled_class_element_[first_class];
    classes_[unfilled_position].second = second_class;
    new_positions_array_[unfilled_position] = first_class_position;
  }

  void FillRankArray() {  // fills ranks_ by positions_;
    for (size_t i = 0; i < string_.size(); ++i) {
      suffix_array_->ranks_[suffix_array_->positions_[i]] = classes_[i].first;
    }
  }

  /* set new classes, saves them into classes[i].first,
   * determining the order of the first half of strings for the next iteration
   * of sort; */
  size_t UpdateClasses() {
    size_t classes_number = 0;
    std::pair<size_t, size_t> previous_pair = {0, 0};
    for (size_t i = 0; i < classes_.size(); ++i) {
      std::pair<size_t, size_t> current_pair = classes_[i];
      if (i == 0 || current_pair != previous_pair) {
        ++classes_number;
        position_of_first_unfilled_class_element_[classes_number] = i;
      }
      classes_[i].first = classes_number;
      previous_pair = current_pair;
    }
    return classes_number;
  }

  size_t GetClassByPosition(size_t position) const {
    return suffix_array_->ranks_[position];
  }
};

class LcpTwoAdjacentStringsArray {
 public:
  LcpTwoAdjacentStringsArray() = default;

  explicit LcpTwoAdjacentStringsArray(string_view string,
                                      const SuffixArray& suffix_array)
      : lcp_(string.size(), 0) {
    size_t string_len = string.size();
    size_t lcp = 0;
    for (size_t first_position = 0; first_position < string_len;
         ++first_position) {
      if (lcp > 0) {
        --lcp;
      }
      if (suffix_array.GetRankByPosition(first_position) == string_len - 1) {
        lcp = 0;
      } else {
        size_t second_position = suffix_array.GetPositionByRank(
            suffix_array.GetRankByPosition(first_position) + 1);
        while (std::max(first_position + lcp, second_position + lcp) <
                   string_len &&
               string[first_position + lcp] == string[second_position + lcp]) {
          ++lcp;
        }
        lcp_[suffix_array.GetRankByPosition(first_position)] = lcp;
      }
    }
  }

  size_t GetLcpByRank(size_t rank) const { return lcp_[rank]; }

 private:
  vector<size_t> lcp_;
};

size_t CountDifferentSubstrings(string_view string) {
  size_t number_different_substrings =
      string.size() * (string.size() + 1) /
      2;  // this is the sum of the lengths of all suffixes
  SuffixArrayBuilder suffix_array_builder;
  LcpTwoAdjacentStringsArray lcp_array(string,
                                       suffix_array_builder.Build(string));
  for (size_t i = 0; i < string.size(); ++i) {
    number_different_substrings -= lcp_array.GetLcpByRank(i);
  }
  return number_different_substrings;
}

int main() {
  std::string input_string;
  std::cin >> input_string;

  std::cout << CountDifferentSubstrings(input_string);
  return 0;
}
