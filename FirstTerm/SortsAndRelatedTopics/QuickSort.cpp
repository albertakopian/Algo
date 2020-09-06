/* 6. Быстрейшая сортировка
 * Дан массив целых чисел в диапазоне [0..10^9].
 * Размер массива кратен 10 и ограничен сверху значением 2.5 * 106 элементов.
 * Все значения массива являются элементами псевдо-рандомной последовательности.
 * Необходимо отсортировать элементы массива за минимальное время и вывести каждый десятый элемент отсортированной последовательности.
 * Реализуйте сортировку, основанную на QuickSort.
 * Минимальный набор оптимизаций, который необходимо реализовать:
 * 1. Оптимизация ввода/вывода
 * 2. Оптимизация выбора опорного элемента
 * 3. Оптимизация Partition
 * 4. Написать без рекурсии
 * 5. Оптимизация концевой рекурсии
 */

#include <cstdio>
#include <vector>
#include <random>
#include <queue>

void InsertionSort(std::vector<int>& array, int start, int end) {
    int j;
    int temp;
    for (int i = start; i < end + 1; ++i) {
        temp = array[i];

        for (j = i; j > start && array[j - 1] > temp; j--)
            array[j] = array[j - 1];

        array[j] = temp;
    }
}

// Практические данные показали, что случайный выбор опорного элемента работает быстрее,
// чем поиск медианы и дальнейшее её использование в качестве опорного элемента
int RandomPartition(std::vector<int>& array, int start, int end) {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<> distribution(start, end);

    // Выбираем случайный элемент массива в качестве опорного
    int pivot = distribution(generator);

    std::swap(array[start], array[pivot]);
    pivot = start;

    int i, j;
    i = j = end;

    while (j > pivot) {
        if (array[j] > array[pivot]) {
            std::swap(array[i], array[j]);
            --i;
        }

        --j;
    }

    std::swap(array[pivot], array[i]);
    pivot = i;

    return pivot;
}

// Итеративная реализация быстрой сортировки. Когда размер рассматриваемого
// отрезка мал, используется сортировка вставками.
void QuickSort(std::vector<int>& array, int start, int end) {
    std::queue<std::pair<int, int>> sort_queue;

    sort_queue.push(std::make_pair(start, end));

    std::pair<int, int> curr_pair;
    while (!sort_queue.empty()) {
        curr_pair = sort_queue.front();
        sort_queue.pop();

        start = curr_pair.first;
        end = curr_pair.second;

        if (end - start + 1 < 63) {
            InsertionSort(array, start, end);
        } else {
            int pivot = RandomPartition(array, start, end);

            if (pivot > start)
                sort_queue.push(std::make_pair(start, pivot - 1));

            if (pivot < end)
                sort_queue.push(std::make_pair(pivot + 1, end));
        }
    }
}

int main() {
    std::vector<int> array;
    int curr_number;
    while (scanf("%d", &curr_number) == 1)
        array.push_back(curr_number);

    QuickSort(array, 0, array.size() - 1);

    for (int i = 9; i < array.size(); i += 10)
        printf("%d ", array[i]);

    return 0;
}
