#include <cmath>
#include <iomanip>
#include <iostream>


/*
	Даны два отрезка в пространстве (x1, y1, z1) - (x2, y2, z2) и (x3, y3, z3) - (x4, y4, z4). Найдите расстояние между отрезками.

	Формат ввода
	Заданы целые x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4. Координаты по модулю не превосходят 1000.

	Формат вывода
	Выведите искомую величину c точностью не менее 6 знаков после десятичной точки.
*/

const double eps = 1e-12;

struct Point {
  Point(double x = 0, double y = 0, double z = 0) : x_(x), y_(y), z_(z) {}

  double x_ = 0;
  double y_ = 0;
  double z_ = 0;
};

bool operator==(const Point &p1, const Point &p2) {
  return p1.x_ == p2.x_ && p1.y_ == p2.y_ && p1.z_ == p2.z_;
}

std::istream &operator>>(std::istream &in, Point &point) {
  return in >> point.x_ >> point.y_ >> point.z_;
}

class Vector {
 public:
  Vector(const Point &from, const Point &to)
      : x_(to.x_ - from.x_), y_(to.y_ - from.y_), z_(to.z_ - from.z_) {}

  double Length() const { return std::hypot(x_, y_, z_); }

  Vector operator/(double number) {
    Vector tmp = *this;
    tmp.x_ /= number;
    tmp.y_ /= number;
    tmp.z_ /= number;
    return tmp;
  }

  double x_;
  double y_;
  double z_;
};

double Distance(const Point &p1, const Point &p2) {
  return Vector(p1, p2).Length();
}

Point operator+(const Point &p, const Vector &v) {
  Point tmp = p;
  tmp.x_ += v.x_;
  tmp.y_ += v.y_;
  tmp.z_ += v.z_;
  return tmp;
}

Point operator-(const Point &p, const Vector &v) {
  Point tmp = p;
  tmp.x_ -= v.x_;
  tmp.y_ -= v.y_;
  tmp.z_ -= v.z_;
  return tmp;
}

class Segment {
 public:
  Segment(const Point &p1, const Point &p2) : a_(p1), b_(p2) {}

  Point a_;
  Point b_;
};

template <typename Fun>
double OptimizeConvexFunction(const Point &left, const Point &right,
                              Fun function) {
  Point copy_left = left;
  Point copy_right = right;
  while (Distance(copy_left, copy_right) > eps) {
    Point mid1 = copy_left + Vector(copy_left, copy_right) / 3;
    Point mid2 = copy_right - Vector(copy_left, copy_right) / 3;
    if (function(mid1) > function(mid2)) {
      copy_left = mid1;
    } else {
      copy_right = mid2;
    }
  }
  return function(copy_left);
}

double Distance(const Point &p, const Segment &s) {
  return OptimizeConvexFunction(
      s.a_, s.b_, [p](const Point &mid) { return Distance(p, mid); });
}

double Distance(const Segment &s1, const Segment &s2) {
  return OptimizeConvexFunction(
      s1.a_, s1.b_, [s2](const Point &p) { return Distance(p, s2); });
}

int main() {
  Point point_1;
  Point point_2;
  Point point_3;
  Point point_4;
  std::cin >> point_1 >> point_2 >> point_3 >> point_4;
  const int accuracy = 30;
  std::cout << std::setprecision(accuracy)
            << Distance(Segment(point_1, point_2), Segment(point_3, point_4));
  return 0;
}

