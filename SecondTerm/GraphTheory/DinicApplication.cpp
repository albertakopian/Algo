/* Задание 3C. Чай
 * В одном из отделов крупной организации работает n человек.
 * Как практически все сотрудники этой организации, они любят пить чай в перерывах между работой.
 * При этом они достаточно дисциплинированы и делают в день ровно один перерыв, во время которого пьют чай.
 * Для того, чтобы этот перерыв был максимально приятным,
 * каждый из сотрудников этого отдела обязательно пьет чай одного из своих любимых сортов.
 * В разные дни сотрудник может пить чай разных сортов. Для удобства пронумеруем сорта чая числами от 1 до m.
 * Недавно сотрудники отдела купили себе большой набор чайных пакетиков,
 * который содержит a1 пакетиков чая сорта номер 1, a2 пакетиков чая сорта номер 2, ..., am пакетиков чая сорта номер m.
 * Теперь они хотят знать, на какое максимальное число дней им может хватить купленного набора так,
 * чтобы в каждый из дней каждому из сотрудников доставался пакетик чая одного из его любимых сортов.
 * Каждый сотрудник отдела пьет в день ровно одну чашку чая, которую заваривает из одного пакетика.
 * При этом пакетики чая не завариваются повторно.
 */

#include <iostream>
#include <vector>
#include <climits>
#include <queue>

struct ArcInfo {
    ArcInfo(int capacity);

    int capacity;
    int flow;
};

class FlowNetwork {
public:
    FlowNetwork(int nodes_count);

    ~FlowNetwork();
    FlowNetwork(const FlowNetwork& flow_network);
    FlowNetwork& operator=(const FlowNetwork& flow_network);

    void AddArc(int from, int to, int capacity);

    void SetSource(int node);
    void SetSink(int node);

    void SetCapacity(int from, int to, int capacity);
    void SetFlow(int from, int to, int flow);

    int GetNodesCount() const;

    int GetSource() const;
    int GetSink() const;

    int GetCapacity(int from, int to) const;
    int GetFlow(int from, int to) const;

private:
    int nodes_count, source, sink;
    std::vector<std::vector<ArcInfo*>> arcs;

    ArcInfo* GetArcInfo(int from, int to);
    ArcInfo GetArcInfoCopy(int from, int to) const;
};

ArcInfo::ArcInfo(int capacity) : capacity(capacity) {
    flow = 0;
}

FlowNetwork::FlowNetwork(int nodes_count) : nodes_count(nodes_count) {
    arcs.resize(GetNodesCount());

    for (int i = 0; i < GetNodesCount(); ++i) {
        for (int j = 0; j < GetNodesCount(); ++j) {
            arcs[i].push_back(new ArcInfo(0));
        }
    }
}

FlowNetwork::FlowNetwork(const FlowNetwork& flow_network) {
    nodes_count = flow_network.GetNodesCount();
    source = flow_network.GetSource();
    sink = flow_network.GetSink();

    arcs.clear();
    arcs.resize(GetNodesCount());

    ArcInfo* arc_info;
    for (int from = 0; from < GetNodesCount(); ++from) {
        for (int to = 0; to < GetNodesCount(); ++to) {
            arc_info = new ArcInfo(flow_network.GetCapacity(from, to));
            arc_info->flow = flow_network.GetFlow(from, to);

            arcs[from].push_back(arc_info);
        }
    }
}

FlowNetwork& FlowNetwork::operator=(const FlowNetwork& flow_network) {
    if (this == &flow_network) {
        return *this;
    }

    nodes_count = flow_network.GetNodesCount();
    source = flow_network.GetSource();
    sink = flow_network.GetSink();

    arcs.clear();
    arcs.resize(GetNodesCount());

    ArcInfo* arc_info;
    for (int from = 0; from < GetNodesCount(); ++from) {
        for (int to = 0; to < GetNodesCount(); ++to) {
            arc_info = new ArcInfo(flow_network.GetCapacity(from, to));
            arc_info->flow = flow_network.GetFlow(from, to);

            arcs[from].push_back(arc_info);
        }
    }

    return *this;
}

