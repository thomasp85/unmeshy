#include "bsp.h"
#include <vector>
#include "geometry.h"

void bsp::build_tree(std::vector<triangle> list) {
  if (list.size() == 0) return;
  auto iter = list.begin();
  partition = *iter;
  triangles.push_back(*iter++);
  std::vector<triangle> front_list, back_list;
  while (iter != list.end()) {
    triangle tri = *iter++;

    switch (partition.classify_triangle(tri)) {
    case COINCIDENT:
      triangles.push_back(tri);
      break;
    case IN_BACK_OF:
      back_list.push_back(tri);
      break;
    case IN_FRONT_OF:
      front_list.push_back(tri);
      break;
    case SPAN: {
      cut_tri split = partition.split_triangle(tri);
      front_list.push_back(split.front);
      back_list.push_back(split.back);
      if (split.last_is_front) {
        front_list.push_back(split.extra);
      } else {
        back_list.push_back(split.extra);
      }
      break;
    }
    default:
      break;
    }
  }
  if (!front_list.empty()) {
    front = std::unique_ptr<bsp>(new bsp);
    front->build_tree(front_list);
  }
  if (!back_list.empty()) {
    back = std::unique_ptr<bsp>(new bsp);
    back->build_tree(back_list);
  }
}

void bsp::add_triangle(const triangle& tri) {
  if (is_leaf()) {
    partition = tri;
    triangles.push_back(tri);
    front = std::unique_ptr<bsp>(new bsp(true));
    back = std::unique_ptr<bsp>(new bsp(false));
  } else {
    switch (partition.classify_triangle(tri)) {
    case COINCIDENT:
      triangles.push_back(tri);
      break;
    case IN_BACK_OF:
      back->add_triangle(tri);
      break;
    case IN_FRONT_OF:
      front->add_triangle(tri);
      break;
    case SPAN: {
      cut_tri split = partition.split_triangle(tri);
      front->add_triangle(split.front);
      back->add_triangle(split.back);
      if (split.last_is_front) {
        front->add_triangle(split.extra);
      } else {
        back->add_triangle(split.extra);
      }
      break;
    }
    default:
      break;
    }
  }
}

void bsp::add_shadow(const point& light, triangle& tri,
                     std::vector<triangle>& new_triangles, bool view,
                     double intensity = 1) {
  if (is_leaf()) {
    if (is_out) {
      add_triangle({light, tri.a(), tri.b()});
      add_triangle({light, tri.b(), tri.c()});
      add_triangle({light, tri.c(), tri.a()});
      if (view) {
        tri.set_visibility(true);
      } else {
        double mod = (light.distance_to(tri.a()) + light.distance_to(tri.b()) + light.distance_to(tri.c())) / 3;
        tri.illuminate(intensity / (mod * mod));
      }
    } else if (view) {
      tri.set_visibility(false);
    }
    new_triangles.push_back(tri);
  } else {
    switch (partition.classify_triangle(tri)) {
    case COINCIDENT:
      triangles.push_back(tri);
      break;
    case IN_BACK_OF:
      back->add_shadow(light, tri, new_triangles, view, intensity);
      break;
    case IN_FRONT_OF:
      front->add_shadow(light, tri, new_triangles, view, intensity);
      break;
    case SPAN: {
      cut_tri split = partition.split_triangle(tri);
      front->add_shadow(light, split.front, new_triangles, view, intensity);
      back->add_shadow(light, split.back, new_triangles, view, intensity);
      if (split.last_is_front) {
        front->add_shadow(light, split.extra, new_triangles, view, intensity);
      } else {
        back->add_shadow(light, split.extra, new_triangles, view, intensity);
      }
      break;
    }
    default:
      break;
    }
  }
}

void bsp::cut_by_shadows(bsp& shadow_bsp, const point& light, bool view,
                         double intensity) {
  if (triangles.size() == 0) return;

  double result = partition.classify_point(light);
  if (result < 0) {
    if (back != nullptr) {
      back->cut_by_shadows(shadow_bsp, light, view, intensity);
    }
  } else {
    if (front != nullptr) {
      front->cut_by_shadows(shadow_bsp, light, view, intensity);
    }
  }

  if (result != 0) {
    auto iter = triangles.begin();
    std::vector<triangle> new_triangles;
    while (iter != triangles.end()) {
      if ((*iter).normal().dot(light - (*iter)[0]) >= 0) {
        shadow_bsp.add_shadow(light, *iter, new_triangles, view, intensity);
      } else {
        new_triangles.push_back(*iter);
      }
      iter++;
    }
    triangles.swap(new_triangles);
  }

  if (result < 0) {
    if (front != nullptr) {
      front->cut_by_shadows(shadow_bsp, light, view, intensity);
    }
  } else {
    if (back != nullptr) {
      back->cut_by_shadows(shadow_bsp, light, view, intensity);
    }
  }
}

void bsp::shine_light(const point& light, double intensity) {
  bsp shadow_volume(true);
  cut_by_shadows(shadow_volume, light, false, intensity);
}

void bsp::look_from(const point& pos) {
  bsp shadow_volume(true);
  cut_by_shadows(shadow_volume, pos, true, 0);
}

void bsp::near_to_far(const point& light, std::vector<triangle>& sort_list) {
  double result = partition.classify_point(light);
  if (result < 0) {
    if (back != nullptr) {
      back->near_to_far(light, sort_list);
    }
  } else {
    if (front != nullptr) {
      front->near_to_far(light, sort_list);
    }
  }

  for (int i = 0; i < triangles.size(); i++) {
    sort_list.push_back(triangles[i]);
  }

  if (result < 0) {
    if (front != nullptr) {
      front->near_to_far(light, sort_list);
    }
  } else {
    if (back != nullptr) {
      back->near_to_far(light, sort_list);
    }
  }
}
