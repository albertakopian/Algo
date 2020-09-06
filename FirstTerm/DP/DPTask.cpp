/*
Дано N кубиков. Требуется определить каким количеством
способов можно выстроить из этих кубиков пирамиду.
На вход подается количество кубиков N.
Каждый вышележащий слой пирамиды должен быть не больше нижележащего.
N ≤ 200.
*/

#include <iostream>
#include <vector>

class Pyramid {
 public:
  Pyramid(const size_t cubes_count = 0) : cubes_count_{cubes_count} {
    configurations_count_.resize(cubes_count + 1);
    configurations_count_[0].resize(cubes_count + 1, 1);
    for (int i = 1; i <= cubes_count; ++i) {
      configurations_count_[i].resize(cubes_count + 1, 0);
    }
  }

  long long GetConfigurationsCount() {
    return GetConfigurationsCountRecurive(cubes_count_, cubes_count_);
  }

 private:
  size_t cubes_count_ = 0;

  std::vector<std::vector<long long>> configurations_count_;

  long long GetConfigurationsCountRecurive(size_t cubes_count,
                                           size_t max_base_width) {
    if (configurations_count_[cubes_count][max_base_width] == 0) {
      for (int i = 1; i <= max_base_width; ++i) {
        if (i <= cubes_count) {
          configurations_count_[cubes_count][max_base_width] +=
              GetConfigurationsCountRecurive(cubes_count - i, i);
        }
      }
    }
    return configurations_count_[cubes_count][max_base_width];
  }
};

int main() {
  size_t cubes_count = 0;
  std::cin >> cubes_count;
  Pyramid pyramid(cubes_count);
  std::cout << pyramid.GetConfigurationsCount() << std::endl;
  return 0;
}