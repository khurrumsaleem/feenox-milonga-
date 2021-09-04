/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: initialization
 *
 *  Copyright (C) 2021 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "neutron_diffusion.h"
extern feenox_t feenox;
extern neutron_diffusion_t neutron_diffusion;

int feenox_problem_init_parser_neutron_diffusion(void) {

#ifdef HAVE_SLEPC  
  feenox.pde.problem_init_runtime_particular = feenox_problem_init_runtime_neutron_diffusion;
  feenox.pde.bc_parse = feenox_problem_bc_parse_neutron_diffusion;
  feenox.pde.setup_eps = feenox_problem_setup_eps_neutron_diffusion;
//  feenox.pde.setup_ksp = feenox_problem_setup_ksp_neutron_diffusion;
//  feenox.pde.setup_pc = feenox_problem_setup_pc_neutron_diffusion;
  feenox.pde.bc_set_dirichlet = feenox_problem_bc_set_neutron_diffusion_null;
  feenox.pde.build_element_volumetric_gauss_point = feenox_problem_build_volumetric_gauss_point_neutron_diffusion;
  feenox.pde.solve_post = feenox_problem_solve_post_neutron_diffusion;
  
  // TODO: higher harmonics?
  feenox.pde.nev = 1;

  // default is 1 group
  if (neutron_diffusion.groups == 0) {
    neutron_diffusion.groups = 1;
  }
  // dofs = number of groups
  feenox.pde.dofs = neutron_diffusion.groups;
  
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.dofs; g++) {
    feenox_check_minusone(asprintf(&feenox.pde.unknown_name[g], "phi%d", g+1));
  }
  feenox_call(feenox_problem_define_solutions());
  
  // TODO: for one group make an alias between phi1 and phi
  
  
  // we'd rather ser nodes than cells 
  feenox.mesh.default_field_location = field_location_nodes;
  
///va+keff+desc The effective multiplication factor\ $k_\text{eff}$.
  neutron_diffusion.keff = feenox_define_variable_get_ptr("keff");

  // define eigenvectors (we don't know its size yet)
  feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));
  for (g = 0; g < feenox.pde.nev; g++) {
    char *modename = NULL;
    feenox_check_minusone(asprintf(&modename, "eig%d", g+1));
    feenox_check_alloc(feenox.pde.vectors.phi[g] = feenox_define_vector_get_ptr(modename, 0));
    feenox_free(modename);
  }

#endif
  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_neutron_diffusion(void) {

#ifdef HAVE_PETSC  
  // we are FEM not FVM
  feenox.pde.mesh->data_type = data_type_node;
  feenox.pde.spatial_unknowns = feenox.pde.mesh->n_nodes;
  feenox.pde.global_size = feenox.pde.spatial_unknowns * feenox.pde.dofs;

  // set the size of the eigenvectors (we did not know their size in init_parser() above
  unsigned int i = 0;
  for (i = 0; i < feenox.pde.nev; i++) {
    feenox_call(feenox_vector_set_size(feenox.pde.vectors.phi[i], feenox.pde.global_size));
  }
  
  // initialize XSs
  feenox_check_alloc(neutron_diffusion.D = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.sigma_t = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.sigma_a = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.nu_sigma_f = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.source = calloc(feenox.pde.dofs, sizeof(distribution_t)));
  feenox_check_alloc(neutron_diffusion.sigma_s = calloc(feenox.pde.dofs, sizeof(distribution_t *)));
  unsigned int g = 0;
  for (g = 0; g < feenox.pde.nev; g++) {
    char *name = NULL;

    feenox_check_minusone(asprintf(&name, "D%d", g+1));
    feenox_distribution_init(&neutron_diffusion.D[g], name);
    if (neutron_diffusion.D[g].defined) {
      neutron_diffusion.D[g].space_dependent = feenox_expression_depends_on_space(neutron_diffusion.D[g].dependency_variables);
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_t%d", g+1));
    feenox_distribution_init(&neutron_diffusion.sigma_t[g], name);
    if (neutron_diffusion.sigma_t[g].defined) {
      neutron_diffusion.sigma_t[g].space_dependent = feenox_expression_depends_on_space(neutron_diffusion.sigma_t[g].dependency_variables);
    }
    feenox_free(name);
    
    feenox_check_minusone(asprintf(&name, "Sigma_a%d", g+1));
    feenox_distribution_init(&neutron_diffusion.sigma_a[g], name);
    if (neutron_diffusion.sigma_a[g].defined) {
      neutron_diffusion.sigma_a[g].space_dependent = feenox_expression_depends_on_space(neutron_diffusion.sigma_a[g].dependency_variables);
    }  
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "nuSigma_f%d", g+1));
    feenox_distribution_init(&neutron_diffusion.nu_sigma_f[g], name);
    if (neutron_diffusion.nu_sigma_f[g].defined) {
      neutron_diffusion.has_fission = PETSC_TRUE;
      neutron_diffusion.nu_sigma_f[g].space_dependent = feenox_expression_depends_on_space(neutron_diffusion.nu_sigma_f[g].dependency_variables);
    }  
    feenox_free(name);

    feenox_check_minusone(asprintf(&name, "S%d", g+1));
    feenox_distribution_init(&neutron_diffusion.source[g], name);
    if (neutron_diffusion.source[g].defined) {
      neutron_diffusion.has_sources = PETSC_TRUE;
      neutron_diffusion.source[g].space_dependent = feenox_expression_depends_on_space(neutron_diffusion.source[g].dependency_variables);
    }  
    feenox_free(name);
    
    feenox_check_alloc(neutron_diffusion.sigma_s[g] = calloc(feenox.pde.dofs, sizeof(distribution_t)));
    unsigned int g_prime = 0;
    for (g_prime = 0; g_prime < feenox.pde.dofs; g_prime++) {
      feenox_check_minusone(asprintf(&name, "Sigma_s%d.%d", g+1, g_prime+1));
      feenox_distribution_init(&neutron_diffusion.sigma_s[g][g_prime], name);
      if (neutron_diffusion.sigma_s[g][g_prime].defined) {
        neutron_diffusion.sigma_s[g][g_prime].space_dependent = feenox_expression_depends_on_space(neutron_diffusion.sigma_s[g][g_prime].dependency_variables);
      }  
      feenox_free(name);
    }
  }


  if (neutron_diffusion.has_sources == PETSC_FALSE && neutron_diffusion.has_fission == PETSC_FALSE) {
    feenox_push_error_message("neither fission nor sources found");
    return FEENOX_ERROR;
  }

  feenox.pde.math_type = (neutron_diffusion.has_sources == PETSC_TRUE) ? math_type_linear :  math_type_eigen;
  feenox.pde.solve     = (neutron_diffusion.has_sources == PETSC_TRUE) ? feenox_problem_solve_petsc_linear : feenox_problem_solve_slepc_eigen;
  
  feenox.pde.has_stiffness = PETSC_TRUE;
  feenox.pde.has_mass = (neutron_diffusion.has_sources == PETSC_TRUE) ? PETSC_FALSE : PETSC_TRUE;
  feenox.pde.has_rhs  = (neutron_diffusion.has_sources == PETSC_TRUE) ? PETSC_TRUE : PETSC_FALSE;
  
  feenox.pde.has_jacobian_K = PETSC_FALSE;
  feenox.pde.has_jacobian_M = PETSC_FALSE;
  feenox.pde.has_jacobian_b = PETSC_FALSE;
  feenox.pde.has_jacobian = feenox.pde.has_jacobian_K || feenox.pde.has_jacobian_M || feenox.pde.has_jacobian_b;
  
  feenox.pde.symmetric_K = PETSC_FALSE;
  feenox.pde.symmetric_M = PETSC_FALSE;
