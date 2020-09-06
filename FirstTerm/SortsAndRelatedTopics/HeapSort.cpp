/* 2_4. Закраска прямой в один слой
 * На числовой прямой окрасили N отрезков.
 * Известны координаты левого и правого концов каждого отрезка [Li, Ri].
 * Найти сумму длин частей числовой прямой, окрашенных ровно в один слой.
 * N ≤ 10000. Li, Ri — целые числа в диапазоне [0, 10^9].
 */

#include <iostream>

// Левый или правый край отрезка и его координата
struct Edge {
    int x;
    bool is_left;

    Edge(int x, bool is_left) : x(x), is_left(is_left) {

    }
};

// min-heap
class Heap {
public:
    Heap(int size) : buffer_size(size), real_size(0) {
        buffer = new Edge*[buffer_size];
    }

    ~Heap() {
        delete[] buffer;
    }

    void grow() {
        Edge** temp_buffer = new Edge*[buffer_size * 2];

        for (int i = 0; i < buffer_size; i++)
            temp_buffer[i] = buffer[i];

        delete[] buffer;

        buffer = temp_buffer;
        buffer_size *= 2;
    }

    void compress() {
        Edge** temp_buffer = new Edge*[buffer_size / 3];

        for (int i = 0; i < buffer_size / 3; i++)
            temp_buffer[i] = buffer[i];

        delete[] buffer;

        buffer = temp_buffer;
        buffer_size /= 3;
    }

    void sift_down(int parent) {
        int new_parent = -1, left_child, right_child;

        while (parent != new_parent) {
            left_child = 2 * parent + 1;
            right_child = left_child + 1;

            new_parent = parent;

            if (left_child < real_size && buffer[left_child]->x < buffer[parent]->x)
                new_parent = left_child;

            if (right_child < real_size && buffer[right_child]->x < buffer[new_parent]->x)
                new_parent = right_child;

            std::swap(buffer[parent], buffer[new_parent]);
            std::swap(parent, new_parent);
        }
    }

    void sift_up(int child) {
        int parent;

        while (child) {
            parent = (child - 1) / 2;

            if (buffer[child]->x < buffer[parent]->x) {
                std::swap(buffer[child], buffer[parent]);
                child = parent;
            } else {
                child = 0;
            }
        }
    }

    void add(Edge* element) {
        if (real_size == buffer_size)
            grow();

        buffer[real_size] = element;
        sift_up(real_size);

        real_size++;
    }

    Edge* get_edge() {
        if (real_size < buffer_size / 3)
            compress();

        Edge* result = buffer[0];
        real_size--;

        buffer[0] = buffer[real_size];
        sift_down(0);

        return result;
    }

private:
    Edge** buffer;
    int buffer_size;
    int real_size;
};

int main() {
    int N;
    std::cin >> N;

    Heap* edges = new Heap(1);

    int L_i, R_i;
    for (int i = 0; i < N; i++) {
        std::cin >> L_i >> R_i;

        Edge* edge_left = new Edge(L_i, true);
        Edge* edge_right = new Edge(R_i, false);

        edges->add(edge_left);
        edges->add(edge_right);
    }

    Edge* temp_edge;
    int colored = 0, last_x = 0, opened_left = 0;
    for (int i = 0; i < N * 2; i++) {
        temp_edge = edges->get_edge();

        if (opened_left == 1)
            colored += (temp_edge->x - last_x);

        if (temp_edge->is_left)
            opened_left++;
        else
            opened_left--;

        last_x = temp_edge->x;

        delete temp_edge;
    }

    std::cout << colored;

    delete edges;

    return 0;
}