void FlowNetwork::AddArc(int from, int to, int capacity) {
    GetArcInfo(from, to)->capacity = capacity;
}

void FlowNetwork::SetSource(int node) {
    source = node;
}

void FlowNetwork::SetSink(int node) {
    sink = node;
}

void FlowNetwork::SetCapacity(int from, int to, int capacity) {
    GetArcInfo(from, to)->capacity = capacity;
}

void FlowNetwork::SetFlow(int from, int to, int flow) {
    GetArcInfo(from, to)->flow = flow;
}

int FlowNetwork::GetNodesCount() const {
    return nodes_count;
}

int FlowNetwork::GetSource() const {
    return source;
}

int FlowNetwork::GetSink() const {
    return sink;
}

int FlowNetwork::GetCapacity(int from, int to) const {
    return GetArcInfoCopy(from, to).capacity;
}

int FlowNetwork::GetFlow(int from, int to) const {
    return GetArcInfoCopy(from, to).flow;
}

ArcInfo* FlowNetwork::GetArcInfo(int from, int to) {
    return arcs[from][to];
}

ArcInfo FlowNetwork::GetArcInfoCopy(int from, int to) const {
    return *arcs[from][to];
}

// Постороение вспомогательной слоистой сети
bool BFS(FlowNetwork* network, std::vector<int>& distance) {
    distance.assign(network->GetNodesCount(), INT32_MAX);
    distance[network->GetSource()] = 0;

    std::queue<int> queue;
    queue.push(network->GetSource());

    int from;
    while (!queue.empty()) {
        from = queue.front();
        queue.pop();

        for (int to = 0; to < network->GetNodesCount(); ++to) {
            if (network->GetFlow(from, to) < network->GetCapacity(from, to) && distance[to] == INT32_MAX) {
                distance[to] = distance[from] + 1;

                queue.push(to);
            }
        }
    }

    return distance[network->GetSink()] != INT32_MAX;
}

// Поиск блокирующего потока
int DFS(FlowNetwork* network, int from, int min_capacity, std::vector<int>& distance, std::vector<int>& ptr) {
    if (from == network->GetSink() || min_capacity == 0) {
        return min_capacity;
    }

    int delta;
    for (int to = ptr[from]; to < network->GetNodesCount(); ++to) {
        if (distance[to] == distance[from] + 1) {
            delta = DFS(network, to, std::min(min_capacity, network->GetCapacity(from, to) - network->GetFlow(from, to)), distance, ptr);

            if (delta != 0) {
                network->SetFlow(from, to, network->GetFlow(from, to) + delta);
                network->SetFlow(to, from, network->GetFlow(to, from) - delta);

                return delta;
            }
        }

        ++ptr[from];
    }

    return 0;
}

// Поиск максимального потока с помощью алгоритма Диница
int Dinic(FlowNetwork* network) {
    int flow;
    int max_flow = 0;
    std::vector<int> ptr;
    std::vector<int> distance;

    while (BFS(network, distance)) {
        ptr.assign(network->GetNodesCount(), 0);

        flow = DFS(network, network->GetSource(), INT32_MAX, distance, ptr);

        while (flow != 0) {
            max_flow += flow;

            flow = DFS(network, network->GetSource(), INT32_MAX, distance, ptr);
        }
    }

    return max_flow;
}

bool BFS(FlowNetwork* network, std::vector<int>& distance) {
    distance.assign(network->GetNodesCount(), INT32_MAX);
    distance[network->GetSource()] = 0;

    std::queue<int> queue;
    queue.push(network->GetSource());

    int from;
    while (!queue.empty()) {
        from = queue.front();
        queue.pop();

        for (int to = 0; to < network->GetNodesCount(); ++to) {
            if (true) {
                if (network->GetFlow(from, to) < network->GetCapacity(from, to) && distance[to] == INT32_MAX) {
                    distance[to] = distance[from] + 1;

                    queue.push(to);
                }
            }
        }
    }

    return distance[network->GetSink()] != INT32_MAX;
}

