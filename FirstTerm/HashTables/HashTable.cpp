/*
Реализуйте структуру данных типа “множество строк” на основе динамической
хеш-таблицы с открытой адресацией. Хранимые строки непустые и состоят из
строчных латинских букв. Начальный размер таблицы должен быть равным 8-ми.
Перехеширование выполняйте при добавлении элементов в случае,
когда коэффициент заполнения таблицы достигает 3/4.
Хеш-функцию строки реализуйте с помощью вычисления
значения многочлена методом Горнера.
Структура данных должна поддерживать операции добавления строки в множество,
удаления строки из множества и проверки принадлежности данной строки множеству.
Для разрешения коллизий используйте двойное хеширование.
Формат входных данных
Каждая строка входных данных задает одну операцию над множеством.
Запись операции состоит из типа операции и следующей за ним через пробел строки,
над которой проводится операция.
Тип операции  – один из трех символов:
+  означает добавление данной строки в множество;
-  означает удаление  строки из множества;
?  означает проверку принадлежности данной строки множеству.
При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ,
что он отсутствует в этом множестве. При удалении элемента
из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.
Формат выходных данных
Программа должна вывести для каждой операции одну из двух строк OK или FAIL,
в зависимости от того, встречается ли данное слово в нашем множестве.
*/

#include <iostream>
#include <string>
#include <utility>
#include <vector>

template <typename KeyType>
class Set {
 public:
  virtual ~Set() = default;
  virtual bool Insert(const KeyType& key) = 0;
  virtual bool Remove(const KeyType& key) = 0;
  virtual bool HasKey(const KeyType& key) const = 0;
};

template <typename KeyType>
class HashTable : public Set<KeyType> {
 public:
  HashTable();
  virtual ~HashTable() = default;
  virtual bool Insert(const KeyType& key) override;
  virtual bool Remove(const KeyType& key) override;
  virtual bool HasKey(const KeyType& key) const override;

 protected:
  struct HashTableNode;

  virtual size_t GetFirstHash(const KeyType& key) const = 0;
  virtual size_t GetSecondHash(const KeyType& key) const = 0;
  virtual void Rehash();

  std::vector<HashTableNode> keys_;
  size_t size_;
};

template <typename KeyType>
struct HashTable<KeyType>::HashTableNode {
  KeyType key_ = {};
  bool is_empty_ = true;
  bool is_deleted_ = false;
};

template <typename KeyType>
HashTable<KeyType>::HashTable() {
  size_ = 0;
  keys_.resize(8);
}

template <typename KeyType>
void HashTable<KeyType>::Rehash() {
  std::vector<HashTableNode> old_keys(std::move(keys_));
  size_ = 0;
  keys_.resize(2 * old_keys.size());
  for (int i = 0; i < old_keys.size(); ++i) {
    if (!old_keys[i].is_empty_) {
      Insert(old_keys[i].key_);
    }
  }
}

template <typename KeyType>
bool HashTable<KeyType>::Insert(const KeyType& key) {
  if (4 * size_ > 3 * keys_.size()) {  // rehash if load factor > 3/4
    Rehash();
  }
  size_t hash1 = GetFirstHash(key);
  size_t hash2 = GetSecondHash(key);
  for (int i = 0; i < keys_.size(); ++i) {
    size_t hash = (hash1 + i * hash2) % keys_.size();
    if (keys_[hash].is_empty_) {
      keys_[hash].key_ = std::move(key);
      keys_[hash].is_empty_ = false;
      keys_[hash].is_deleted_ = false;
      ++size_;
      return true;
    }
    if (!keys_[hash].is_empty_ && keys_[hash].key_ == key) {
      return false;
    }
  }
  return false;
}

template <typename KeyType>
bool HashTable<KeyType>::Remove(const KeyType& key) {
  size_t hash1 = GetFirstHash(key);
  size_t hash2 = GetSecondHash(key);
  for (int i = 0; i < keys_.size(); ++i) {
    size_t hash = (hash1 + i * hash2) % keys_.size();
    if (!keys_[hash].is_empty_ && keys_[hash].key_ == key) {
      keys_[hash].is_empty_ = true;
      keys_[hash].is_deleted_ = true;
      --size_;
      return true;
    }
    if (keys_[hash].is_empty_ && !keys_[hash].is_deleted_) {
      return false;
    }
  }
  return false;
}

template <typename KeyType>
bool HashTable<KeyType>::HasKey(const KeyType& key) const {
  size_t hash1 = GetFirstHash(key);
  size_t hash2 = GetSecondHash(key);
  for (int i = 0; i < keys_.size(); ++i) {
    size_t hash = (hash1 + i * hash2) % keys_.size();
    if (!keys_[hash].is_empty_ && keys_[hash].key_ == key) {
      return true;
    }
    if (keys_[hash].is_empty_ && !keys_[hash].is_deleted_) {
      return false;
    }
  }
  return false;
}

class StringSet : public HashTable<std::string> {
 private:
  virtual size_t GetFirstHash(const std::string& string) const override;
  virtual size_t GetSecondHash(const std::string& string) const override;
};

size_t StringSet::GetFirstHash(const std::string& string) const {
  const size_t modulo = 1117;  // prime
  const size_t coefficient = keys_.size();
  size_t hash = 0;
  for (auto symbol : string) {
    hash = (hash * coefficient + symbol) % modulo;
  }
  return hash;
}

size_t StringSet::GetSecondHash(const std::string& string) const {
  const size_t modulo = 1777;  // prime
  const size_t coefficient = keys_.size();
  size_t hash = 0;
  for (auto symbol : string) {
    hash = (hash * coefficient + symbol) % modulo;
  }
  return 2 * hash + 1;
}

void TestStringSet() {
  enum class Operations { Insert = '+', Remove = '-', HasKey = '?' };

  StringSet set;
  char operation = 0;
  std::string string;
  while (std::cin >> operation >> string) {
    if (operation == static_cast<char>(Operations::Insert)) {
      if (set.Insert(string)) {
        std::cout << "OK\n";
      } else {
        std::cout << "FAIL\n";
      }
    } else if (operation == static_cast<char>(Operations::Remove)) {
      if (set.Remove(string)) {
        std::cout << "OK\n";
      } else {
        std::cout << "FAIL\n";
      }
    } else if (operation == static_cast<char>(Operations::HasKey)) {
      if (set.HasKey(string)) {
        std::cout << "OK\n";
      } else {
        std::cout << "FAIL\n";
      }
    }
  }
}

int main() {
  TestStringSet();
  return 0;
}