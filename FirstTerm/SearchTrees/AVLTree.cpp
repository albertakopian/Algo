/* 3_2. Порядковые статистики
 * Дано число N и N строк.
 * Каждая строка содержащит команду добавления или удаления натуральных чисел,
 * а также запрос на получение k-ой порядковой статистики.
 * Команда добавления числа A задается положительным числом A,
 * команда удаления числа A задается отрицательным числом “-A”.
 * Запрос на получение k-ой порядковой статистики задается числом k.
 * Требуемая скорость выполнения запроса - O(log n).
 */

#include <iostream>
#include <stack>

template <class T>
struct Node {
    Node(int some_data) : data(some_data) {
        height = 1;
        element_count = 1;
        left_child = nullptr;
        right_child = nullptr;
    }

    T data;
    int height;
    int element_count;
    Node<T>* left_child;
    Node<T>* right_child;
};

template <class T>
class AVLTree {
public:
    AVLTree() {
        root = nullptr;
    }

    ~AVLTree() {
        std::stack<Node<T>*> queue;
        queue.push(root);

        Node<T>* temp_node;
        while (!queue.empty()) {
            temp_node = queue.top();
            queue.pop();

            if (temp_node != nullptr) {
                queue.push(temp_node->left_child);
                queue.push(temp_node->right_child);
                delete temp_node;
            }
        }
    }

    Node<T>*& get_root();

    int get_height(Node<T>*);
    int get_element_count(Node<T>*);

    void update_height(Node<T>*);
    void update_element_count(Node<T>*);

    Node<T>* rotate_left(Node<T>*);
    Node<T>* rotate_right(Node<T>*);

    int difference(Node<T>*);

    Node<T>* balance(Node<T>*&);

    Node<T>* insert(Node<T>*&, T);

    Node<T>* remove(Node<T>*&, T);

    T get_k_stat(Node<T>*, int);
private:
    Node<T>* root;
};

template <class T>
Node<T>*& AVLTree<T>::get_root() {
    return root;
}

template <class T>
int AVLTree<T>::get_height(Node<T>* node) {
    if (node != nullptr)
        return node->height;
    else
        return 0;
}

template <class T>
int AVLTree<T>::get_element_count(Node<T>* node) {
    if (node != nullptr)
        return node->element_count;
    else
        return 0;
}

template <class T>
void AVLTree<T>::update_height(Node<T>* node) {
    if (node != nullptr)
        node->height = std::max(get_height(node->left_child), get_height(node->right_child)) + 1;
}

template <class T>
void AVLTree<T>::update_element_count(Node<T>* node) {
    if (node != nullptr)
        node->element_count = get_element_count(node->left_child) + get_element_count(node->right_child) + 1;
}

template <class T>
Node<T>* AVLTree<T>::rotate_left(Node<T>* z) {
    Node<T>* y = z->right_child;
    z->right_child = y->left_child;
    y->left_child = z;

    if (z == root)
        root = y;

    update_height(z);
    update_height(y);

    update_element_count(z);
    update_element_count(y);

    return y;
}

template <class T>
Node<T>* AVLTree<T>::rotate_right(Node<T>* z) {
    Node<T>* y = z->left_child;
    z->left_child = y->right_child;
    y->right_child = z;

    if (z == root)
        root = y;

    update_height(z);
    update_height(y);

    update_element_count(z);
    update_element_count(y);

    return y;
}

template <class T>
int AVLTree<T>::difference(Node<T>* node) {
    if (node != nullptr)
        return get_height(node->left_child) - get_height(node->right_child);
    else
        return 0;
}

template <class T>
Node<T>* AVLTree<T>::balance(Node<T>*& node) {
    if (node == nullptr)
        return nullptr;

    update_height(node);
    update_element_count(node);

    int delta = difference(node);

    if (delta == 2) {
        if (difference(node->left_child) < 0)
            node->left_child = rotate_left(node->left_child);

        return rotate_right(node);
    } else if (delta == -2) {
        if (difference(node->right_child) > 0)
            node->right_child = rotate_right(node->right_child);

        return rotate_left(node);
    }

    return node;
}

template <class T>
Node<T>* AVLTree<T>::insert(Node<T>*& root_node, T some_data) {
    if (root != nullptr) {
        if (root_node == nullptr) {
            auto* new_node = new Node<T>(some_data);
            return new_node;
        }

        if (some_data < root_node->data)
            root_node->left_child = insert(root_node->left_child, some_data);
        else
            root_node->right_child = insert(root_node->right_child, some_data);

    } else {
        auto* new_node = new Node<T>(some_data);
        root = new_node;
        return new_node;
    }

    return balance(root_node);
}

template <class T>
Node<T>* AVLTree<T>::remove(Node<T>*& root_node, T some_data) {
    if (root_node != nullptr) {
        if (some_data < root_node->data)
            root_node->left_child = remove(root_node->left_child, some_data);
        else if (root_node->data < some_data)
            root_node->right_child = remove(root_node->right_child, some_data);
        else if (root_node->left_child != nullptr && root_node->right_child != nullptr) {
            Node<T>* current_node = root_node->right_child;
            while (current_node->left_child != nullptr)
                current_node = current_node->left_child;

            root_node->data = current_node->data;

            root_node->right_child = remove(root_node->right_child, current_node->data);
        } else {
            Node<T>* temp_node;
            if (root_node->left_child != nullptr) {
                temp_node = root_node->left_child;
                delete root_node;
                root_node = temp_node;
            } else {
                temp_node = root_node->right_child;
                delete root_node;
                root_node = temp_node;
            }
        }
    } else
        return nullptr;

    return balance(root_node);
}

template <class T>
T AVLTree<T>::get_k_stat(Node<T>* root_node, int k) {
    int root_stat = get_element_count(root_node->left_child);

    if (root_stat != k) {
        if (k < root_stat)
            return get_k_stat(root_node->left_child, k);
        else {
            return get_k_stat(root_node->right_child, k - root_stat - 1);
        }
    } else {
        return root_node->data;
    }
}

int main() {
    int N;
    std::cin >> N;
    auto* tree = new AVLTree<int>();

    int some_data, k_stat;
    for (int i = 0; i < N; ++i) {
        std::cin >> some_data >> k_stat;

        if (some_data > 0)
            tree->insert(tree->get_root(), some_data);
        else
            tree->remove(tree->get_root(), abs(some_data));

        std::cout << tree->get_k_stat(tree->get_root(), k_stat) << ' ';
    }

    delete tree;

    return 0;
}
