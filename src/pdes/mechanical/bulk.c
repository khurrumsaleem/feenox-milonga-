/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox routines to build elemental mechanical objects
 *
 *  Copyright (C) 2021-2022 jeremy theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "mechanical.h"

int feenox_problem_build_allocate_aux_mechanical(size_t n_nodes) {
  
  mechanical.n_nodes = n_nodes;
  
  feenox_free(mechanical.B);
  feenox_check_alloc(mechanical.B = feenox_lowlevel_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));
  
  feenox_free(mechanical.CB);
  feenox_check_alloc(mechanical.CB = feenox_lowlevel_matrix_calloc(mechanical.stress_strain_size, feenox.pde.dofs * mechanical.n_nodes));

  return FEENOX_OK;
}



int feenox_problem_build_volumetric_gauss_point_mechanical(element_t *e, unsigned int v) {
  
#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));

  if (mechanical.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_mechanical(e->type->nodes));
  }
  
  if (mechanical.uniform_C == 0) {
    // material stress-strain relationship
    feenox_call(feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration));
    mechanical.compute_C(e->x[v], e->physical_group != NULL ? e->physical_group->material : NULL);
  }
  
  // TODO: axisymmetric
//  r_for_axisymmetric = feenox_compute_r_for_axisymmetric(this, v);
//  double r_for_axisymmetric = 1;
//  double w = this->w[v] * r_for_axisymmetric;
  
  lowlevel_matrix_t *dhdx = e->dhdx[v];
  for (int j = 0; j < mechanical.n_nodes; j++) {
    // TODO: virtual methods? they cannot be inlined...
    if (mechanical.variant == variant_full) {
      
      feenox_lowlevel_matrix_set(mechanical.B, 0, 3*j+0, feenox_lowlevel_matrix_get(dhdx, j, 0));
      feenox_lowlevel_matrix_set(mechanical.B, 1, 3*j+1, feenox_lowlevel_matrix_get(dhdx, j, 1));
      feenox_lowlevel_matrix_set(mechanical.B, 2, 3*j+2, feenox_lowlevel_matrix_get(dhdx, j, 2));
    
      feenox_lowlevel_matrix_set(mechanical.B, 3, 3*j+0, feenox_lowlevel_matrix_get(dhdx, j, 1));
      feenox_lowlevel_matrix_set(mechanical.B, 3, 3*j+1, feenox_lowlevel_matrix_get(dhdx, j, 0));

      feenox_lowlevel_matrix_set(mechanical.B, 4, 3*j+1, feenox_lowlevel_matrix_get(dhdx, j, 2));
      feenox_lowlevel_matrix_set(mechanical.B, 4, 3*j+2, feenox_lowlevel_matrix_get(dhdx, j, 1));

      feenox_lowlevel_matrix_set(mechanical.B, 5, 3*j+0, feenox_lowlevel_matrix_get(dhdx, j, 2));
      feenox_lowlevel_matrix_set(mechanical.B, 5, 3*j+2, feenox_lowlevel_matrix_get(dhdx, j, 0));
      
    } else if (mechanical.variant == variant_axisymmetric) {
      
      feenox_push_error_message("axisymmetric still not implemented");
      return FEENOX_ERROR;
      
    } else if (mechanical.variant == variant_plane_stress || mechanical.variant == variant_plane_strain) {
      
      // plane stress and plane strain are the same
      // see equation 14.18 IFEM CH.14 sec 14.4.1 pag 14-11
      feenox_lowlevel_matrix_set(mechanical.B, 0, 2*j+0, feenox_lowlevel_matrix_get(dhdx, j, 0));
      feenox_lowlevel_matrix_set(mechanical.B, 1, 2*j+1, feenox_lowlevel_matrix_get(dhdx, j, 1));
    
      feenox_lowlevel_matrix_set(mechanical.B, 2, 2*j+0, feenox_lowlevel_matrix_get(dhdx, j, 1));
      feenox_lowlevel_matrix_set(mechanical.B, 2, 2*j+1, feenox_lowlevel_matrix_get(dhdx, j, 0));

    } else {
      return FEENOX_ERROR;
    }
  }
  
  // volumetric force densities
  if (mechanical.f_x.defined || mechanical.f_y.defined || mechanical.f_z.defined) {
    if (mechanical.f_x.uniform == 0 || mechanical.f_y.uniform == 0 || mechanical.f_z.uniform == 0) {
      feenox_call(feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration));
    }
    
    for (int j = 0; j < e->type->nodes; j++) {
      int offset = feenox.pde.dofs*j;
      material_t *material = e->physical_group != NULL ? e->physical_group->material : NULL;
      double wh = e->w[v] * e->type->gauss[feenox.pde.mesh->integration].h[v][j];
      if (mechanical.f_x.defined) {
        feenox_lowlevel_vector_accum(feenox.pde.bi, offset+0, wh * mechanical.f_x.eval(&mechanical.f_x, e->x[v], material));
      }  
      if (mechanical.f_y.defined) {
        feenox_lowlevel_vector_accum(feenox.pde.bi, offset+1, wh * mechanical.f_y.eval(&mechanical.f_y, e->x[v], material));
      }
      if (mechanical.f_z.defined) {
        feenox_lowlevel_vector_accum(feenox.pde.bi, offset+2, wh * mechanical.f_z.eval(&mechanical.f_z, e->x[v], material));
      }  
    }
  }
    
  

  // elemental stiffness B'*C*B
  feenox_call(feenox_matmatmult(mechanical.C, mechanical.B, mechanical.CB));
  feenox_call(feenox_matTmatmult_accum(e->w[v], mechanical.B, mechanical.CB, feenox.pde.Ki));

  // thermal expansion strain vector
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    // if C is not uniform we already have x
    if (mechanical.uniform_C == 1 && mechanical.uniform_expansion == 0) {
      feenox_call(feenox_mesh_compute_x_at_gauss(e, v, feenox.pde.mesh->integration));
    }
    mechanical.compute_thermal_strain(e->x[v], e->physical_group != NULL ? e->physical_group->material : NULL);
    
    feenox_call(feenox_matTvecmult(mechanical.C, mechanical.et, mechanical.Cet));
    feenox_call(feenox_matTvecmult_accum(e->w[v], mechanical.B, mechanical.Cet, feenox.pde.bi));
  }
  
  // TODO: cleanup aux matrices C, B and CB
  
#endif
  
  return FEENOX_OK;
}
