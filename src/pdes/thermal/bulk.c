/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for the heat equation: bulk elements
 *
 *  Copyright (C) 2021--2023 Jeremy Theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "thermal.h"

// TODO: put the gauss loop inside this call so we can cache number of nodes, etc
int feenox_problem_build_volumetric_gauss_point_thermal(element_t *e, unsigned int q) {

#ifdef HAVE_PETSC
  
  double *x = feenox_fem_compute_x_at_gauss_if_needed(e, q, feenox.pde.mesh->integration, thermal.space_dependent_stiffness || thermal.space_dependent_source || thermal.space_dependent_mass);
  material_t *material = feenox_fem_get_material(e);  
  double k = thermal.k.eval(&thermal.k, x, material);

  double wdet = feenox_fem_compute_w_det_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  gsl_matrix *B = feenox_fem_compute_B_at_gauss_integration(e, q, feenox.pde.mesh->integration);
  feenox_call(feenox_blas_BtB_accum(B, wdet*k, feenox.fem.Ki));
  
  // volumetric heat source term Ht*q
  // TODO: total source Q
  if (thermal.q.defined) {
    double power = thermal.q.eval(&thermal.q, x, material);
    feenox_call(feenox_problem_rhs_add(e, q, &power));
  }
    
  if (feenox.pde.has_jacobian) {
    gsl_matrix *elemental_T = NULL;
    feenox_check_alloc(elemental_T = gsl_matrix_calloc(e->type->nodes, 1));
    double T = 0;
    double Tj = 0;
    gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
    for (unsigned int j = 0; j < e->type->nodes; j++) {
      Tj = feenox_vector_get(feenox.pde.solution[0]->vector_value, e->node[j]->index_dof[0]);
      gsl_matrix_set(elemental_T, j, 0, Tj);
      T += gsl_matrix_get(H, 0, j) * Tj; 
    }  

    if (thermal.temperature_dependent_stiffness) {
      double dkdT = feenox_expression_derivative_wrt_function(thermal.k.expr, feenox.pde.solution[0], T);

      gsl_matrix *BtB = gsl_matrix_calloc(e->type->nodes, e->type->nodes);
      feenox_call(feenox_blas_BtB(B, 1.0, BtB));
      feenox_call(feenox_blas_PtCB_accum(BtB, elemental_T, H, NULL, wdet*dkdT, feenox.fem.JKi));
      gsl_matrix_free(BtB);
    }

    if (thermal.temperature_dependent_source) {
      double dqdT = feenox_expression_derivative_wrt_function(thermal.q.expr, feenox.pde.solution[0], T);
      // mind the positive sign!
      feenox_call(feenox_blas_BtB_accum(H, +wdet*dqdT, feenox.fem.Jbi));
    } 

    gsl_matrix_free(elemental_T);
  }

  // mass matrix Ht*rho*cp*H
  if (feenox.pde.has_mass) {
    double rhocp = 0;
    if (thermal.rhocp.defined) {
      rhocp = thermal.rhocp.eval(&thermal.rhocp, x, material);
    } else if (thermal.kappa.defined) {
      rhocp = k / thermal.kappa.eval(&thermal.kappa, x, material);
    } else if (thermal.rho.defined && thermal.cp.defined) {
      rhocp = thermal.rho.eval(&thermal.rho, x, material) * thermal.cp.eval(&thermal.cp, x, material);
    } else {
      // this should have been already checked
      feenox_push_error_message("no heat capacity found");
      return FEENOX_ERROR;
    }
    gsl_matrix *H = feenox_fem_compute_H_Gc_at_gauss(e, q, feenox.pde.mesh->integration);
    feenox_call(feenox_blas_BtB_accum(H, wdet*rhocp, feenox.fem.Mi));
  }
  

#endif
  
  return FEENOX_OK;
  
}
