#' Project the vertices in a mesh onto a plane
#'
#' This function cast a ray between a viewpoint and each vertex in a mesh, and
#' sets the coordinate of the vertex to the intersection of the ray and a plane.
#' `project_coords()` works in the same way but simply takes a data.frame with
#' `x`, `y`, and `z` values and projcts these
#'
#' @param mesh A trimesh object
#' @param coords A data.frame with an `x`, `y`, and `z` column
#' @param from A numeric vector with 3 elements giving the point of view
#' @param to A numeric vector with 3 elements giving the point you're looking
#' towards. The projection plane is defined as a plane including `to` and with
#' `to - from` as its normal
#'
#' @return Either a new mesh with the vb reprojected, or a new data.frame with
#' the `x`, `y`, and `z` columns reprojected.
#'
#' @export
project_mesh <- function(mesh, from, to) {
  if (length(from) != 3 || length(to) != 3) {
    stop('`from` and `to` must be vectors of length 3', call. = FALSE)
  }
  from <- as.numeric(from)
  to <- as.numeric(to)
  mesh <- as_trimesh(mesh)
  coords <- project_coords_c(
    mesh$vb[1,], mesh$vb[2,], mesh$vb[3,],
    from[1], from[2], from[3],
    to[1], to[2], to[3]
  )
  mesh$vb[1,] <- coords$x
  mesh$vb[2,] <- coords$y
  mesh$vb[3,] <- coords$z
  mesh
}
#' @rdname project_mesh
#' @export
project_coords <- function(coords, from, to) {
  if (!all('x', 'y', 'z') %in% names(coords)) {
    stop('coords must include an `x`, `y`, and `z` column', call. = FALSE)
  }
  if (length(from) != 3 || length(to) != 3) {
    stop('`from` and `to` must be vectors of length 3', call. = FALSE)
  }
  from <- as.numeric(from)
  to <- as.numeric(to)
  new_coords <- project_coords_c(
    coords$x, coords$y, coords$z,
    from[1], from[2], from[3],
    to[1], to[2], to[3]
  )
  coords$x <- new_coords$x
  coords$y <- new_coords$y
  coords$z <- new_coords$z
  coords
}