#endif
  return FEENOX_OK;
}

#ifdef HAVE_PETSC
int feenox_problem_setup_pc_neutron_diffusion(PC pc) {

  PCType pc_type = NULL;
  petsc_call(PCGetType(pc, &pc_type));
  if (pc_type == NULL) {
#ifdef PETSC_HAVE_MUMPS
    petsc_call(PCSetType(pc, PCCHOLESKY));
    petsc_call(PCFactorSetMatSolverType(pc, MATSOLVERMUMPS));
#else
    // TODO: this will complain in parallel
    petsc_call(PCSetType(pc, PCLU));    
#endif
  }
  
  petsc_call(PCGetType(pc, &pc_type));
  
  return FEENOX_OK;
}

int feenox_problem_setup_ksp_neutron_diffusion(KSP ksp ) {

  KSPType ksp_type = NULL;
  petsc_call(KSPGetType(ksp, &ksp_type));
  if (ksp_type == NULL) {
    // if the user did not choose anything, we default to preonly + direct solver
    petsc_call(KSPSetType(ksp, KSPPREONLY));
  }  

  return FEENOX_OK;
}
#endif

#ifdef HAVE_SLEPC
int feenox_problem_setup_eps_neutron_diffusion(EPS eps) {

  // generalized non-hermitian problem
  petsc_call(EPSSetProblemType(eps, EPS_GNHEP));
  
/*  
  ST st = NULL;
  petsc_call(EPSGetST(feenox.pde.eps, &st));
  if (st == NULL) {
    feenox_push_error_message("error getting spectral transform object");
    return FEENOX_ERROR;
  }
 */
/*  
  // the user might have already set a custom ST, se we peek what it is
  STType st_type = NULL;
  petsc_call(STGetType(st, &st_type));
  
  if (st_type != NULL && feenox.pde.eigen_formulation == eigen_formulation_undefined) {
    // if there is no formulation but an st_type, choose an appropriate one
    feenox.pde.eigen_formulation = (strcmp(st_type, STSHIFT) == 0) ? eigen_formulation_lambda : eigen_formulation_omega;
  }

  if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
    if (st_type != NULL) {
      if (strcmp(st_type, STSINVERT) != 0) {
        feenox_push_error_message("eigen formulation omega needs shift-and-invert spectral transformation");
        return FEENOX_ERROR;
      }  
    } else {
      petsc_call(STSetType(st, STSINVERT));
    }
    // shift and invert needs a target
    petsc_call(EPSSetTarget(eps, feenox_var_value(1e6)));
    
  } else {
    // lambda needs shift
    if (st_type != NULL) {
      if (strcmp(st_type, STSHIFT) != 0) {
        feenox_push_error_message("eigen formulation lambda needs shift spectral transformation");
        return FEENOX_ERROR;
      }
    } else {
      petsc_call(STSetType(st, STSHIFT));
    }
    // seek for largest lambda (i.e. smallest omega)
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_SMALLEST_MAGNITUDE));
  }  
*/

  if (feenox.pde.eigen_formulation == eigen_formulation_omega) {
//    petsc_call(STSetType(st, STSINVERT));
     petsc_call(EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE));
//    petsc_call(EPSSetTarget(eps, 1e6));
    
  } else {
//    petsc_call(STSetType(st, STSHIFT));
    petsc_call(EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE));
  }
  

  return FEENOX_OK;
}
#endif

