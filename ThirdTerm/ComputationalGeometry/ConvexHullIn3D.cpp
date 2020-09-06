#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
	Даны n точек в пространстве. Никакие 4 точки не лежат в одной плоскости. Найдите выпуклую оболочку этих точек за O(n*log(n)).

	Формат ввода
	Первая строка содержит число m - количество тестов. В последующих строках описаны сами тесты. Каждый тест начинается со строки, содержащей n (n ≤ 1000) - число точек. Далее, в n строках даны по 		три числа - координаты точек. Все координаты целые, не превосходят по модулю 500.

	Формат вывода
	Для каждого теста выведите следующее. В первую строку выведите количество граней m. Далее в последующие m строк выведите описание граней: количество точек грани (=3) и номера точек в исходном 	множестве. Точки нумеруются в том же порядке, в котором они даны во входном файле. Точки в пределах грани должны быть отсортированы в порядке против часовой стрелки относительно внешней нормали к 		грани. Первая точка – точка с минимальным номером. Порядок граней лексикографический.
*/

using std::pow;
using std::sqrt;

const double eps = 0.0000000001;

bool IsEqual(double a, double b) { return std::abs(a - b) < eps; }

struct Point3 {
  double x_, y_, z_;

  double DistanceToPoint(const Point3& another) const {
    return sqrt(pow(another.x_ - this->x_, 2) + pow(another.y_ - this->y_, 2) +
                pow(another.z_ - this->z_, 2));
  }

  bool operator==(const Point3& another) const {
    double distance = DistanceToPoint(another);
    return IsEqual(distance, 0.0);
  }

  bool operator!=(Point3 another) const { return !(*this == another); }

  Point3& operator+=(const Point3& another) {
    this->x_ += another.x_;
    this->y_ += another.y_;
    this->z_ += another.z_;
    return *this;
  }

  Point3& operator-=(const Point3& another) {
    this->x_ -= another.x_;
    this->y_ -= another.y_;
    this->z_ -= another.z_;
    return *this;
  }

  Point3 operator+(const Point3& another) const {
    Point3 result = *this;
    result += another;
    return result;
  }

  Point3 operator-(const Point3& another) const {
    Point3 result = *this;
    result -= another;
    return result;
  }

  class Hash {
   public:
    size_t operator()(const Point3& point) const {
      return double_hash_(point.x_) + double_hash_(point.y_) +
             double_hash_(point.z_);
    }

   private:
    std::hash<double> double_hash_;
  };
};

struct Edge {
  Point3 begin_, end_;
  double Length() const { return begin_.DistanceToPoint(end_); }
  Point3 GetVector() const { return end_ - begin_; }
};

class SquareMatrix {
 public:
  SquareMatrix(size_t size, std::vector<std::vector<double>>&& row_columns)
      : row_columns_(std::move(row_columns)), size_(size) {}

  explicit SquareMatrix(size_t size) : size_(size) {
    row_columns_.resize(size);
    for (size_t row = 0; row < size; ++row) {
      row_columns_[row].resize(size, 0);
    }
  }

  std::vector<double>& operator[](size_t row) { return row_columns_[row]; };

  double Determinant() const {
    if (size_ == 3) {
      return Determinant3();
    }
    if (size_ == 1) {
      return row_columns_[0][0];
    }

    double result = 0.0;
    for (size_t column = 0; column < size_; ++column) {
      std::vector<std::vector<double>> submatrix_row_columns;
      submatrix_row_columns.resize(size_ - 1);
      for (size_t row = 0; row < size_ - 1; ++row) {
        submatrix_row_columns[row].resize(size_ - 1);
      }
      for (size_t subrow = 1; subrow < size_; ++subrow) {
        for (size_t subcolumn = 0; subcolumn < column; ++subcolumn) {
          submatrix_row_columns[subrow - 1][subcolumn] =
              row_columns_[subrow][subcolumn];
        }

        for (size_t subcolumn = column + 1; subcolumn < size_; ++subcolumn) {
          submatrix_row_columns[subrow - 1][subcolumn - 1] =
              row_columns_[subrow][subcolumn];
        }
      }
      SquareMatrix submatrix(size_ - 1, std::move(submatrix_row_columns));
      result += submatrix.Determinant() * row_columns_[0][column] *
                (1 - 2 * (static_cast<int>(column) % 2));
    }
    return result;
  }

 private:
  std::vector<std::vector<double>> row_columns_;
  size_t size_;

  double Determinant3() const {
    return (row_columns_[0][0] * row_columns_[1][1] * row_columns_[2][2]) +
           (row_columns_[1][0] * row_columns_[2][1] * row_columns_[0][2]) +
           (row_columns_[0][1] * row_columns_[1][2] * row_columns_[2][0]) -
           (row_columns_[0][2] * row_columns_[1][1] * row_columns_[2][0]) -
           (row_columns_[0][0] * row_columns_[1][2] * row_columns_[2][1]) -
           (row_columns_[1][0] * row_columns_[0][1] * row_columns_[2][2]);
  }
};

