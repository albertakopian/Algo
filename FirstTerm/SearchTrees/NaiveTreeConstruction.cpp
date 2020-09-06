/* 1_2. Обход дерева в порядке pre-order
 * Дано число N < 106 и последовательность целых чисел из [-2^31..2^31] длиной N.
 * Требуется построить бинарное дерево, заданное наивным порядком вставки.
 * Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K,
 * то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
 * Выведите элементы в порядке pre-order (сверху вниз).
 * Рекурсия запрещена.
 */

#include <iostream>
#include <stack>
#include <vector>

template <typename T>
struct Node {
    Node(T some_data) : data(some_data) {
        left_child = nullptr;
        right_child = nullptr;
    }

    T data;
    Node* left_child;
    Node* right_child;
};

// Добавление узла в дерево
template <typename T>
void AddData(Node<int>* root, T some_data) {
    Node<T>* current_root = root;

    Node<T>* new_node = new Node<T>(some_data);
    
    // Используется итеративный алгоритм, а не рекурсивный
    while (current_root != new_node) {
        if (some_data < current_root->data) {
            if (current_root->left_child == nullptr)
                current_root->left_child = new_node;

            current_root = current_root->left_child;
        } else {
            if (current_root->right_child == nullptr)
                current_root->right_child = new_node;

            current_root = current_root->right_child;
        }
    }
}

// Обход дерева с вершиной root в порядке pre-order
template <typename  T>
void PreOrder(Node<T>* root, std::vector<T>& answer) {
    std::stack<Node<T>*> queue;
    queue.push(root);

    Node<T>* current_root;
    while (!queue.empty()) {
        current_root = queue.top();
        queue.pop();

        answer.push_back(current_root->data);
        
        // Сначала в стек добавляется правый потомок (если существует), а затем левый (если существует)
        // Это нужно для того, чтобы сначала обрабабатывались левые поддеревья
        if (current_root->right_child != nullptr)
            queue.push(current_root->right_child);
        if (current_root->left_child != nullptr)
            queue.push(current_root->left_child);
        
        // Удаляем узел, поскольку он больше использован не будет
        delete current_root;
    }
}

int main() {
    int N;
    std::cin >> N;

    int data;
    std::cin >> data;
    Node<int>* root = new Node<int>(data);

    for (int i = 1; i < N; ++i) {
        std::cin >> data;

        AddData(root, data);
    }

    std::vector<int> answer;
    PreOrder(root, answer);

    for (int i = 0; i < answer.size(); ++i)
        std::cout << answer[i] << ' ';

    return 0;
}
