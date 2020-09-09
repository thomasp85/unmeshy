#include "geometry.h"
#include "bsp.h"
#include <vector>
#include <cpp11/doubles.hpp>
#include <cpp11/logicals.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/data_frame.hpp>
#include <cpp11/list.hpp>
#include <cpp11/named_arg.hpp>

using namespace cpp11::literals;

[[cpp11::register]]
cpp11::writable::data_frame render_bsp(
    cpp11::doubles x, cpp11::doubles y, cpp11::doubles z,
    cpp11::doubles xl, cpp11::doubles yl, cpp11::doubles zl,
    cpp11::doubles intensity, double xv, double yv, double zv,
    bool mark_occlusion, bool project, double xp, double yp, double zp) {
  std::vector<triangle> triangles;

  for (int i = 0; i < x.size() / 3; ++i) {
    triangles.push_back({
      point(x[i], y[i], z[i]),
      point(x[i+1], y[i+1], z[i+1]),
      point(x[i+2], y[i+2], z[i+2]),
      i
    });
  }

  bsp tree(triangles);
  for (int i = 0; i < xl.size(); ++i) {
    tree.shine_light(point(xl[i], yl[i], zl[i]), intensity[i]);
  }
  if (mark_occlusion) {
    tree.look_from(point(xv, yv, zv));
  }

  triangles.clear();
  tree.near_to_far(point(xv, yv, zv), triangles);

  if (project) {
    vec3 n = vec3(xp - xv, yp - yv, zp - zv).normalize();
    point v0(xp, yp, zp);
    point l0(xv, yv, zv);
    for (auto it = triangles.begin(); it != triangles.end(); ++it) {
      it->project(l0, n, v0);
    }
  }
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
  cpp11::writable::logicals visible;
  visible.reserve(full_length);

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
    visible.push_back( (Rboolean) it->is_visible());
    visible.push_back( (Rboolean) it->is_visible());
    visible.push_back( (Rboolean) it->is_visible());
  }
  return cpp11::writable::data_frame({
    "x"_nm = x_new,
    "y"_nm = y_new,
    "z"_nm = z_new,
    "id"_nm = id,
    "light"_nm = light,
    "visible"_nm = visible
  });
}
