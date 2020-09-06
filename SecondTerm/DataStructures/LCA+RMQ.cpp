#include <iostream>
#include <vector>
#include <algorithm>

class RMQ {
public:
    RMQ(int n) {
        number_of_elements = n;
        log = std::vector<int>(n + 1);
        a = std::vector<int>(n);
        height = fill_log();
        st = std::vector<std::vector<int>>(n, std::vector<int>(height + 1));
    }

    int fill_log() {
        log[0] = -1;
        for (int i = 1; i <= number_of_elements; ++i) {
            log[i] = log[i / 2] + 1;
        }
        return log[number_of_elements];
    }

    void add_element(int index, int element) {
        a[index] = element;
    }

    void fill_s() {
        for (int i = 0; i < number_of_elements; ++i) {
            st[i][0] = i;
        }
        for (int j = 1; (1 << j) <= number_of_elements; ++j) {
            for (int i = 0; i + (1 << j) <= number_of_elements; ++i) {
                if (a[st[i][j - 1]] < a[st[i + (1 << (j - 1))][j - 1]]) {
                    st[i][j] = st[i][j - 1];
                }
                else {
                    st[i][j] = st[i + (1 << (j - 1))][j - 1];
                }
            }
        }
    }

    int query(int u, int v) {
        if (u > v) {
            std::swap(u, v);
        }
        int k = log[v - u + 1];
        if (a[st[u][k]] < a[st[v - (1 << k) + 1][k]]) {
            return st[u][k];
        }
        return st[v - (1 << k) + 1][k];
    }

private:

    std::vector<int>  log;
    std::vector<std::vector<int>> st;
    std::vector<int> a;
    int number_of_elements = 0;
    int height = 0;
};

class LCA {
public:
    LCA(int n, std::vector<int>& parents_) : rmq(2 * n - 1) {
        number_of_vertices = n;
        index = std::vector<int>(n);
        g = std::vector<std::vector<int>>(n);
        parents = parents_;
        fill_g();
        dfs(0, 0);
        for (int i = 0; i < dist.size(); ++i) {
            rmq.add_element(i, dist[i]);
        }
        rmq.fill_s();
    }

    void fill_g() {
        for (int i = 0; i < number_of_vertices - 1; ++i) {
            g[parents[i]].push_back(i + 1);
        }
    }

    int find_lca(int u, int v) {
        return vtx[rmq.query(index[u], index[v])];
    }

    void dfs(int curr_node, int curr_dist) {
        dist.push_back(curr_dist);
        index[curr_node] = dist.size() - 1;
        vtx.push_back(curr_node);
        for (int son : g[curr_node]) {
            dfs(son, curr_dist + 1);
            dist.push_back(curr_dist);
            vtx.push_back(curr_node);
        }
    }


private:
    std::vector<int> index;
    std::vector<int> vtx;
    std::vector<int> dist;
    std::vector<int> parents;
    std::vector<std::vector<int>> g;
    RMQ rmq;
    int number_of_vertices = 0;
};

int main() {

    int number_of_vertices, number_of_requests;
    std::cin >> number_of_vertices >> number_of_requests;
    std::vector<int> parents(number_of_vertices - 1);
    for (int i = 0; i < number_of_vertices - 1; ++i) {
        int parent = 0;
        std::cin >> parent;
        parents[i] = parent;
    }
    LCA a(number_of_vertices, parents);
    long long a1, a2, x, y, z;
    std::cin >> a1 >> a2 >> x >> y >> z;
    int sum = 0;
    int v = 0;
    for (int i = 0; i < number_of_requests; ++i) {
        v = a.find_lca((a1 + v) % number_of_vertices, a2);
        sum += v;
        a1 = (x * a1 + y * a2 + z) % number_of_vertices;
        a2 = (x * a2 + y * a1 + z) % number_of_vertices;
    }
    std::cout << sum;
    return 0;
}
