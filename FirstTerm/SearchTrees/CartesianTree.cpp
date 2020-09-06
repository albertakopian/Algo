/* 2_1. Декартово дерево. Разница глубин
 * Дано число N < 10^6 и последовательность пар целых чисел из [-231, 231] длиной N.
 * Построить декартово дерево из N узлов, характеризующихся парами чисел (Xi, Yi).
 * Каждая пара чисел (Xi, Yi) определяет ключ Xi и приоритет Yi в декартовом дереве.
 * Добавление узла в декартово дерево выполняйте следующим образом:
 * При добавлении узла (x, y) выполняйте спуск по ключу до узла P с меньшим приоритетом.
 * Затем разбейте найденное поддерево по ключу x так, чтобы в первом поддереве все ключи меньше x,
 * а во втором больше или равны x. Получившиеся два дерева сделайте дочерними для нового узла (x, y).
 * Новый узел вставьте на место узла P.
 * Построить также наивное дерево поиска по ключам Xi.
 * Т.е., при добавлении очередного числа K в наивное дерево с корнем root,
 * если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
 * Вычислить разницу глубин наивного дерева поиска и декартового дерева.
 * Разница может быть отрицательна, необходимо вывести модуль разности.
 */

#include <iostream>
#include <stack>

template <class T>
struct TreapNode {
    TreapNode(T some_data, int priority) : data(some_data), priority(priority) {
        left_child = nullptr;
        right_child = nullptr;
    }

    T data;
    int priority;
    TreapNode* left_child;
    TreapNode* right_child;
};

template <class T>
class Treap {
public:
    Treap(TreapNode<T>* root) : root(root) {}

    ~Treap();

    void split(TreapNode<T>*, int, TreapNode<T>*&, TreapNode<T>*&);

    TreapNode<T>* merge(TreapNode<T>*, TreapNode<T>*);

    void add(TreapNode<T>*);

    int get_max_depth(TreapNode<T>*, int);

    TreapNode<T>* get_root();
private:
    TreapNode<T>* root;
};

template <class T>
void Treap<T>::split(TreapNode<T>* node, int key, TreapNode<T>*& left_treap, TreapNode<T>*& right_treap) {
    if (node == nullptr) {
        left_treap = nullptr;
        right_treap = nullptr;
    } else if (node->data <= key) {
        split(node->right_child, key, node->right_child, right_treap);
        left_treap = node;
    } else {
        split(node->left_child, key, left_treap, node->left_child);
        right_treap = node;
    }
}

template <class T>
TreapNode<T>* Treap<T>::merge(TreapNode<T> *left_treap, TreapNode<T> *right_treap) {
    if (left_treap == nullptr || right_treap == nullptr) {
        return (left_treap == nullptr ? right_treap : left_treap);
    }

    if (left_treap->priority > right_treap->priority) {
        left_treap->right_child = merge(left_treap->right_child, right_treap);
        return left_treap;
    }

    right_treap->left_child = merge(left_treap, right_treap->left_child);
    return right_treap;
}

template <class T>
void Treap<T>::add(TreapNode<T>* node) {
    TreapNode<T>* left_treap = nullptr;
    TreapNode<T>* right_treap = nullptr;

    split(root, node->data, left_treap, right_treap);

    root = merge(merge(left_treap, node), right_treap);
}

template <class T>
TreapNode<T>* Treap<T>::get_root() {
    return root;
}

template <class T>
int Treap<T>::get_max_depth(TreapNode<T>* node, int curr_depth) {
    if (node == nullptr)
        return curr_depth - 1;

    return std::max(get_max_depth(node->left_child, curr_depth + 1), get_max_depth(node->right_child, curr_depth + 1));
}

template <class T>
Treap<T>::~Treap() {
    std::stack<TreapNode<T>*> queue;
    queue.push(root);

    TreapNode<T>* current_root;
    while (!queue.empty()) {
        current_root = queue.top();
        queue.pop();

        if (current_root->right_child != nullptr)
            queue.push(current_root->right_child);
        if (current_root->left_child != nullptr)
            queue.push(current_root->left_child);

        delete current_root;
    }
}

template <class T>
struct BSTNode {
    BSTNode(T some_data) : data(some_data) {
            left_child = nullptr;
            right_child = nullptr;
    }

    T data;
    BSTNode* left_child;
    BSTNode* right_child;
};

template <class T>
class BinarySearchTree {
public:
    BinarySearchTree(BSTNode<T>* root) : root(root) {}

    ~BinarySearchTree();

    void add_data(T some_data);

    int get_max_depth(BSTNode<T>*, int);
private:
    BSTNode<T>* root;
};

template <class T>
void BinarySearchTree<T>::add_data(T some_data) {
    BSTNode<T>* current_root = root;

    BSTNode<T>* new_node = new BSTNode<T>(some_data);

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

template <class T>
int BinarySearchTree<T>::get_max_depth(BSTNode<T>* node, int curr_depth) {
    if (node == nullptr)
        return curr_depth - 1;

    return std::max(get_max_depth(node->left_child, curr_depth + 1), get_max_depth(node->right_child, curr_depth + 1));
}

template <class T>
BinarySearchTree<T>::~BinarySearchTree() {
    std::stack<BSTNode<T>*> queue;
    queue.push(root);

    BSTNode<T>* current_root;
    while (!queue.empty()) {
        current_root = queue.top();
        queue.pop();

        if (current_root->right_child != nullptr)
            queue.push(current_root->right_child);
        if (current_root->left_child != nullptr)
            queue.push(current_root->left_child);

        delete current_root;
    }
}

int main() {
    int N;
    std::cin >> N;

    int data, priority;
    std::cin >> data >> priority;
    auto* tree_root = new BSTNode<int>(data);
    auto* tree = new BinarySearchTree<int>(tree_root);

    auto* treap_root = new TreapNode<int>(data, priority);
    auto* treap = new Treap<int>(treap_root);


    for (int i = 1; i < N; ++i) {
        std::cin >> data >> priority;
        tree->add_data(data);

        auto* treap_node = new TreapNode<int>(data, priority);
        treap->add(treap_node);
    }

    treap->get_max_depth(treap_root, 0);

    int tree_depth = tree->get_max_depth(tree_root, 0);
    int treap_depth = treap->get_max_depth(treap->get_root(), 0);

    std::cout << abs(tree_depth - treap_depth);

    delete tree;
    delete treap;

    return 0;
}
