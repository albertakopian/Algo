#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

struct AhoCorasickNode {
  std::vector<size_t> ids_;

  std::map<char, AhoCorasickNode> trie_transitions_;
  std::map<char, AhoCorasickNode*> automaton_transitions_cache_;

  AhoCorasickNode* suffix_link_ = nullptr;
  AhoCorasickNode* terminal_link_ = nullptr;

  bool IsTerminal() { return (!ids_.empty()); }
};

AhoCorasickNode* GetAutomatonTransition(AhoCorasickNode* node, char ch) {
  auto& transition_node = node->automaton_transitions_cache_[ch];
  if (transition_node == nullptr) {
    if (node->suffix_link_ == nullptr) {
      transition_node = node;
    } else {
      transition_node = GetAutomatonTransition(node->suffix_link_, ch);
    }
  }

  return transition_node;
}

class NodeReference {
 public:
  NodeReference() = default;
  explicit NodeReference(AhoCorasickNode* node) : node_(node){};

  [[nodiscard]] NodeReference Next(char ch) const {
    return NodeReference(GetAutomatonTransition(node_, ch));
  }

  template <class Callback>
  void ForEachMatch(const Callback&& cb) const {
    AhoCorasickNode* current_node = node_;
    do {
      for (auto& id : current_node->ids_) {
        cb(id);
      }
      current_node = current_node->terminal_link_;
    } while (current_node != nullptr);
  }

 private:
  AhoCorasickNode* node_ = nullptr;
};

class AhoCorasick {
 public:
  AhoCorasick() = default;

  AhoCorasick(const AhoCorasick&) = delete;
  AhoCorasick& operator=(const AhoCorasick&) = delete;
  AhoCorasick(AhoCorasick&&) = delete;
  AhoCorasick& operator=(AhoCorasick&&) = delete;

  NodeReference Root() { return NodeReference(&root_); };

 private:
  friend class AhoCorasickBuilder;

  mutable AhoCorasickNode root_;
};

class AhoCorasickBuilder {
 public:
  void AddString(const std::string& string, size_t id) {
    strings_.emplace_back(string);
    ids_.push_back(id);
  }

  [[nodiscard]] std::unique_ptr<AhoCorasick> Build() {
    auto automaton = std::make_unique<AhoCorasick>();
    for (size_t i = 0; i < strings_.size(); ++i) {
      AddString(&automaton->root_, strings_[i], ids_[i]);
    }
    CalculateLinks(&automaton->root_);
    return automaton;
  }

 private:
  std::vector<std::string> strings_;
  std::vector<size_t> ids_;

  static void AddString(AhoCorasickNode* root, const std::string& str,
                        size_t id) {
    AhoCorasickNode* current_node = root;
    for (char ch : str) {
      current_node = current_node->automaton_transitions_cache_[ch] =
          &current_node->trie_transitions_[ch];
    }
    current_node->ids_.push_back(id);
  }

  static void CalculateLinks(AhoCorasickNode* root) {
    std::queue<AhoCorasickNode*> nodes_queue;
    nodes_queue.push(root);
    while (!nodes_queue.empty()) {
      AhoCorasickNode* current_node = nodes_queue.front();
      nodes_queue.pop();

      for (auto& [transition_char, next_node] :
           current_node->trie_transitions_) {
        AhoCorasickNode* suffix_current_node = current_node->suffix_link_;
        if (suffix_current_node != nullptr) {
          next_node.suffix_link_ =
              GetAutomatonTransition(suffix_current_node, transition_char);
        } else {
          next_node.suffix_link_ = root;
        }

        if (next_node.suffix_link_->IsTerminal()) {
          next_node.terminal_link_ = next_node.suffix_link_;
        } else {
          next_node.terminal_link_ = next_node.suffix_link_->terminal_link_;
        }

        nodes_queue.push(&next_node);
      }
    }
  }
};

std::vector<std::string> Split(const std::string& string, char delimiter) {
  std::vector<std::string> splitted;
  bool letter_met = false;
  for (auto ch : string) {
    if (ch != delimiter) {
      if (!letter_met) {
        splitted.emplace_back();
        letter_met = true;
      }
      splitted.back().push_back(ch);
    } else {
      splitted.emplace_back();
    }
  }

  return splitted;
}

class WildcardMatcher {
 public:
  WildcardMatcher(const std::string& pattern, char wildcard)
      : occurrences_by_offset_(pattern.size() + 1),
        pattern_length_(pattern.size()) {
    AhoCorasickBuilder builder;
    std::vector<std::string> splitted = Split(pattern, wildcard);
    number_of_words_ = splitted.size();
    bool letter_met = false;
    size_t index = 0;
    for (const auto& string : splitted) {
      ++index;
      if (string.empty() && (!letter_met)) {
        builder.AddString(string, index);
      } else {
        letter_met = true;
        index += string.size();
        builder.AddString(string, index - 1);
      }
    }
    automaton_ = builder.Build();
    state_ = automaton_->Root();
  }

  // Resets the matcher. Call allows to abandon all data which was already
  // scanned, a new stream can be scanned afterwards.
  void Reset() {
    occurrences_by_offset_.assign(pattern_length_ + 1, 0);
    state_ = automaton_->Root();
  }

  template <class Callback>
  void Scan(char character, Callback&& callback) {
    state_ = state_.Next(character);
    UpdateWordOccurrencesCounters();
    if (occurrences_by_offset_.back() == number_of_words_) {
      callback();
    }
    ShiftWordOccurrencesCounters();
  }

 private:
  std::deque<size_t> occurrences_by_offset_;
  size_t number_of_words_;
  size_t pattern_length_;
  std::unique_ptr<AhoCorasick> automaton_;
  NodeReference state_;

  void UpdateWordOccurrencesCounters() {
    state_.ForEachMatch([&](size_t index) { ++occurrences_by_offset_[index]; });
  }

  void ShiftWordOccurrencesCounters() {
    occurrences_by_offset_.pop_back();
    occurrences_by_offset_.push_front(0);
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string pattern;
  std::string text;
  std::cin >> pattern;
  std::cin >> text;

  WildcardMatcher matcher(pattern, '?');
  for (size_t i = 0; i < text.size(); ++i) {
    matcher.Scan(text[i],
                 [&]() { std::cout << i - pattern.size() + 1 << ' '; });
  }
}
