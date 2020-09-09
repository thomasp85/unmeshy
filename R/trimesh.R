new_trimesh <- function(vertices, triangles, vertice_info = NULL, triangle_info = NULL) {
  trimesh <- list(vb = vertices, it = triangles, primitivetype = 'triangle',
                  material = list(), normals = NULL, texcoords = NULL,
                  vb_info = vertice_info, it_info = triangle_info)
  class(trimesh) <- c('trimesh', 'mesh3d', 'shape3d')
  trimesh
}

trimesh_from_triangles <- function(x, y, z, ...) {
  triangle_info <- data.frame(...)
  if (length(x) != length(y) || length(x) != length(z)) {
    stop("Coordinates must all have the same length", call. = FALSE)
  }
  if (length(x) %% 3 != 0) {
    stop("Number of vertices must be divisible by 3", call. = FALSE)
  }
  if (nrow(triangle_info) != 0 || nrow(triangle_info) != length(x)/3) {
    stop("Triangle information must match the number of triangles", call. = FALSE)
  }
  mesh <- join_triangles(x, y, z)
  new_trimesh(mesh$vertices, mesh$triangles, triangle_info = triangle_info)
}
