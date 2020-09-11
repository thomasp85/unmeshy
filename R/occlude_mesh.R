#' Perform hidden-surface determination of mesh for e.g. occlusion culling
#'
#' This function will determine the visibility of each triangle in a mesh,
#' potentially cut up triangles if they are only partly occluded. No triangles
#' are removed by this function but a `visible` and `back_facing` column is
#' added indicating if the given triangle can be seen from the specified
#' viewpoint. If a given triangle is not visible and not back facing it means
#' that it is being occluded by other triangles in from of it.
#'
#' @param mesh A trimesh object
#' @param view A numeric vector with three elements giving the viewpoint to look
#' from when calculating occlusion.
#'
#' @return A new trimesh object with a `visible` and `back_facing` column added
#' to the triangle info.
#'
#' @export
occlude_mesh <- function(mesh, view) {
  mesh <- as_trimesh(mesh)
  occluded <- occlude_mesh_c(mesh$vb, mesh$it, view[1], view[2], view[3])
  trimesh_from_triangles(
    occluded$x, occluded$y, occluded$z,
    cbind(triangle_info(mesh)[occluded$id, ],
          occluded[, !names(occluded) %in% c('x', 'y', 'z', 'id')])
  )
}
