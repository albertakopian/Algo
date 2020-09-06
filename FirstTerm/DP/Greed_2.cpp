/*
В город N приехал цирк с командой атлетов.
Они хотят удивить горожан города N — выстроить из своих тел
башню максимальной высоты. Башня — это цепочка атлетов,
первый стоит на земле, второй стоит у него на плечах,
третий стоит на плечах у второго и т.д.
Каждый атлет характеризуется силой si (kg) и массой mi (kg).
Сила — это максимальная масса, которую атлет способен держать у себя на плечах.
К сожалению ни один из атлетов не умеет программировать,
так как всю жизнь они занимались физической подготовкой,
и у них не было времени на изучение языков программирования.
Помогите им, напишите программу, которая определит максимальную высоту башни,
которую они могут составить. Известно, что если атлет тяжелее, то он и сильнее:
если mi>mj, то si > sj. Атлеты равной массы могут иметь различную силу.
Вход содержит только пары целых чисел — массу и силу атлетов.
Число атлетов 1 ≤ n ≤ 100000. Масса и сила являются
положительными целыми числами меньше, чем 2000000.
Выход должен содержать натуральное число — максимальную высоту башни.
*/

#include <algorithm>
#include <iostream>
#include <vector>

class AthletesTeam {
 public:
  void AddAthlete(const size_t weight, const size_t strength) {
    athletes_.push_back(Athlete(weight, strength));
  }

  size_t GetMaxTowerHeight() {
    std::sort(athletes_.begin(), athletes_.end(), [](const Athlete& lhs,
                                                     const Athlete& rhs) {
      return lhs.weight_ < rhs.weight_ ||
             lhs.weight_ == rhs.weight_ && lhs.strength_ < rhs.strength_;
    });
    size_t max_tower_height = 0;
    size_t total_current_weight = 0;
    for (int i = 0; i < athletes_.size(); ++i) {
      if (total_current_weight <= athletes_[i].strength_) {
        ++max_tower_height;
        total_current_weight += athletes_[i].weight_;
      }
    }
    return max_tower_height;
  }

 private:
  struct Athlete {
    explicit Athlete(const size_t weight, const size_t strength)
        : weight_{weight}, strength_{strength} {}

    size_t weight_ = 0;
    size_t strength_ = 0;
  };

  std::vector<Athlete> athletes_;
};

int main() {
  AthletesTeam athletes_team;
  size_t weight = 0;
  size_t strength = 0;
  while (std::cin >> weight >> strength) {
    athletes_team.AddAthlete(weight, strength);
  }
  std::cout << athletes_team.GetMaxTowerHeight() << std::endl;
  return 0;
}