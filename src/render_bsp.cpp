#include "geometry.h"
#include "bsp.h"
#include <vector>
#include <algorithm>
#include <random>
#include <cpp11/doubles.hpp>
#include <cpp11/logicals.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/data_frame.hpp>
#include <cpp11/list.hpp>
#include <cpp11/named_arg.hpp>

using namespace cpp11::literals;

[[cpp11::register]]
cpp11::writable::data_frame shade_mesh_c(
    cpp11::doubles vert, cpp11::integers tri,
    cpp11::doubles xl, cpp11::doubles yl, cpp11::doubles zl,
    cpp11::doubles intensity) {
  std::vector<triangle> triangles;

  for (int i = 0; i < tri.size() / 3; ++i) {
    triangles.push_back({
      point(vert[tri[i]], vert[tri[i]+1], vert[tri[i]+2]),
      point(vert[tri[i+1]], vert[tri[i+1]+1], vert[tri[i+1]+2]),
      point(vert[tri[i+2]], vert[tri[i+2]+1], vert[tri[i+2]+2]),
      i
    });
  }

  std::shuffle(triangles.begin(), triangles.end(), std::default_random_engine(1));

  bsp tree(triangles);
  for (int i = 0; i < xl.size(); ++i) {
    tree.shine_light(point(xl[i], yl[i], zl[i]), intensity[i]);
  }

  triangles.clear();
  tree.near_to_far(point(tri[0], tri[1], tri[2]), triangles);

  int full_length = triangles.size() * 3;
  cpp11::writable::doubles x_new;
  x_new.reserve(full_length);
  cpp11::writable::doubles y_new;
  y_new.reserve(full_length);
  cpp11::writable::doubles z_new;
  z_new.reserve(full_length);
  cpp11::writable::integers id;
  id.reserve(full_length);
  cpp11::writable::doubles light;
  light.reserve(full_length);

  for (auto it = triangles.begin(); it != triangles.end(); ++it) {
    x_new.push_back(it->a().x);
    y_new.push_back(it->a().y);
    z_new.push_back(it->a().z);
    x_new.push_back(it->b().x);
    y_new.push_back(it->b().y);
    z_new.push_back(it->b().z);
    x_new.push_back(it->c().x);
    y_new.push_back(it->c().y);
    z_new.push_back(it->c().z);
    id.push_back(it->id());
    id.push_back(it->id());
    id.push_back(it->id());
    light.push_back(it->light());
    light.push_back(it->light());
    light.push_back(it->light());
  }
  return cpp11::writable::data_frame({
    "x"_nm = x_new,
    "y"_nm = y_new,
    "z"_nm = z_new,
    "id"_nm = id,
    "light"_nm = light
  });
}

[[cpp11::register]]
cpp11::writable::data_frame occlude_mesh_c(
    cpp11::doubles vert, cpp11::integers tri,
    double xv, double yv, double zv) {
  std::vector<triangle> triangles;

  for (int i = 0; i < tri.size() / 3; ++i) {
    triangles.push_back({
      point(vert[tri[i]], vert[tri[i]+1], vert[tri[i]+2]),
      point(vert[tri[i+1]], vert[tri[i+1]+1], vert[tri[i+1]+2]),
      point(vert[tri[i+2]], vert[tri[i+2]+1], vert[tri[i+2]+2]),
      i
    });
  }

  std::shuffle(triangles.begin(), triangles.end(), std::default_random_engine(1));

  bsp tree(triangles);
  //tree.look_from(point(xv, yv, zv));
//
  //triangles.clear();
  //tree.near_to_far(point(xv, yv, zv), triangles);

  int full_length = triangles.size() * 3;
  cpp11::writable::doubles x_new;
  x_new.reserve(full_length);
  cpp11::writable::doubles y_new;
  y_new.reserve(full_length);
  cpp11::writable::doubles z_new;
  z_new.reserve(full_length);
  cpp11::writable::integers id;
  id.reserve(full_length);
  cpp11::writable::logicals visible;
  visible.reserve(full_length);
  cpp11::writable::logicals back_facing;
  back_facing.reserve(full_length);

  for (auto it = triangles.begin(); it != triangles.end(); ++it) {
    x_new.push_back(it->a().x);
    y_new.push_back(it->a().y);
    z_new.push_back(it->a().z);
    x_new.push_back(it->b().x);
    y_new.push_back(it->b().y);
    z_new.push_back(it->b().z);
    x_new.push_back(it->c().x);
    y_new.push_back(it->c().y);
    z_new.push_back(it->c().z);
    id.push_back(it->id());
    id.push_back(it->id());
    id.push_back(it->id());
    visible.push_back( (Rboolean) it->is_visible());
    visible.push_back( (Rboolean) it->is_visible());
    visible.push_back( (Rboolean) it->is_visible());
    back_facing.push_back( (Rboolean) it->is_back_facing());
    back_facing.push_back( (Rboolean) it->is_back_facing());
    back_facing.push_back( (Rboolean) it->is_back_facing());
  }
  return cpp11::writable::data_frame({
    "x"_nm = x_new,
    "y"_nm = y_new,
    "z"_nm = z_new,
    "id"_nm = id,
    "visible"_nm = visible,
    "back_facing"_nm = back_facing
  });
}

[[cpp11::register]]
cpp11::writable::data_frame project_coords_c(
    cpp11::doubles x, cpp11::doubles y, cpp11::doubles z,
    double xv, double yv, double zv,
    double xp, double yp, double zp) {

  int full_length = x.size();
  cpp11::writable::doubles x_new;
  x_new.reserve(full_length);
  cpp11::writable::doubles y_new;
  y_new.reserve(full_length);
  cpp11::writable::doubles z_new;
  z_new.reserve(full_length);

  vec3 n = vec3(xp - xv, yp - yv, zp - zv).normalize();
  point v0(xp, yp, zp);
  point l0(xv, yv, zv);
  for (size_t i = 0; i < full_length; ++i) {
    point coord(x[i], y[i], z[i]);
    coord.project(l0, n, v0);
    x_new.push_back(coord.x);
    y_new.push_back(coord.y);
    z_new.push_back(coord.z);
  }

  return cpp11::writable::data_frame({
    "x"_nm = x_new,
    "y"_nm = y_new,
    "z"_nm = z_new
  });
}