struct Polyhedron {
 public:
  struct Face {  // Пример крайне неэффективной реализации(
    Face(Point3 point_1, Point3 point_2, Point3 point_3)
        : points_{point_1, point_2, point_3} {}
    Edge GetEdge(size_t edge_number) const {
      return Edge{points_[edge_number],
                  points_[(edge_number + 1) % points_.size()]};
    }
    const Point3* GetThirdPoint(size_t edge_number) const {
      return &points_[(edge_number + 2) % points_.size()];
    }
    std::vector<Point3> points_;

    double Area() const {
      double distance_0_1 = points_[0].DistanceToPoint(points_[1]);
      double distance_0_2 = points_[0].DistanceToPoint(points_[2]);
      double distance_1_2 = points_[1].DistanceToPoint(points_[2]);
      double half_perimeter = (distance_0_1 + distance_0_2 + distance_1_2) / 2;

      double area = std::sqrt(static_cast<double>(
          half_perimeter * (half_perimeter - distance_0_1) *
          (half_perimeter - distance_0_2) * (half_perimeter - distance_1_2)));
      return area;
    }

    class Hash {
     public:
      size_t operator()(const Polyhedron::Face& face) const {
        size_t result = 0;
        for (const auto& point : face.points_) {
          result += point_hash_(point);
        }
        return result;
      }

     private:
      Point3::Hash point_hash_;
    };

    bool operator==(const Face& another) const {
      return (another.points_[0] == this->points_[0] &&
              another.points_[1] == this->points_[1] &&
              another.points_[2] == this->points_[2]) ||
             (another.points_[0] == this->points_[1] &&
              another.points_[1] == this->points_[2] &&
              another.points_[2] == this->points_[0]) ||
             (another.points_[0] == this->points_[2] &&
              another.points_[1] == this->points_[0] &&
              another.points_[2] == this->points_[1]);
    }
  };

  std::vector<Face> faces_;
};

int CalculateOrientationTripleVectors(const Point3& point_1,
                                      const Point3& point_2,
                                      const Point3& point_3) {
  SquareMatrix vectors_matrix(3);
  vectors_matrix[0] = {point_1.x_, point_1.y_, point_1.z_};
  vectors_matrix[1] = {point_2.x_, point_2.y_, point_2.z_};
  vectors_matrix[2] = {point_3.x_, point_3.y_, point_3.z_};
  double determinant = vectors_matrix.Determinant();
  if (determinant > 0) {
    return 1;
  }
  if (determinant < 0) {
    return -1;
  }
  return 0;
}

class ConvexHullBuilder {
 public:
  Polyhedron Build(const std::vector<Point3>& points) {
    points_ = &points;
    stack_edges_ = {};
    set_added_faces_ = {};
    Polyhedron polyhedron;

    Polyhedron::Face first_face = FindFirstFace();
    OrientFace(first_face);
    polyhedron.faces_.push_back(first_face);
    set_added_faces_.emplace(first_face);
    for (size_t edge_number = 0; edge_number < 3; ++edge_number) {
      AddEdgeToStack(0, edge_number);
    }

    while (!stack_edges_.empty()) {
      const auto& pair_face_number_edge = stack_edges_.top();
      auto& face_number = pair_face_number_edge.first;
      auto& edge = pair_face_number_edge.second;
      stack_edges_.pop();
      Point3 new_point =
          FindNextPoint(polyhedron.faces_[face_number].GetEdge(edge),
                        polyhedron.faces_[face_number].GetThirdPoint(edge));
      Polyhedron::Face new_face(
          polyhedron.faces_[face_number].GetEdge(edge).end_,
          polyhedron.faces_[face_number].GetEdge(edge).begin_,
          new_point);  // ориентация сохраняется правильной в том случае, если
                       // она была правильно посчитана для первой грани
      if (set_added_faces_.find(new_face) == set_added_faces_.end()) {
        polyhedron.faces_.push_back(new_face);
        set_added_faces_.emplace(new_face);
        AddEdgeToStack(polyhedron.faces_.size() - 1, 1);
        AddEdgeToStack(polyhedron.faces_.size() - 1, 2);
      }
    }
    return polyhedron;
  }

 private:
  const std::vector<Point3>* points_ = nullptr;
  std::stack<std::pair<size_t, size_t>>
      stack_edges_{};  // first - face_number, second - edge_number
  std::unordered_set<Polyhedron::Face, Polyhedron::Face::Hash>
      set_added_faces_{};  // множество уже добавленных граней

  void AddEdgeToStack(size_t face_number, size_t edge_number) {
    stack_edges_.push(std::make_pair(face_number, edge_number));
  }

