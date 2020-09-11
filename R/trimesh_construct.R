#' Create a trimesh object
#'
#' These functions helps in creating trimesh objects. A trimesh is a subclass of
#' mesh3d as defined in the rgl package. It is exclusively using triangles
#' (unsurprisingly) and adds two additional elements: `it_info` which is a
#' data.frame with information about the triangles in the mesh, and `vb_info`
#' which is a data.frame with information about the vertices. Since trimesh is
#' all about vector graphics it doesn't concern itself with material, texcoords,
#' and normals — converting from a mesh3d will loose that information. trimesh
#' objects can be constructed from either mesh3d object or data frames
#' containing an `x`, `y`, and `z` column (remaining columns will go to the
#' `it_info` element), or potentially from raw data giving vertice coordinates
#' and triangle vertex indexes.
#'
#' @param mesh A trimesh or an object convertible to one
#' @param vertices A 4-row matrix giving the coordinates of vertices in 3D space
#' (last row is filled with 1's by convention). Each vertex correspond to a
#' column in the matrix
#' @param triangles A 3-row matrix giving the indexes of the three vertices that
#' corresponds to the triangle. Each triangle corresponds to a column in the
#' matrix
#' @param vertex_info A data.frame giving additional information about each
#' vertex in the `vertices` matrix.
#' @param triangle_info A data.frame giving addtional information about each
#' triangle given in the `triangles` matrix
#' @param ... ignored
#'
#' @name trimesh_construct
#' @rdname trimesh_construct
#'
NULL

#' @rdname trimesh_construct
#' @export
new_trimesh <- function(vertices, triangles, vertex_info = NULL, triangle_info = NULL) {
  if (is.null(vertex_info) || nrow(vertex_info) == 0) {
    vertex_info <- tibble(.rows = ncol(vertices))
  }
  if (is.null(triangle_info) || nrow(triangle_info) == 0) {
    triangle_info <- tibble(.rows = ncol(triangles))
  }
  mode(triangles) <- 'integer'
  trimesh <- list(vb = vertices, it = triangles, primitivetype = 'triangle',
                  material = list(), normals = NULL, texcoords = NULL,
                  vb_info = vertex_info, it_info = triangle_info)
  class(trimesh) <- c('trimesh', 'mesh3d', 'shape3d')
  trimesh
}
#' @rdname trimesh_construct
#' @export
is_trimesh <- function(mesh) {
  inherits(mesh, 'trimesh')
}
#' @rdname trimesh_construct
#' @export
as_trimesh <- function(mesh, ...) {
  UseMethod('as_trimesh')
}
#' @export
as_trimesh.trimesh <- function(mesh, ...) {
  mesh
}
#' @export
as_trimesh.mesh3d <- function(mesh, ...) {
  trimesh_from_mesh3d(mesh)
}
#' @export
as_trimesh.data.frame <- function(mesh, ...) {
  if (!all(c('x', 'y', 'z') %in% names(mesh))) {
    stop('data.frame must have an `x`, `y`, and `z` column to be converted to a trimesh', call. = FALSE)
  }
  trimesh_from_triangles(
    mesh$x, mesh$y, mesh$z,
    mesh[, !names(mesh) %in% c('x', 'y', 'z'), drop = FALSE]
  )
}
#' @export
#' @importFrom tibble as_tibble trunc_mat
print.trimesh <- function(x, ...) {
  v <- format(x$vb, digits = 2)
  t <- x$it
  tri <- vapply(seq_len(ncol(t)), function(i) paste0(
    '{', v[1,t[1,i]], ';', v[2,t[1,i]], ';', v[3,t[1,i]], '}, ',
    '{', v[1,t[2,i]], ';', v[2,t[2,i]], ';', v[3,t[2,i]], '}, ',
    '{', v[1,t[3,i]], ';', v[2,t[3,i]], ';', v[3,t[3,i]], '}'), character(1))
  tri <- as_tibble(cbind(data.frame(Vertices = tri), x$it_info))
  tri <- trunc_mat(tri)
  tri$summary <- c("A trimesh" = paste0(ncol(x$it), ' triangles and ', ncol(x$vb), ' unique vertices'))
  print(tri)
  invisible(x)
}

#' @importFrom tibble tibble
trimesh_from_triangles <- function(x, y, z, ...) {
  triangle_info <- tibble(...)
  if (ncol(triangle_info) == 0) triangle_info <- NULL
  if (length(x) != length(y) || length(x) != length(z)) {
    stop("Coordinates must all have the same length", call. = FALSE)
  }
  if (length(x) %% 3 != 0) {
    stop("Number of vertices must be divisible by 3", call. = FALSE)
  }
  if (!is.null(triangle_info) && nrow(triangle_info) != length(x)/3) {
    stop("Triangle information must match the number of triangles", call. = FALSE)
  }
  mesh <- join_triangles(x, y, z)
  new_trimesh(mesh$vertices, mesh$triangles, triangle_info = triangle_info)
}

trimesh_from_mesh3d <- function(mesh) {
  if (mesh$primitivetype == 'quad') {
    tri <- matrix(rbind(
      mesh$it[c(1, 2, 3), ],
      mesh$it[c(3, 4, 1), ]
    ), nrow = 3)
  } else if (mesh$primitivetype == 'triangle') {
    tri <- mesh$it
  } else {
    stop('Cannot convert a ', mesh$primitivetype, ' mesh3d object to a trimesh', call. = FALSE)
  }
  new_trimesh(mesh$vb, tri)
}
