#include "geometry.h"

#include <cpp11/doubles.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/list.hpp>
#include <cpp11/named_arg.hpp>
#include <unordered_map>

using namespace cpp11::literals;

[[cpp11::register]]
cpp11::list join_triangles(cpp11::doubles x, cpp11::doubles y, cpp11::doubles z) {
  std::unordered_map<point, int> vertice_map;
  cpp11::writable::doubles vertices;
  vertices.reserve(x.size() * 4);
  cpp11::writable::integers triangles;
  triangles.reserve(x.size() * 3);

  int j = 0;
  for (size_t i = 0; i < x.size(); ++i) {
    point vert = {x[i], y[i], z[i]};
    auto it = vertice_map.find(vert);
    if (it == vertice_map.end()) {
      vertices.push_back(vert.x);
      vertices.push_back(vert.y);
      vertices.push_back(vert.z);
      vertices.push_back(1.0);
      j = vertices.size() / 4;
      vertice_map[vert] = j;
    } else {
      j = it->second;
    }
    triangles.push_back(j);
  }
  vertices.resize(vertices.size());
  triangles.resize(triangles.size());

  vertices.attr("class") = {"matrix", "array"};
  vertices.attr("dim") = cpp11::writable::integers({4, (int) vertices.size() / 4});
  triangles.attr("class") = {"matrix", "array"};
  triangles.attr("dim") = cpp11::writable::integers({3, (int) triangles.size() / 3});

  return cpp11::writable::list({
    "vertices"_nm = vertices,
    "triangles"_nm = triangles
  });
}
