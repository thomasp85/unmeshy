#' Calculate amount of light hitting triangles in a mesh
#'
#' This functions allows you to calculate the light hitting triangles in a mesh
#' based on one or more point lights. For each light the visibility of the mesh
#' from that point is calculated (with potential triangle splitting if it is
#' only halfway visible) and all visible triangles will get a luminance value
#' assigned based on it's distance to the light (light fall-off following the
#' inverse square law). For multiple lights the luminance is cumulative.
#'
#' @param mesh A trimesh object
#' @param lights A data.frame with `x`, `y`, and `z` columns giving light
#' positions in 3D space
#' @param luminance The intensity of each light (recycled to the number of rows
#' in `lights`)
#'
#' @return A new trimesh object, potentially with additional triangles if
#' triangles have been splitted. triangle info has been
#'
#' @export
#'
illuminate_mesh <- function(mesh, lights, luminance = 1) {
  mesh <- as_trimesh(mesh)
  if (!all('x', 'y', 'z') %in% names(lights)) {
    stop('lights must include an `x`, `y`, and `z` column', call. = FALSE)
  }
  luminance <- rep_len(luminance, ncol(lights))

  old_light <- triangle_info(mesh)[['luminance']]
  if (is.null(old_light)) old_light <- rep(0, ncol(mesh$it))

  shaded <- illuminate_mesh_c(
    mesh$vb, mesh$it, old_light,
    as.numeric(lights$x), as.numeric(lights$y), as.numeric(lights$z),
    as.numeric(luminance)
  )
  trimesh_from_triangles(
    shaded$x, shaded$y, shaded$z,
    cbind(triangle_info(mesh)[shaded$id, names(triangle_info(mesh) != 'luminance')],
          shaded[, !names(shaded) %in% c('x', 'y', 'z', 'id')])
  )
}
