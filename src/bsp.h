#pragma once

#include <vector>
#include <memory>

#include "geometry.h"

class bsp {
private:
  plane partition;
  std::vector<triangle> triangles;
  std::unique_ptr<bsp> front;
  std::unique_ptr<bsp> back;
  bool is_out;

  void add_triangle(const triangle& tri);
  void add_shadow(const point& light, triangle& tri, std::vector<triangle>& new_triangles, bool view, double intensity);
  void cut_by_shadows(bsp& shadow_bsp, const point& light, bool view, double intensity);
public:
  enum PositionType {
    COINCIDENT,
    IN_BACK_OF,
    IN_FRONT_OF,
    SPAN
  };
  bsp(bool is_out = false) : is_out(is_out) {}
  bsp(std::vector<triangle> list, bool is_out = false) :
    is_out(is_out) {
    build_tree(list);
  }
  ~bsp() {}
  void build_tree(std::vector<triangle> list);
  bool is_leaf() { return (front == nullptr && back == nullptr); }
  void shine_light(const point& light, double intensity = 1);
  void look_from(const point& pos);
  void near_to_far(const point& light, std::vector<triangle>& sort_list);
};
