#pragma once

#include <math.h>
#include <cmath>
#include <vector>

struct vec3 {
  double x, y, z;

  vec3() { x = 0; y = 0; z = 0; }
  vec3(double x, double y, double z) : x(x), y(y), z(z) {}
  vec3(const vec3& copy) : x(copy.x), y(copy.y), z(copy.z) {}
  double length() const { return sqrt(x * x + y * y + z * z); }
  vec3 normalize() const {
    double l = length();
    return vec3(x / l, y / l, z / l);
  }
  vec3 operator* (const double& s) { return vec3(x * s, y * s, z * s); }
  double dot(const vec3& b) const { return x * b.x + y * b.y + z * b.z; }
  vec3 cross(const vec3& b) const { return vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};

struct point : vec3 {
  point() {}
  ~point() {}
  point(double x, double y, double z) : vec3(x, y, z) {}
  point(const point& copy) : vec3(copy) {}
  vec3 operator- (const point& b) const { return vec3(x - b.x, y - b.y, z - b.z); }
  point operator+ (const vec3& v) const { return point(x + v.x, y + v.y, z + v.z); }
  double distance_to(const point& p2) const {
    double xd = x - p2.x;
    double yd = y - p2.y;
    double zd = z - p2.z;
    return std::sqrt(xd*xd + yd*yd + zd*zd);
  }
};

class triangle {
private:
  point _a;
  point _b;
  point _c;
  vec3 _n;
  int _id = 0;
  double _light = 0;
  bool _visible = false;

public:

  triangle() {}
  triangle(const point& a, const point& b, const point& c, int id = 0, int light = 0, bool visible = true) :
    _a(a),
    _b(b),
    _c(c),
    _n(((b - a).cross(c - b)).normalize()),
    _id(id),
    _light(light),
    _visible(visible) {}

  triangle(const triangle& t2) :
    _a(t2._a),
    _b(t2._b),
    _c(t2._c),
    _n(t2._n),
    _id(t2._id),
    _light(t2._light),
    _visible(t2._visible) {}

  ~triangle() {}

  void operator=(const triangle& t2) {
    _a = t2._a;
    _b = t2._b;
    _c = t2._c;
    _id = t2._id;
    _light = t2._light;
    _n = t2._n;
    _visible = t2._visible;
  }

  const point& a() const { return _a; }
  const point& b() const { return _b; }
  const point& c() const { return _c; }
  const point& operator[](int index) const {
    switch (index % 3) {
    case 0: return a();
    case 1: return b();
    case 2: return c();
    default: return a();
    }
  }
  const vec3& normal() const { return _n; }
  int id() const { return _id; }
  double light() const { return _light; }
  void illuminate(double light = 1) { _light += light; }
  bool is_visible() { return _visible; }
  void set_visibility(bool visible = true) { _visible = visible; }
  void project(const point& from, const vec3& plane_n, const point& plane_p) {
    vec3 proj_dir;
    double ln, d;
    proj_dir = _a - from;
    ln = proj_dir.dot(plane_n);
    if (ln == 0) ln = 1e-6;
    d = (plane_p - from).dot(plane_n) / ln;
    if (d < 0) {
      set_visibility(false);
    }
    _a = from + proj_dir * d;

    proj_dir = _b - from;
    ln = proj_dir.dot(plane_n);
    if (ln == 0) ln = 1e-6;
    d = (plane_p - from).dot(plane_n) / ln;
    if (d < 0) {
      set_visibility(false);
    }
    _b = from + proj_dir * d;

    proj_dir = _c - from;
    ln = proj_dir.dot(plane_n);
    if (ln == 0) ln = 1e-6;
    d = (plane_p - from).dot(plane_n) / ln;
    if (d < 0) {
      set_visibility(false);
    }
    _c = from + proj_dir * d;
  }
};

struct cut_tri {
  triangle front;
  triangle back;
  triangle extra;

  bool last_is_front;
};

class plane {
private:
  vec3 n;
  double d;

public:
  plane() : n(0, 0, 0), d(0) {}
  plane(const vec3& n, double d) : n(n), d(d) {}
  plane(const triangle& tri) : n(tri.normal()), d(-tri.a().dot(n)) {}

  double classify_point(const point& p) const {
    return n.x * p.x + n.y * p.y + n.z * p.z + d;
  }
  int classify_triangle(const triangle& tri) const {
    double res_a = classify_point(tri.a());
    double res_b = classify_point(tri.b());
    double res_c = classify_point(tri.c());

    if (res_a == 0 && res_b == 0 && res_c == 0) {
      return 0;
    } else if (res_a <= 0 && res_b <= 0 && res_c <= 0) {
      return 1;
    } else if (res_a >= 0 && res_b >= 0 && res_c >= 0) {
      return 2;
    }
    return 3;
  }
  cut_tri split_triangle(const triangle& tri) const {
    point pt_a = tri.a();
    point pt_b = tri.b();
    point pt_c = tri.c();
    cut_tri result;

    int side_a = classify_point(pt_a);
    int side_b = classify_point(pt_b);
    int side_c = classify_point(pt_c);
    vec3 v = pt_b - pt_a;
    point ab = pt_a + (v * (-side_a/n.dot(v)));
    v = pt_c - pt_b;
    point bc = pt_b + (v * (-side_b/n.dot(v)));
    v = pt_a - pt_c;
    point ca = pt_c + (v * (-side_c/n.dot(v)));
    result.last_is_front = side_a * side_b * side_c < 0;

    if (side_a >= 0) {
      if (side_b >= 0) {
        result.front = {pt_a, pt_b, bc};
        result.extra = {pt_a, bc, ca};
        result.back = {bc, pt_c, ca};
      } else if (side_c >= 0) {
        result.front = {pt_c, pt_a, ab};
        result.extra = {pt_c, ab, bc};
        result.back = {ab, pt_b, bc};
      } else {
        result.front = {pt_a, ab, ca};
        result.extra = {pt_b, ca, ab};
        result.back = {pt_b, pt_c, ca};
      }
    } else {
      if (side_b <= 0) {
        result.back = {pt_a, pt_b, bc};
        result.extra = {pt_a, bc, ca};
        result.front = {bc, pt_c, ca};
      } else if (side_c <= 0) {
        result.back = {pt_c, pt_a, ab};
        result.extra = {pt_c, ab, bc};
        result.front = {ab, pt_b, bc};
      } else {
        result.back = {pt_a, ab, ca};
        result.extra = {pt_b, ca, ab};
        result.front = {pt_b, pt_c, ca};
      }
    }

    return result;
  };
};