  // Ориентация грани в соответствии с условием задачи
  void OrientFace(Polyhedron::Face& face) const {
    Point3 estimated_direction = face.GetEdge(0).GetVector();
    Point3 second_direction = face.GetEdge(2).GetVector();
    int orientation = 0;
    size_t linearly_independent_vector_number = 0;

    // Пока не найдена еще одна точка из множества такая, что тройка векторов
    // линейно независима:
    do {
      Point3 third_temp_direction =
          (*points_)[linearly_independent_vector_number] - face.points_[0];
      orientation = CalculateOrientationTripleVectors(
          estimated_direction, second_direction, third_temp_direction);
      ++linearly_independent_vector_number;
    } while (orientation == 0);

    if (orientation == -1) {
      std::swap(face.points_[0], face.points_[1]);
    }
  }

  Polyhedron::Face FindFirstFace() const {
    Point3 first_point = FindFirstPoint();
    Point3 second_point = FindSecondPoint(first_point);
    Point3 third_point = FindThirdPoint(first_point, second_point);

    return Polyhedron::Face(first_point, second_point, third_point);
  }

  Point3 FindNextPoint(const Edge& edge,
                       const Point3* third_point = nullptr) const {
    auto min_point = std::min_element(points_->begin(), points_->end(),
                                      NextPointsComparator(edge, third_point));
    return *min_point;
  }

  Point3 FindThirdPoint(const Point3& point_1, const Point3& point_2) const {
    return FindNextPoint(Edge{point_1, point_2});
  }

  Point3 FindSecondPoint(const Point3& first_point) const {
    Edge temp_edge{
        Point3{first_point.x_ + 1, first_point.y_ + 1, first_point.z_},
        first_point};
    return FindNextPoint(temp_edge);
  }

  Point3 FindFirstPoint() const {
    return *std::min_element(points_->begin(), points_->end(),
                             FirstPointsComparator());
  }
  class FirstPointsComparator {
   public:
    bool operator()(const Point3& point_1, const Point3& point_2) const {
      return std::make_tuple(point_1.z_, point_1.x_, point_1.y_) <
             std::make_tuple(point_2.z_, point_2.x_, point_2.y_);
    }
  };

  class NextPointsComparator {
   public:
    explicit NextPointsComparator(const Edge& edge, const Point3* third_point)
        : edge_(edge),
          edge_direction_(edge.end_ - edge.begin_),
          third_point_(third_point) {}
    bool operator()(const Point3& point_1, const Point3 point_2) const {
      if (point_1 == edge_.begin_ || point_1 == edge_.end_) {
        return false;
      }
      if (point_2 == edge_.begin_ || point_2 == edge_.end_) {
        return true;
      }
      if (third_point_ != nullptr) {
        if (point_1 == *third_point_) {
          return false;
        }
        if (point_2 == *third_point_) {
          return true;
        }
      }
      return CalculateOrientationTripleVectors(edge_.begin_ - point_1,
                                               edge_.end_ - point_2,
                                               edge_direction_) == 1;
    }

   private:
    const Edge& edge_;
    Point3 edge_direction_;
    const Point3* third_point_;
  };
};

void SortFace(std::vector<size_t>& points) {
  while (*std::min_element(points.begin(), points.end()) != points[0]) {
    size_t temp_number = points[2];
    points[2] = points[1];
    points[1] = points[0];
    points[0] = temp_number;
  }
}

void TestRun(ConvexHullBuilder& convex_hull_builder) {
  size_t points_number;
  std::cin >> points_number;
  std::vector<Point3> points;
  std::unordered_map<Point3, size_t, Point3::Hash>
      points_values_numbers_map{};  // Потом происходит восстановление номера
                                    // точки для правильного вывода
  for (size_t point_number = 0; point_number < points_number; ++point_number) {
    double x;
    double y;
    double z;
    std::cin >> x >> y >> z;
    points.push_back(Point3{x, y, z});
    points_values_numbers_map.emplace(Point3{x, y, z}, point_number);
  }

  Polyhedron convex_hull = convex_hull_builder.Build(points);

  std::vector<std::vector<size_t>> faces_points_numbers_vector(
      convex_hull.faces_.size());
  for (size_t face = 0; face < convex_hull.faces_.size(); ++face) {
    for (auto& point : convex_hull.faces_[face].points_) {
      faces_points_numbers_vector[face].push_back(
          points_values_numbers_map[point]);
    }
    SortFace(faces_points_numbers_vector[face]);
  }
  std::sort(faces_points_numbers_vector.begin(),
            faces_points_numbers_vector.end());

  std::cout << faces_points_numbers_vector.size() << std::endl;
  for (auto& face : faces_points_numbers_vector) {
    std::cout << 3 << ' ';
    for (size_t point = 0; point < 3; ++point) {
      std::cout << face[point] << ' ';
    }
    std::cout << std::endl;
  }
}

int main() {
  size_t tests_number;
  std::cin >> tests_number;
  ConvexHullBuilder convex_hull_builder;

  for (size_t test_number = 0; test_number < tests_number; ++test_number) {
    TestRun(convex_hull_builder);
  }

  return 0;
}