int DFS(FlowNetwork* network, int from, int min_capacity, std::vector<int>& distance, std::vector<int>& ptr) {
    if (from == network->GetSink() || min_capacity == 0) {
        return min_capacity;
    }

    int delta;
    for (int to = ptr[from]; to < network->GetNodesCount(); ++to) {
        if (distance[to] == distance[from] + 1) {
            delta = DFS(network, to, std::min(min_capacity, network->GetCapacity(from, to) - network->GetFlow(from, to)), distance, ptr);

            if (delta != 0) {
                network->SetFlow(from, to, network->GetFlow(from, to) + delta);
                network->SetFlow(to, from, network->GetFlow(to, from) - delta);

                return delta;
            }
        }

        ++ptr[from];
    }

    return 0;
}

int Dinic(FlowNetwork* network) {
    int flow;
    int max_flow = 0;
    std::vector<int> ptr;
    std::vector<int> distance;

    while (BFS(network, distance)) {
        ptr.assign(network->GetNodesCount(), 0);

        flow = DFS(network, network->GetSource(), INT32_MAX, distance, ptr);

        while (flow != 0) {
            max_flow += flow;

            flow = DFS(network, network->GetSource(), INT32_MAX, distance, ptr);
        }
    }

    return max_flow;
}

void ClearFlows(FlowNetwork* network) {
    for (int from = 0; from < network->GetNodesCount(); ++from) {
        for (int to = 0; to < network->GetNodesCount(); ++to) {
            network->SetFlow(from, to, 0);
        }
    }
}

/* Поиск ответа происходит с помощью бинпоиска
 * Нам нужно найти максимальную пропускную способность ребёр из вершин-сотрудников в сток такую,
 * что при такой пропускной способности эти ребра полностью насыщены (поток == пропускная способность)
 */
int GetMaxTeaParties(FlowNetwork* network, int sorts_count) {
    int max_tea_parties = 0;

    int left = 0, middle, right = INT32_MAX, min_tea_parties;
    while (left < right) {
        min_tea_parties = right;

        middle = (left + right) / 2;

        for (int from = sorts_count + 1; from < network->GetNodesCount() - 1; ++from) {
            network->SetCapacity(from, network->GetSink(), middle);
        }

        // После предыдущей итерации потоки в сети чем-то заполнены, очистим их
        ClearFlows(network);

        // Используем алгоритм Диница для поиска максимального потока
        Dinic(network);

        for (int from = sorts_count + 1; from < network->GetNodesCount() -1; ++from) {
            min_tea_parties = std::min(min_tea_parties, network->GetFlow(from, network->GetSink()));
        }

        if (min_tea_parties < middle) {
            right = middle;
        } else {
            max_tea_parties = min_tea_parties;

            left = middle + 1;
        }
    }

    return max_tea_parties;
}

int main(int argc, char const *argv[]) {
    int n, m;
    std::cin >> n >> m;

    FlowNetwork* network = new FlowNetwork(n + m + 2);
    network->SetSource(0);
    network->SetSink(n + m + 1);

    // Из истока проводим по ребру до каждой из m вершин
    // Ребро отвечает за количество доступных пакетиков каждого из m сортов
    int capacity;
    for (int i = 0; i < m; ++i) {
        std::cin >> capacity;

        network->AddArc(0, i + 1, capacity);
    }

    // Далее из каждой вершины-сорта чая проводим ребра к сотрудникам, которые хотели бы попить такой чай
    int sorts_count, sort;
    for (int i = 0; i < n; ++i) {
        std::cin >> sorts_count;

        for (int j = 0; j < sorts_count; ++j) {
            std::cin >> sort;

            network->AddArc(sort, m + 1 + i, network->GetCapacity(network->GetSource(), sort));
        }

        // Из каждой вершины-сотрудника проводим ребро в сток
        network->AddArc(m + 1 + i, n + m + 1, 0);
    }

    int max_tea_parties = GetMaxTeaParties(network, m);

    std::cout << max_tea_parties;

    return 0;
}
