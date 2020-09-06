/*
Дан массив из целых чисел a1, a2, ..., an (индексация с 1!).
Для каждого запроса [left, right] найдите такой подотрезок al, al+1, ..., ar
этого массива (1 <= left <= l <= r  <= right <= n),
что сумма чисел al + al+1 + ... + ar является максимально возможной.
Требуемое время ответа на запрос - O(log n).
Формат входных данных.
Входные данные содержат один или несколько тестовых примеров.
Описание каждого из них начинается с двух чисел n и m - длины массива
и числа интересующих подотрезков.
В следующей строке содержится n чисел — элементы массива.
Каждое из этих чисел по абсо­лютной величине не превосходит 10^4.
Далее следуют описания подотрезков, каждое описание состоит из двух чисел
left и right , обозначаю­щих левый и правый конец подотрезка
(1 <= left <= right  <= n).
Суммарная длина всех массивов, а также суммарное число подотрезков
не превосходит 105.
Формат выходных данных.
Для каждого из тестовых примеров выведите m чисел:
искомую максимальную сумму для каж­дого из подотрезков.
*/

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

class IntervalTree {
 public:
  IntervalTree(const std::vector<int>& vector);

  int GetMaxSubarraySum(size_t left, size_t right) const;

 private:
  struct Node;

  std::shared_ptr<Node> CreateTree(size_t left, size_t right);

  struct IntervalInfo;

  IntervalInfo GetIntervalInfo(const std::shared_ptr<Node>& node, size_t left,
                               size_t right) const;

  std::shared_ptr<Node> root_;

  std::vector<int> prefix_sum_;

  static constexpr int infinity = std::numeric_limits<int>::max() / 2;
};

struct IntervalTree::IntervalInfo {
 public:
  int min_prefix_sum_;
  int max_prefix_sum_;
  int max_subarray_sum_;
};

struct IntervalTree::Node {
 public:
  Node(const size_t left, const size_t right) : left_(left), right_(right) {}

  size_t left_;
  size_t right_;
  std::shared_ptr<Node> left_child_;
  std::shared_ptr<Node> right_child_;
  IntervalInfo data_;
};

IntervalTree::IntervalTree(const std::vector<int>& vector) {
  prefix_sum_.resize(vector.size() + 1, 0);
  for (int i = 0; i < vector.size(); ++i) {
    prefix_sum_[i + 1] = prefix_sum_[i] + vector[i];
  }
  root_ = CreateTree(1, vector.size());
}

std::shared_ptr<IntervalTree::Node> IntervalTree::CreateTree(
    const size_t left, const size_t right) {
  auto node = std::make_shared<Node>(left, right);
  if (left == right) {
    node->left_child_ = node->right_child_ = nullptr;
    node->data_ = {std::min(prefix_sum_[left], prefix_sum_[left - 1]),
                   prefix_sum_[left],
                   prefix_sum_[left] - prefix_sum_[left - 1]};
  } else {
    node->left_child_ = CreateTree(left, (left + right) / 2);
    node->right_child_ = CreateTree((left + right) / 2 + 1, right);
    node->data_ = {
        std::min(node->left_child_->data_.min_prefix_sum_,
                 node->right_child_->data_.min_prefix_sum_),
        std::max(node->left_child_->data_.max_prefix_sum_,
                 node->right_child_->data_.max_prefix_sum_),
        std::max(std::max(node->left_child_->data_.max_subarray_sum_,
                          node->right_child_->data_.max_subarray_sum_),
                 node->right_child_->data_.max_prefix_sum_ -
                     node->left_child_->data_.min_prefix_sum_)};
  }
  return node;
}

IntervalTree::IntervalInfo IntervalTree::GetIntervalInfo(
    const std::shared_ptr<Node>& node, const size_t left,
    const size_t right) const {
  if (node->right_ < left || node->left_ > right) {
    return {infinity, -infinity, -infinity};
  }
  if (left <= node->left_ && node->right_ <= right) {
    return node->data_;
  }
  auto left_child_info = GetIntervalInfo(node->left_child_, left, right);
  auto right_child_info = GetIntervalInfo(node->right_child_, left, right);
  return {std::min(left_child_info.min_prefix_sum_,
                   right_child_info.min_prefix_sum_),
          std::max(left_child_info.max_prefix_sum_,
                   right_child_info.max_prefix_sum_),
          std::max(std::max(left_child_info.max_subarray_sum_,
                            right_child_info.max_subarray_sum_),
                   right_child_info.max_prefix_sum_ -
                       left_child_info.min_prefix_sum_)};
}

int IntervalTree::GetMaxSubarraySum(const size_t left,
                                    const size_t right) const {
  return GetIntervalInfo(root_, left + 1, right + 1).max_subarray_sum_;
}

int main() {
  size_t number_count = 0;
  size_t request_count = 0;
  while (std::cin >> number_count >> request_count) {
    std::vector<int> vector(number_count);
    for (int i = 0; i < number_count; ++i) {
      std::cin >> vector[i];
    }
    IntervalTree tree(vector);
    for (int i = 0; i < request_count; ++i) {
      size_t left = 0;
      size_t right = 0;
      std::cin >> left >> right;
      std::cout << tree.GetMaxSubarraySum(left - 1, right - 1) << std::endl;
    }
  }
  return 0;
}