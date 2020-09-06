/*
Даны два выпуклых многоугольника на плоскости. В первом n точек, во втором m.
Определите, пересекаются ли они за O(n + m).
Указание. Используйте сумму Минковского.
Формат ввода
Первая строка содержит число n точек первого многоугольника.
Затем идут n строчек с координатами точек первого многоугольника
по часовой стрелке (координаты — действительные числа, double).
Второй многоугольник задается аналогично. n, m ≤ 80000.
Формат вывода
YES/NO.
*/

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

class Point2D {
public:
  Point2D(double x = 0, double y = 0) : x_(x), y_(y) {}

  double& X() { return x_; }
  double& Y() { return y_; }
  double X() const { return x_; }
  double Y() const { return y_; }

protected:
  double x_;
  double y_;
};

bool operator==(const Point2D& p1, const Point2D& p2) {
  return p1.X() == p2.X() && p1.Y() == p2.Y();
}

class Vector2D : public Point2D {
public:
  Vector2D(double x, double y) : Point2D(x, y) {}

  Vector2D(const Point2D& from, const Point2D& to)
    : Point2D(to.X() - from.X(), to.Y() - from.Y()) {
  }

  double Length() const { return std::hypot(x_, y_); }
};

Point2D& operator+=(Point2D& p, const Vector2D& v) {
  p.X() += v.X();
  p.Y() += v.Y();
  return p;
}

Point2D operator+(const Point2D& p, const Vector2D& v) {
  Point2D tmp = p;
  return tmp += v;
}

double operator^(const Vector2D& v1, const Vector2D& v2) {
  return v1.X() * v2.Y() - v2.X() * v1.Y();
}

class Polygon2D {
public:
  explicit Polygon2D(const std::vector<Point2D>& points) : points_(points) {}

  Polygon2D operator-() const {
    Polygon2D tmp = *this;
    for (Point2D& point : tmp.points_) {
      point.X() = -point.X();
      point.Y() = -point.Y();
    }
    return tmp;
  }

  Polygon2D& operator+=(const Polygon2D& other) {
    std::vector<Point2D> sum;
    if (points_.size() == 1) {
      sum = other.points_;
      for (Point2D& p : sum) {
        p += Vector2D(Point2D(0, 0), points_[0]);
      }
      points_ = std::move(sum);
      return *this;
    } else if (other.points_.size() == 1) {
      sum = points_;
      for (Point2D& p : sum) {
        p += Vector2D(Point2D(0, 0), other.points_[0]);
      }
      points_ = std::move(sum);
      return *this;
    }
    int index_of_min1 = 0;
    for (int i = 1; i < points_.size(); ++i) {
      if (points_[i].Y() < points_[index_of_min1].Y() ||
          points_[i].Y() == points_[index_of_min1].Y() &&
          points_[i].X() < points_[index_of_min1].X()) {
        index_of_min1 = i;
      }
    }
    int index_of_min2 = 0;
    for (int i = 1; i < other.points_.size(); ++i) {
      if (other.points_[i].Y() < other.points_[index_of_min2].Y() ||
          other.points_[i].Y() == other.points_[index_of_min2].Y() &&
          other.points_[i].X() < other.points_[index_of_min2].X()) {
        index_of_min2 = i;
      }
    }
    int j = 0;
    int k = 0;
    while (j < points_.size() || k < other.points_.size()) {
      sum.push_back(points_[index_of_min1] +
        Vector2D(Point2D(0, 0), other.points_[index_of_min2]));
      if ((Vector2D(points_[index_of_min1], points_[(index_of_min1 + 1) % points_.size()]) ^
           Vector2D(other.points_[index_of_min2], other.points_[(index_of_min2 + 1) % other.points_.size()]))
          > 0) {
        index_of_min1 = (index_of_min1 + 1) % points_.size();
        ++j;
      } else if ((Vector2D(points_[index_of_min1], points_[(index_of_min1 + 1) % points_.size()]) ^
                  Vector2D(other.points_[index_of_min2], other.points_[(index_of_min2 + 1) % other.points_.size()]))
                 < 0) {
        index_of_min2 = (index_of_min2 + 1) % other.points_.size();
        ++k;
      } else {
        index_of_min1 = (index_of_min1 + 1) % points_.size();
        ++j;
        index_of_min2 = (index_of_min2 + 1) % other.points_.size();
        ++k;
      }
    }
    points_ = std::move(sum);
    return *this;
  }

  bool ContainsPoint(const Point2D& point) const {
    if (points_.size() == 1) {
      return points_[0] == point;
    }
    for (int i = 1; i < points_.size(); ++i) {
      if ((Vector2D(points_[i - 1], point) ^
           Vector2D(points_[i - 1], points_[i])) > 0) {
        return false;
      }
    }
    int min_x = points_[0].X();
    int max_x = points_[0].X();
    int min_y = points_[0].Y();
    int max_y = points_[0].Y();
    for (const Point2D& p : points_) {
      if (p.X() < min_x) {
        min_x = p.X();
      }
      if (p.X() > max_x) {
        max_x = p.X();
      }
      if (p.Y() < min_y) {
        min_y = p.Y();
      }
      if (p.Y() > max_y) {
        max_y = p.Y();
      }
    }
    return point.X() >= min_x && point.X() <= max_x &&
           point.Y() >= min_y && point.Y() <= max_y;
  }

private:
  std::vector<Point2D> points_;
};

Polygon2D operator+(const Polygon2D& lhs, const Polygon2D& rhs) {
  Polygon2D tmp = lhs;
  return tmp += rhs;
}

int main() {
  int point_count = 0;
  std::cin >> point_count;
  std::vector<Point2D> points1(point_count);
  for (Point2D& point : points1) {
    std::cin >> point.X() >> point.Y();
  }
  std::cin >> point_count;
  std::vector<Point2D> points2(point_count);
  for (Point2D& point : points2) {
    std::cin >> point.X() >> point.Y();
  }
  std::reverse(points1.begin(), points1.end());
  std::reverse(points2.begin(), points2.end());
  if ((Polygon2D(points1) + -Polygon2D(points2)).ContainsPoint(Point2D(0, 0))) {
    std::cout << "YES" << std::endl;
  } else {
    std::cout << "NO" << std::endl;
  }
  return 0;
}
