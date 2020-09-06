/* Задание 4D. LCA
 * Задано дерево с корнем, содержащее (1 ≤ n ≤ 100 000) вершин, пронумерованных от 0 до n-1.
 * Требуется ответить на m (1 ≤ m ≤ 10 000 000) запросов о наименьшем общем предке для пары вершин.
 * Запросы генерируются следующим образом. Заданы числа a1, a2 и числа x, y и z.
 * Числа a3, ..., a2m генерируются следующим образом: ai = (x ⋅ ai-2 + y ⋅ ai-1 + z) mod n.
 * Первый запрос имеет вид (a1, a2). Если ответ на i-1-й запрос равен v, то i-й запрос имеет вид ((a2i-1 + v) mod n, a2i).
 * Для решения задачи можно использовать метод двоичного подъёма.
 */

#include <iostream>
#include <vector>
#include <cmath>

// Структура "вершина"
struct Vertex {
    Vertex() : in(0), out(0) {}

    int in; // Время входа
    int out; // Время выхода
    std::vector<int> children; // Вершины, для которых данная вершина - родитель
};

// Препроцессинг
// Заполняем массив dp: dp[i][j] = вершина, в которую можно попасть из i за 2^j шагов
// Заполняем время входа и выхода для вершин
void Preprocess(int from, int parent, int& t, std::vector<Vertex*>& vertices, std::vector<std::vector<int>>& dp) {
    vertices[from]->in = t++;
    
    dp[from][0] = parent;

    for (int j = 1; j <= std::log2(vertices.size()); ++j) {
        dp[from][j] = dp[dp[from][j - 1]][j - 1];
    }

    for (int to : vertices[from]->children) {
        if (to != parent) {
            Preprocess(to, from, t, vertices, dp);
        }
    }

    vertices[from]->out = t++;
}

// Если условие выполняется, то u находится в поддереве v
bool IsUpper(int v, int u, std::vector<Vertex*>& vertices) {
    return vertices[v]->in <= vertices[u]->in && vertices[v]->out >= vertices[u]->out;
}

// Нахождение наименьшего общего предка
int LCA(int v, int u, std::vector<Vertex*>& vertices, std::vector<std::vector<int>>& dp) {
    if (IsUpper(v, u, vertices)) {
        return v;
    }

    if (IsUpper(u, v, vertices)) {
        return u;
    }

    for (int i = std::log2(vertices.size()); i >= 0; --i)
		if (!IsUpper(dp[v][i], u, vertices))
			v = dp[v][i];

	return dp[v][0];
}

int main() {
    int n, m;
    std::cin >> n >> m;

    // vertices[i] - вершина Vertex с номером i
    std::vector<Vertex*> vertices;
    for (int i = 0; i < n; ++i) {
        Vertex* v = new Vertex();

        vertices.push_back(v);
    }

    int from, to;
    for (int to = 1; to < n; ++to) {
        std::cin >> from;

        vertices[from]->children.push_back(to);
    }

    unsigned long long a1, a2;
    std::cin >> a1 >> a2;

    unsigned long long x, y, z;
    std::cin >> x >> y >> z;

    std::vector<std::vector<int>> dp(n, std::vector<int>(std::log2(n) + 1));

    // В Preprocess мы передаем ссылку на t как на глобальный счётчик времени
    int t = 0;

    Preprocess(0, 0, t, vertices, dp);

    int last_lca = 0;
    unsigned long long lca_sum = 0;
    for (int i = 0; i < m; ++i) {
        last_lca = LCA((a1 + last_lca) % n, a2, vertices, dp);

        lca_sum += last_lca;

        a1 = (a1 * x + a2 * y + z) % n;
        a2 = (a2 * x + a1 * y + z) % n;
    }

    std::cout << lca_sum;

    return 0;
}
