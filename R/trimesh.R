new_trimesh <- function(vertices, triangles, vertice_info = NULL, triangle_info = NULL) {
  if (is.null(vertice_info) || nrow(vertice_info) == 0) {
    vertice_info <- tibble(.rows = ncol(vertices))
  }
  if (is.null(triangle_info) || nrow(triangle_info) == 0) {
    triangle_info <- tibble(.rows = ncol(triangles))
  }
  trimesh <- list(vb = vertices, it = triangles, primitivetype = 'triangle',
                  material = list(), normals = NULL, texcoords = NULL,
                  vb_info = vertice_info, it_info = triangle_info)
  class(trimesh) <- c('trimesh', 'mesh3d', 'shape3d')
  trimesh
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
#' @export
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
