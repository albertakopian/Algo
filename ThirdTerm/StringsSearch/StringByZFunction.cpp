#include <iostream>
#include <vector>



/*	Найти лексикографически-минимальную строку, построенную по z-функции, в алфавите a-z.

	Пример
	Ввод		Вывод
	5 3 2 1 0	aaaab
*/

class StringCreatorByPrefixFunction {
 public:
  StringCreatorByPrefixFunction(std::vector<size_t> &prefix_function,
                                size_t alphabet_size) : pi_(prefix_function), alphabet_size_(alphabet_size) {
    indexes_of_letters_in_the_alphabet_.resize(alphabet_size, 0);
  }

  void RefreshVector() {
    for (int i = 0; i < alphabet_size_; ++i) {
      indexes_of_letters_in_the_alphabet_[i] = 0;
    }
  }

  size_t FindMinElem() {
    size_t index = 0;
    while (indexes_of_letters_in_the_alphabet_[index] == 1) {
      ++index;
    }
    return index;
  }

  void MakeMinString() {
    size_t first_index = 1;
    min_string_.push_back(0);
    indexes_of_letters_in_the_alphabet_[0] = 1;
    while (first_index < pi_.size()) {
      if (pi_[first_index] == 0) {
        size_t second_index = first_index;
        while (second_index > 0) {
          indexes_of_letters_in_the_alphabet_
          [min_string_[pi_[second_index - 1]]] = 1;
          second_index = pi_[second_index - 1];
        }
        min_string_.push_back(FindMinElem());
        RefreshVector();
        ++first_index;
      } else {
        min_string_.push_back(min_string_[pi_[first_index] - 1]);
        ++first_index;
      }
    }
  }

  void PrintMinString() {
    for (size_t i = 0; i < pi_.size(); ++i) {
      std::cout << static_cast<char>('a' + min_string_[i]);
    }
  }

 private:
  std::vector<size_t> pi_;
  size_t alphabet_size_;
  std::vector<size_t> indexes_of_letters_in_the_alphabet_;
  std::vector<size_t> min_string_;
};

void MakePrefFuctionByZFunction(const std::vector<size_t> &z_func, std::vector<size_t> &pref_func) {
  pref_func.resize(z_func.size(), 0);
  for (size_t first_index = 1; first_index < z_func.size(); ++first_index) {
    for (size_t second_index = first_index + z_func[first_index] - 1;
         second_index >= first_index && pref_func[second_index] == 0;
         --second_index) {
      pref_func[second_index] = second_index - first_index + 1;
    }
  }
}

int main() {
  std::vector<size_t> pre_func;
  std::vector<size_t > z_func;
  const size_t alphabet_size = 26;
  size_t elem = 0;
  while (std::cin >> elem) {
    z_func.push_back(elem);
  }

  MakePrefFuctionByZFunction(z_func, pre_func);
  StringCreatorByPrefixFunction s_c(pre_func, alphabet_size);

  s_c.MakeMinString();
  s_c.PrintMinString();

  return 0;
}
