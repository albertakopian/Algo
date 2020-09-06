#include <climits>
#include <deque>
#include <iostream>
#include <map>
#include <vector>

/* 	Даны строки s и t. Постройте сжатое суффиксное дерево, которое содержит все суффиксы строки s и строки t. Найдите такое дерево, которое содержит минимальное количество вершин.

	Формат ввода
	В первой строке записана строка s (1 ≤ |s| ≤ 105), последний символ строки равен `$', остальные символы строки — маленькие латинские буквы.
	Во второй строке записана строка t (1 ≤ |t| ≤ 105), последний символ строки равен `#', остальные символы строки — маленькие латинские буквы.

	Формат вывода
	Пронумеруйте вершины дерева от 0 до n-1 в порядке обхода в глубину, обходя поддеревья в порядке лексикографической сортировки исходящих из вершины ребер. Используйте ASCII-коды символов для опре- 		деления их порядка.

	В первой строке выведите целое число n — количество вершин дерева. В следующих n-1 строках выведите описание вершин дерева, кроме корня, в порядке увеличения их номеров.

	Описание вершины дерева v состоит из четырех целых чисел: p, w, lf, rg, где p (0 ≤ p < n, p ≠ v) — номер родителя текущей вершины, w (0 ≤ w ≤ 1) — номер строки для определения подстроки на ребре. 		Если w = 0, то на ребре, ведущем из p в v, написана подстрока s[lf … rg - 1] (0 ≤ lf < rg ≤ |s|). Если w = 1, то на ребре, ведущем из p в v, написана подстрока t[lf … rg -1] (0 ≤ lf < rg ≤ |t|).

	Пример 1
	Ввод	Вывод
	ab$	8
	ac#	0 1 2 3
		0 0 2 3
		0 0 0 1
		3 0 1 3
		3 1 1 3
		0 0 1 3
		0 1 1 3 
	Пример 2
	Ввод	Вывод
	aba$	14
	baab#	0 1 4 5
		0 0 3 4
		0 0 0 1
		3 0 3 4
		3 1 2 5
		3 0 1 2
		6 1 4 5
		6 0 2 4
		0 0 1 2
		9 1 4 5
		9 0 2 3
		11 0 3 4
		11 1 2 5
*/

struct UkkonenNode {
  std::map<char, UkkonenNode *> tree_transitions_;
  UkkonenNode() : left_(-1), right_(-1), parent_(nullptr) {}
  UkkonenNode(ssize_t left, ssize_t right, UkkonenNode *parent)
      : left_(left), right_(right), parent_(parent) {}
  ssize_t left_;
  ssize_t right_;
  UkkonenNode *parent_ = nullptr;
  UkkonenNode *suffix_link_ = nullptr;

  template <class Callback>
  ssize_t Print(ssize_t parent_number, ssize_t curr_number,
                Callback on_match) {  // DFS
    if (parent_number != -1) {
      on_match(parent_number, left_, right_);
    }
    ssize_t next_number = curr_number + 1;
    for (auto &pair : tree_transitions_) {
      next_number = pair.second->Print(curr_number, next_number, on_match);
    }
    return next_number;
  }
};

class ComprSuffTree {
 public:
  ComprSuffTree() {
    nodes_.emplace_back(UkkonenNode());
    sp_ = &nodes_[0];
  };

  void SetMaxDepth(size_t max_depth) { max_depth_ = max_depth; }

  size_t GetVertexNum() { return vertex_num_; }

  template <class Callback>
  void Print(Callback on_match) {
    nodes_[0].Print(-1, 0, on_match);
  }

  void AddSymbol(char symbol) {
    curr_prefix_.push_back(symbol);
    bool transition_exists = false;

    while (!transition_exists) {
      // если текущее ребро закончилось
      if (lag_ > sp_->right_) {
        if (sp_->tree_transitions_.find(symbol) ==
            sp_->tree_transitions_.end()) {
          ++vertex_num_;
          nodes_.emplace_back(
              UkkonenNode(static_cast<ssize_t>(curr_prefix_.size() - 1),
                          static_cast<ssize_t>(max_depth_), sp_));
          sp_->tree_transitions_[symbol] = &nodes_[nodes_.size() - 1];
          // достроение суффиксной ссылки предыдущей вершины
          if (suff_unbuilt_ != nullptr) {
            suff_unbuilt_->suffix_link_ = sp_->tree_transitions_[symbol];
            suff_unbuilt_ = nullptr;
          }
          if (sp_->right_ == -1) {
            transition_exists = true;
          } else {
            sp_ = sp_->suffix_link_;
          }
          lag_ = sp_->right_ + 1;
        } else {
          sp_ = sp_->tree_transitions_[symbol];
          lag_ = sp_->left_;
        }
      } else {
        if (curr_prefix_[static_cast<size_t>(lag_)] != symbol) {
          SuffixTransition(Split(symbol));
        } else {
          ++lag_;
          transition_exists = true;
        }
      }
    }
  }

