#' Extract and set information from trimesh object
#'
#' This family of functions allows you to query, set, combine, and filter your
#' trimesh object.
#'
#' @param x,mesh A trimesh object
#' @param drop Should unused vertices be dropped when filtering
#' @param value data.frame with matching number of rows
#' @param ... additional meshes
#' @param row.names,optional ignored
#'
#' @return Either a new trimesh object, a matrix, or a tibble, depending on the
#' function
#'
#' @name trimesh_index
#' @rdname trimesh_index
NULL

#' @rdname trimesh_index
#' @export
`[.trimesh` <- function(x, ..., drop = TRUE) {
  index <- seq_len(nrow(x$it_info))[x]
  x$it <- x$it[, index, drop = FALSE]
  x$it_info <- x$it_info[index, , drop = FALSE]
  if (drop) {
    vb_ind <- seq_len(x$vb)
    vb_ind_kept <- vb_ind[vb_ind %in% x$it]
    x$it[] <- match(x$it, vb_ind_kept)
    x$vb <- x$vb[, vb_ind_kept, drop = FALSE]
    x$vb_info <- x$vb_info[vb_ind_kept, , drop = FALSE]
  }
  x
}
#' @rdname trimesh_index
#' @export
as.data.frame.trimesh <- function(x, row.names = NULL, optional = FALSE, ...) {
  verts <- as.integer(x$it)
  data.frame(
    x = x$vb[1, verts],
    y = x$vb[2, verts],
    z = x$vb[3, verts],
    id = rep(seq_len(ncol(x$it)), each = 3),
    x$it_info
  )
}
#' @rdname trimesh_index
#' @export
triangles <- function(mesh) {
  mesh$it
}
#' @rdname trimesh_index
#' @export
vertices <- function(mesh) {
  mesh$vb
}
#' @rdname trimesh_index
#' @export
#' @importFrom tibble as_tibble
triangle_info <- function(mesh) {
  as_tibble(mesh$it_info)
}
#' @rdname trimesh_index
#' @export
`triangle_info<-` <- function(mesh, value) {
  if (nrow(value) != ncol(mesh$it)) {
    stop('Info must match the number of triangles in the mesh')
  }
  mesh$it_info <- value
  invisible(mesh)
}
#' @rdname trimesh_index
#' @export
#' @importFrom tibble as_tibble
vertice_info <- function(mesh) {
  as_tibble(mesh$vb_info)
}
#' @rdname trimesh_index
#' @export
`vertice_info<-` <- function(mesh, value) {
  if (nrow(value) != ncol(mesh$vb)) {
    stop('Info must match the number of vertices in the mesh')
  }
  mesh$vb_info <- value
  invisible(mesh)
}

#' @rdname trimesh_index
#' @export
#' @importFrom vctrs vec_c
mesh_bind <- function(mesh, ...) {
  mesh <- as_trimesh(mesh)
  meshes <- list(...)

  if (length(meshes) == 0) return(mesh)

  mesh2 <- as_trimesh(meshes[[1]])
  mesh$it <- cbind(mesh$it, mesh2$it + ncol(mesh$vb))
  mesh$vb <- cbind(mesh$vb, mesh2$vb)
  mesh$it_info <- vec_c(mesh$it_info, mesh2$it_info)
  mesh$vb_info <- vec_c(mesh$vb_info, mesh2$vb_info)

  if (length(meshes) == 1) return(mesh)

  meshes[[1]] <- mesh
  do.call(mesh_bind, meshes)
}
