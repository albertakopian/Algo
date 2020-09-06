/* Задание 3B. Максимальный поток в ориентированном графе
 * Задан ориентированный граф, каждое ребро которого обладает целочисленной пропускной способностью.
 * Найдите максимальный поток из вершины с номером 1 в вершину с номером n.
 */

#include <vector>
#include <iostream>
#include <queue>
#include <climits>

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

int main(int argc, char const *argv[]) {
    int n, m;
    std::cin >> n >> m;

    FlowNetwork* network = new FlowNetwork(n + 1);
    network->SetSource(1);
    network->SetSink(n);

    int from, to, capacity;
    for (int i = 0; i < m; ++i) {
        std::cin >> from >> to >> capacity;

        network->AddArc(from, to, capacity);
    }

    int max_flow = Dinic(network);

    std::cout << max_flow;

    return 0;
}
