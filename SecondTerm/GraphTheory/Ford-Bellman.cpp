#include <iostream>
#include <vector>
#include <algorithm>

/*	Необходимо написать торгового советника для поиска арбитража. 
   	Определение
	Арбитраж - это торговля по цепочке различных валют в надежде заработать на небольших различиях в коэффициентах. Например, есть следующие курсы валют (на 03.05.2015):
	GBP/USD: 0.67
	RUB/GBP: 78.66
	USD/RUB: 0.02
	Имея 1$ и совершив цикл USD->GBP->RUB->USD, получим 1.054$. Таким образом заработав 5.4%.
	Время работы – O(VE).
	Формат входных данных.
	Первая строка содержит число N – количество возможных валют (размер таблицы котировок)
	Далее следует построчное представление таблицы. Диагональные элементы (i, i) пропущены (подразумевается, что курс валюты к себе же 1.0).
	В элементе таблицы (i, j) содержится обменный курс i->j.
	Если обмен в данном направлении не производится, то -1.
	Формат выходных данных.
	Выведите YES, если арбитраж есть, и NO, иначе.*/



struct edge{
    int from;
    int to;
    double wt;
    edge(int from, int to, double wt){
        this->from = from;
        this->to = to;
        this->wt = wt;
    }
};

bool GetAnswer(const int vertices_count,
               const std::vector<edge>& edges){
    
    std::vector<double> d(vertices_count, 1);

    for (int k = 0; k < vertices_count - 1; ++k ){
        for (auto e : edges){
            d[e.to] = std::max(d[e.to], d[e.from] * e.wt);
        }
    }

    for (auto e : edges){
        if (d[e.to] < d[e.from] * e.wt){
            return true;
        }
    }
    return false;
}

int main(){
    int vertices_count = 0;
    std::cin>>vertices_count;
    std::vector<edge> edges;
    for (int i = 0; i < vertices_count; ++i){
        for (int j = 0; j < vertices_count; ++j){
            if (i != j){
                double wt = 0;
                std::cin >> wt;
                if (wt != -1) {
                    edges.emplace_back(i, j, wt);
                }
            }
        }
    }
    if (GetAnswer(vertices_count, edges)){
        std::cout<<"YES";
    } else {
        std::cout<<"NO";
    }
    return 0;
}