 private:
  UkkonenNode *Split(char symbol) {
    nodes_.emplace_back(UkkonenNode(sp_->left_, lag_ - 1, sp_->parent_));
    // создание разделяющей вершины
    sp_->parent_
        ->tree_transitions_[curr_prefix_[static_cast<size_t>(sp_->left_)]] =
        &nodes_[nodes_.size() - 1];
    UkkonenNode *split_point =
        sp_->parent_
            ->tree_transitions_[curr_prefix_[static_cast<size_t>(sp_->left_)]];
    vertex_num_ += 2;

    // достроение суффиксной ссылки предыдущей вершины
    if (suff_unbuilt_ != nullptr) {
      suff_unbuilt_->suffix_link_ = split_point;
      suff_unbuilt_ = nullptr;
    }

    // ребро по несовпадающему символу ведет из разделяющей вершины в старую
    split_point->tree_transitions_[curr_prefix_[static_cast<size_t>(lag_)]] =
        sp_;
    nodes_.emplace_back(
        UkkonenNode(static_cast<ssize_t>(curr_prefix_.size() - 1),
                    static_cast<ssize_t>(max_depth_), split_point));
    // создание листа по новому символу
    split_point->tree_transitions_[symbol] = &nodes_[nodes_.size() - 1];
    sp_->left_ = lag_;
    // разделяющая вершина является новым предком старой
    sp_->parent_ = split_point;
    return split_point;
  }

  void SuffixTransition(UkkonenNode *split_point) {
    ssize_t depth = split_point->left_;
    if (split_point->parent_->right_ != -1) {
      // спуск и переход по суфф ссылке
      sp_ = split_point->parent_->suffix_link_;
    } else {
      sp_ = split_point->parent_;
      ++depth;
    }

    while (depth <= split_point->right_) {
      sp_ = sp_->tree_transitions_[curr_prefix_[static_cast<size_t>(depth)]];
      depth += sp_->right_ - sp_->left_ + 1;
    }
    // поднялись до
    if (depth == split_point->right_ + 1) {
      split_point->suffix_link_ = sp_;
    } else {
      suff_unbuilt_ = split_point;
    }
    lag_ = sp_->right_ - (depth - split_point->right_) + 2;
  }
  std::vector<char> curr_prefix_;
  UkkonenNode *suff_unbuilt_ = nullptr;
  UkkonenNode *sp_;  // первая вершина, не являющаяся листом; с неё начинается
  // добавление след символа
  std::deque<UkkonenNode> nodes_;
  ssize_t lag_ =
      0;  // положение текущего символа на ребре, лежит между sp_.left и
  // sp_.right
  size_t vertex_num_ = 1;  // число вершин в дереве
  size_t max_depth_ = LONG_MAX;
};

void Output(const std::string &first, const std::string &second) {
  size_t first_string_size = first.size();
  size_t second_string_size = second.size();
  ComprSuffTree tree;
  tree.SetMaxDepth(first_string_size + second_string_size);
  for (auto symbol : first) {
    tree.AddSymbol(symbol);
  }
  for (auto symbol : second) {
    tree.AddSymbol(symbol);
  }
  std::cout << tree.GetVertexNum() << "\n";
  tree.Print([&](ssize_t parent_number, ssize_t left, ssize_t right) {
    bool is_second_str_substr = left >= static_cast<ssize_t>(first_string_size);
    if (right == static_cast<ssize_t>(first_string_size + second_string_size)) {
      right = static_cast<ssize_t>((is_second_str_substr) ? second_string_size
                                                          : first_string_size);

    } else {
      ++right;
      if (is_second_str_substr) {
        right -= first_string_size;
      }
    }
    if (is_second_str_substr) {
      left -= first_string_size;
    }
    std::cout << parent_number << " " << is_second_str_substr << " " << left
              << " " << right << "\n";
  });
}

int main() {
  std::string first;
  std::string second;
  std::cin >> first;
  std::cin >> second;
  Output(first, second);
  return 0;
}
