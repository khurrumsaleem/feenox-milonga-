/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: virtual methods
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
#ifndef NEUTRON_DIFFUSION_FEM_METHODS_H
#define NEUTRON_DIFFUSION_FEM_METHODS_H

// neutron_diffusion/parser.c
extern int feenox_problem_parse_problem_neutron_diffusion(const char *token);
extern int feenox_problem_parse_write_post_neutron_diffusion(mesh_write_t *mesh_write, const char *token);

// neutron_diffusion/init.c
extern int feenox_problem_parse_time_init_neutron_diffusion(void);
extern int feenox_problem_init_runtime_neutron_diffusion(void);
#ifdef HAVE_PETSC
extern int feenox_problem_setup_pc_neutron_diffusion(PC pc);
extern int feenox_problem_setup_ksp_neutron_diffusion(KSP ksp);
#endif
#ifdef HAVE_SLEPC
extern int feenox_problem_setup_eps_neutron_diffusion(EPS eps);
#endif

// neutron_diffusion/bulk.c
extern int feenox_problem_build_allocate_aux_neutron_diffusion(unsigned int n_nodes);
extern int feenox_problem_neutron_diffusion_eval_XS(material_t *material, double *x);
extern int feenox_problem_build_volumetric_neutron_diffusion(element_t *e);
extern int feenox_problem_build_volumetric_gauss_point_neutron_diffusion(element_t *element, unsigned int q);

// neutron_diffusion/bc.c
extern int feenox_problem_bc_parse_neutron_diffusion(bc_data_t *bc_data, const char *lhs, char *rhs);
extern int feenox_problem_bc_set_neutron_diffusion_null(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_neutron_diffusion_flux(bc_data_t *bc_data, element_t *e, size_t j_global);
extern int feenox_problem_bc_set_neutron_diffusion_vacuum(bc_data_t *bc_data, element_t *e, unsigned int q);
extern int feenox_problem_bc_set_neutron_diffusion_current(bc_data_t *bc_data, element_t *e, unsigned int q);


// thermal/currents.c
extern int feenox_problem_gradient_fill_neutron_diffusion(void);
extern int feenox_problem_gradient_properties_at_element_nodes_neutron_diffusion(element_t *element, mesh_t *mesh);
extern int feenox_problem_gradient_fluxes_at_node_alloc_neutron_diffusion(node_t *node);
extern int feenox_problem_gradient_add_elemental_contribution_to_node_neutron_diffusion(node_t *node, element_t *element, unsigned int j, double rel_weight);
extern int feenox_problem_gradient_fill_fluxes_neutron_diffusion(mesh_t *mesh, size_t j);

// neutron_diffusion/post.c
extern int feenox_problem_solve_post_neutron_diffusion(void);
#endif  // NEUTRON_DIFFUSION_FEM
