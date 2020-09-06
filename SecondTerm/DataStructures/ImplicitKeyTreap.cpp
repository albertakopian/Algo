/*
Реализуйте структуру данных “массив строк” на основе декартового дерева
по неявному ключу со следующими методами:
// Добавление строки в позицию position.
// Все последующие строки сдвигаются на одну позицию вперед.
void InsertAt( int position, const std::string& value );
// Удаление строки из позиции position.
// Все последующие строки сдвигаются на одну позицию назад.
void DeleteAt( int position );
// Получение строки из позиции position.
std::string GetAt( int position );
Все методы должны работать за O(log n) в среднем,
где n – текущее количество строк в массиве.
Формат входных данных.
Первая строка содержит количество команд k ≤ 10^6.
Последующие k строк содержат описания команд:
Команда "+ 10 hello" означает добавление строки hello в позицию 10.
Команда “- 14 16” означает удаление строк
от позиции 14 до позиции 16 включительно.
Команда “? 33” означает запрос на вывод строки из массива в позиции 33.
Формат выходных данных.
Выведите все строки, запрошенные командами “?”.
*/

#include <iostream>
#include <memory>
#include <random>
#include <string>

class StringArray {
 public:
  void Insert(size_t index, const std::string& value);

  void Delete(size_t from, size_t to);

  const std::string& Get(size_t index) const;

 private:
  struct Node;

  size_t GetSize(const std::shared_ptr<Node>& node) const;

  void UpdateSize(const std::shared_ptr<Node>& node);

  const std::string& Get(const std::shared_ptr<Node>& node, size_t index) const;

  void Split(std::shared_ptr<Node> node, std::shared_ptr<Node>& left,
             std::shared_ptr<Node>& right, size_t index);

  std::shared_ptr<Node> Merge(const std::shared_ptr<Node>& left,
                              const std::shared_ptr<Node>& right);

  std::shared_ptr<Node> root_ = nullptr;

  std::mt19937 random_;
};

struct StringArray::Node {
 public:
  Node(const std::string& value, const size_t priority)
      : value_(value), priority_(priority) {}

  std::string value_;
  size_t priority_;
  size_t subtree_size_ = 1;
  std::shared_ptr<Node> left_child_ = nullptr;
  std::shared_ptr<Node> right_child_ = nullptr;
};

size_t StringArray::GetSize(const std::shared_ptr<Node>& node) const {
  return node != nullptr ? node->subtree_size_ : 0;
}

void StringArray::UpdateSize(const std::shared_ptr<Node>& node) {
  node->subtree_size_ =
      GetSize(node->left_child_) + GetSize(node->right_child_) + 1;
}

void StringArray::Split(std::shared_ptr<Node> node, std::shared_ptr<Node>& left,
                        std::shared_ptr<Node>& right, const size_t index) {
  if (node == nullptr) {
    left = right = nullptr;
    return;
  }
  if (GetSize(node->left_child_) >= index) {
    Split(node->left_child_, left, node->left_child_, index);
    right = node;
  } else {
    Split(node->right_child_, node->right_child_, right,
          index - GetSize(node->left_child_) - 1);
    left = node;
  }
  UpdateSize(node);
}

std::shared_ptr<StringArray::Node> StringArray::Merge(
    const std::shared_ptr<Node>& left, const std::shared_ptr<Node>& right) {
  if (left == nullptr || right == nullptr) {
    return left != nullptr ? left : right;
  }
  if (left->priority_ > right->priority_) {
    left->right_child_ = Merge(left->right_child_, right);
    UpdateSize(left);
    return left;
  }
  right->left_child_ = Merge(left, right->left_child_);
  UpdateSize(right);
  return right;
}

void StringArray::Insert(size_t index, const std::string& value) {
  std::shared_ptr<Node> left;
  std::shared_ptr<Node> right;
  Split(root_, left, right, index);
  auto node = std::make_shared<Node>(value, random_());
  root_ = Merge(Merge(left, node), right);
}

void StringArray::Delete(const size_t from, const size_t to) {
  std::shared_ptr<Node> left;
  std::shared_ptr<Node> middle;
  std::shared_ptr<Node> right;
  Split(root_, left, middle, from);
  Split(middle, middle, right, to - from + 1);
  root_ = Merge(left, right);
}

const std::string& StringArray::Get(const std::shared_ptr<Node>& node,
                                    const size_t index) const {
  if (GetSize(node->left_child_) > index) {
    return Get(node->left_child_, index);
  }
  if (GetSize(node->left_child_) < index) {
    return Get(node->right_child_, index - GetSize(node->left_child_) - 1);
  }
  return node->value_;
}

const std::string& StringArray::Get(size_t index) const {
  return Get(root_, index);
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  size_t request_count = 0;
  std::cin >> request_count;
  StringArray array;
  for (int i = 0; i < request_count; ++i) {
    char operation = 0;
    size_t index = 0;
    std::cin >> operation >> index;
    if (operation == '+') {
      std::string value;
      std::cin >> value;
      array.Insert(index, value);
    } else if (operation == '-') {
      size_t second_index;
      if (std::cin.peek() == '\n') {
        second_index = index;
      } else {
        std::cin >> second_index;
      }
      array.Delete(index, second_index);
    } else {
      std::cout << array.Get(index) << '\n';
    }
  }
  return 0;
}
