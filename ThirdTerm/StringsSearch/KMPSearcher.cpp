#include <iostream>
#include <string>
#include <string_view>
#include <vector>

/*	Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
	p <= 30000, n <= 300000. 
	Использовать один из методов: 
	- С помощью префикс-функции;
*/

using std::string;

class KMPSearcher {
 public:
  explicit KMPSearcher(std::string_view pattern)
      : pattern_(pattern), pattern_prefix_function_arr_(pattern.size(), 0) {
    if (!pattern.empty()) {
      pattern_prefix_function_arr_[0] = 0;
    }
    for (size_t i = 1; i < pattern_.size(); ++i) {
      pattern_prefix_function_arr_[i] = GetPrefixFunctionFromPrevious(
          pattern[i], pattern_prefix_function_arr_[i - 1]);
    }
  };

  template <class Callback>
  void Search(std::string_view text, Callback on_occurrence) const {
    if (pattern_.empty()) {
      for (size_t i = 0; i < text.size() + 1; ++i) {
        on_occurrence(i);
      }
    } else {
      size_t k = 0;
      for (size_t i = 0; i < text.size(); ++i) {
        k = GetPrefixFunctionFromPrevious(text[i], k);
        if (k == pattern_.size()) {
          on_occurrence(i - k + 1);
          if (k > 0) {
            k = pattern_prefix_function_arr_[k - 1];
          }
        }
      }
    }
  }

 private:
  std::string_view pattern_;
  std::vector<size_t> pattern_prefix_function_arr_;

  size_t GetPrefixFunctionFromPrevious(char current_char,
                                       size_t prev_prefix_len) const {
    size_t prefix_len = prev_prefix_len;

    while (prefix_len > 0 && (prefix_len == pattern_.size() ||
                              current_char != pattern_[prefix_len])) {
      prefix_len = pattern_prefix_function_arr_[prefix_len - 1];
    }

    if (prefix_len < pattern_.size() && current_char == pattern_[prefix_len]) {
      ++prefix_len;
    }

    return prefix_len;
  }
};

int main() {
  string pattern;
  string text;
  std::cin >> pattern;
  std::cin >> text;

  KMPSearcher pattern_searcher(pattern);
  pattern_searcher.Search(text, [](size_t pos) { std::cout << pos << ' '; });
  return 0;
}
