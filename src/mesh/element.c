/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX's mesh-related element routines
 *
 *  Copyright (C) 2015--2021 Jeremy Theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
#include "../feenox.h"
#include "element.h"

/*

Line:                   Line3:           Line4:    
                                                
0----------1 --> u      0-----2----1     0----2----3----1

Triangle:               Triangle6:          Triangle9/10:          Triangle12/15:

v                                                              
^                                                                   2 
|                                                                   | \ 
2                       2                    2                      9   8
|`\                     |`\                  | \                    |     \ 
|  `\                   |  `\                7   6                 10 (14)  7
|    `\                 5    `4              |     \                |         \ 
|      `\               |      `\            8  (9)  5             11 (12) (13) 6
|        `\             |        `\          |         \            |             \
0----------1 --> u      0-----3----1         0---3---4---1          0---3---4---5---1

Quadrangle:            Quadrangle8:            Quadrangle9:

      v
      ^
      |
3-----------2          3-----6-----2           3-----6-----2 
|     |     |          |           |           |           | 
|     |     |          |           |           |           | 
|     +---- | --> u    7           5           7     8     5 
|           |          |           |           |           | 
|           |          |           |           |           | 
0-----------1          0-----4-----1           0-----4-----1 

Tetrahedron:                          Tetrahedron10:

                   v
                 .
               ,/
              /
           2                                     2                              
         ,/|`\                                 ,/|`\                          
       ,/  |  `\                             ,/  |  `\       
     ,/    '.   `\                         ,6    '.   `5     
   ,/       |     `\                     ,/       8     `\   
 ,/         |       `\                 ,/         |       `\ 
0-----------'.--------1 --> u         0--------4--'.--------1
 `\.         |      ,/                 `\.         |      ,/ 
    `\.      |    ,/                      `\.      |    ,9   
       `\.   '. ,/                           `7.   '. ,/     
          `\. |/                                `\. |/       
             `3                                    `3        
                `\.
                   ` w

Hexahedron:             Hexahedron20:          Hexahedron27:

       v
3----------2            3----13----2           3----13----2     
|\     ^   |\           |\         |\          |\         |\    
| \    |   | \          | 15       | 14        |15    24  | 14  
|  \   |   |  \         9  \       11 \        9  \ 20    11 \  
|   7------+---6        |   7----19+---6       |   7----19+---6 
|   |  +-- |-- | -> u   |   |      |   |       |22 |  26  | 23| 
0---+---\--1   |        0---+-8----1   |       0---+-8----1   | 
 \  |    \  \  |         \  17      \  18       \ 17    25 \  18
  \ |     \  \ |         10 |        12|        10 |  21    12| 
   \|      w  \|           \|         \|          \|         \| 
    4----------5            4----16----5           4----16----5 

Prism:                      Prism15:               Prism18:

           w
           ^
           |
           3                       3                      3        
         ,/|`\                   ,/|`\                  ,/|`\      
       ,/  |  `\               12  |  13              12  |  13    
     ,/    |    `\           ,/    |    `\          ,/    |    `\  
    4------+------5         4------14-----5        4------14-----5 
    |      |      |         |      8      |        |      8      | 
    |    ,/|`\    |         |      |      |        |    ,/|`\    | 
    |  ,/  |  `\  |         |      |      |        |  15  |  16  | 
    |,/    |    `\|         |      |      |        |,/    |    `\| 
   ,|      |      |\        10     |      11       10-----17-----11
 ,/ |      0      | `\      |      0      |        |      0      | 
u   |    ,/ `\    |    v    |    ,/ `\    |        |    ,/ `\    | 
    |  ,/     `\  |         |  ,6     `7  |        |  ,6     `7  | 
    |,/         `\|         |,/         `\|        |,/         `\| 
    1-------------2         1------9------2        1------9------2 

Pyramid:                     Pyramid13:                   Pyramid14:

               4                            4                            4
             ,/|\                         ,/|\                         ,/|\
           ,/ .'|\                      ,/ .'|\                      ,/ .'|\
         ,/   | | \                   ,/   | | \                   ,/   | | \
       ,/    .' | `.                ,/    .' | `.                ,/    .' | `.
     ,/      |  '.  \             ,7      |  12  \             ,7      |  12  \
   ,/       .' w |   \          ,/       .'   |   \          ,/       .'   |   \
 ,/         |  ^ |    \       ,/         9    |    11      ,/         9    |    11
0----------.'--|-3    `.     0--------6-.'----3    `.     0--------6-.'----3    `.
 `\        |   |  `\    \      `\        |      `\    \     `\        |      `\    \
   `\     .'   +----`\ - \ -> v  `5     .'        10   \      `5     .' 13     10   \
     `\   |    `\     `\  \        `\   |           `\  \       `\   |           `\  \ 
       `\.'      `\     `\`          `\.'             `\`         `\.'             `\` 
          1----------------2            1--------8-------2           1--------8-------2
                    `\
                       u
*/

int feenox_mesh_element_types_init(void) {

  if (feenox.mesh.element_types != NULL) {
    return FEENOX_OK;
  }
  
  feenox_check_alloc(feenox.mesh.element_types = calloc(NUMBER_ELEMENT_TYPE, sizeof(element_type_t)));
  element_type_t *element_type = NULL;

  // undefined  ----------------------------------------------------------------
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_UNDEFINED];
  feenox_check_alloc((element_type->name = strdup("undefined")));
  element_type->id = ELEMENT_TYPE_UNDEFINED;
  element_type->dim = 0;
  element_type->nodes = 0;
  element_type->faces = 0;
  element_type->nodes_per_face = 0;
  element_type->h = NULL;
  element_type->dhdxi = NULL;
  element_type->point_inside = NULL;
  element_type->volume = NULL;
  
  // line ----------------------------------------------------------------------
  feenox_call(feenox_mesh_line2_init());
  feenox_call(feenox_mesh_line3_init());

  // triangles -----------------------------------------------------------------
  feenox_call(feenox_mesh_triang3_init());
  feenox_call(feenox_mesh_triang6_init());
  
  // quadrangles ----------------------------------------------------------------
  feenox_call(feenox_mesh_quad4_init());
  feenox_call(feenox_mesh_quad8_init());
  feenox_call(feenox_mesh_quad9_init());
  
  // tetrahedra  ---------------------------------------------------------------
  feenox_call(feenox_mesh_tet4_init());
  feenox_call(feenox_mesh_tet10_init());
  
  // hexahedra ---------------------------------------------------------------- 
  feenox_call(feenox_mesh_hexa8_init());
  feenox_call(feenox_mesh_hexa20_init());
  feenox_call(feenox_mesh_hexa27_init());
    
  // prism ---------------------------------------------------------------------
  feenox_call(feenox_mesh_prism6_init());
  feenox_call(feenox_mesh_prism15_init());

  // not supported  
  element_type = &feenox.mesh.element_types[ELEMENT_TYPE_PYRAMID5];
  element_type->dim = 3;
  feenox_check_alloc(element_type->name = strdup("pyramid5"));
  element_type->id = ELEMENT_TYPE_PYRAMID5;
  element_type->nodes = 0;

  feenox_check_alloc(feenox.mesh.element_types[13].name = strdup("prism18"));
  feenox_check_alloc(feenox.mesh.element_types[14].name = strdup("pyramid14"));

  
  // point
  feenox_call(feenox_mesh_one_node_point_init());
  
  // compute the barycenter of each element type in the r-space
  for (unsigned int i = 0; i < NUMBER_ELEMENT_TYPE; i++) {
    if (feenox.mesh.element_types[i].node_coords != NULL) {
      feenox_check_alloc(feenox.mesh.element_types[i].barycenter_coords = calloc(feenox.mesh.element_types[i].dim, sizeof(double)));
      for (unsigned int j = 0; j < feenox.mesh.element_types[i].nodes; j++) {
        for (unsigned int d = 0; d < feenox.mesh.element_types[i].dim; d++) {
          feenox.mesh.element_types[i].barycenter_coords[d] += feenox.mesh.element_types[i].node_coords[j][d];
        }
      }
      for (unsigned int d = 0; d < feenox.mesh.element_types[i].dim; d++) {
        feenox.mesh.element_types[i].barycenter_coords[d] /= feenox.mesh.element_types[i].nodes;
      }
    }
  }
  
  return FEENOX_OK;
  
};

int feenox_mesh_alloc_gauss(gauss_t *gauss, element_type_t *element_type, int Q) {

  gauss->Q = Q;
  feenox_check_alloc(gauss->w = calloc(gauss->Q, sizeof(double)));
  feenox_check_alloc(gauss->xi = calloc(gauss->Q, sizeof(double *)));
  feenox_check_alloc(gauss->H_c = calloc(gauss->Q, sizeof(gsl_matrix *)));
  feenox_check_alloc(gauss->B_c = calloc(gauss->Q, sizeof(gsl_matrix *)));
  
  unsigned int dim = (element_type->dim != 0) ? element_type->dim : 1;
  for (unsigned int q = 0; q < gauss->Q; q++) {
    feenox_check_alloc(gauss->xi[q] = calloc(dim, sizeof(double)));
    
    feenox_check_alloc(gauss->H_c[q] = gsl_matrix_calloc(1, element_type->nodes));
    feenox_check_alloc(gauss->B_c[q] = gsl_matrix_calloc(dim, element_type->nodes));
  }
  
  return FEENOX_OK;
  
}

int feenox_mesh_init_shape_at_gauss(gauss_t *gauss, element_type_t *element_type) {

  for (unsigned int q = 0; q < gauss->Q; q++) {
    for (unsigned int j = 0; j < element_type->nodes; j++) {
      gsl_matrix_set(gauss->H_c[q], 0, j, element_type->h(j, gauss->xi[q]));
      for (unsigned int d = 0; d < element_type->dim; d++) {
        gsl_matrix_set(gauss->B_c[q], d, j, element_type->dhdxi(j, d, gauss->xi[q]));
      }
    }
  }
  
  return FEENOX_OK;
}


// note this one does not fill in the nodes
int feenox_mesh_create_element(element_t *e, size_t index, size_t tag, unsigned int type, physical_group_t *physical_group) {
 
  e->index = index;
  e->tag = tag;
  e->type = &(feenox.mesh.element_types[type]);
  feenox_check_alloc(e->node  = calloc(e->type->nodes, sizeof(node_t *)));
  e->physical_group = physical_group;
  
  return FEENOX_OK;
}

int feenox_mesh_add_element_to_list(element_ll_t **list, element_t *e) {

  element_ll_t *item = NULL;
  feenox_check_alloc(item = calloc(1, sizeof(element_ll_t)));
  item->element = e;
  // it is way faster to prepend than to append
//  LL_APPEND(*list, item);
  LL_PREPEND(*list, item);

  return FEENOX_OK;
  
}

int feenox_mesh_compute_element_barycenter(element_t *this, double barycenter[]) {
  
  int J = this->type->nodes;
  barycenter[0] = barycenter[1] = barycenter[2] = 0;
  for (unsigned int j = 0; j < J; j++) {
    barycenter[0] += this->node[j]->x[0];
    barycenter[1] += this->node[j]->x[1];
    barycenter[2] += this->node[j]->x[2];
  }
  barycenter[0] /= J; 
  barycenter[1] /= J; 
  barycenter[2] /= J; 
  
  return FEENOX_OK;
}


int feenox_mesh_init_nodal_indexes(mesh_t *this, int dofs) {
  
  this->degrees_of_freedom = dofs;

  for (size_t j = 0; j < this->n_nodes; j++) {
    feenox_check_alloc(this->node[j].index_dof = malloc(this->degrees_of_freedom*sizeof(size_t)));
    for (unsigned int g = 0; g < this->degrees_of_freedom; g++) {
      this->node[j].index_dof[g] = this->degrees_of_freedom*j + g;
    }
  }  
  
  return FEENOX_OK;
}

int feenox_mesh_compute_normal_2d(element_t *e) {
  if (e->normal == NULL) {
    feenox_check_alloc(e->normal = calloc(3, sizeof(double)));
    double a[3], b[3];
    feenox_mesh_subtract(e->node[0]->x, e->node[1]->x, a);
    feenox_mesh_subtract(e->node[0]->x, e->node[2]->x, b);
    feenox_mesh_normalized_cross(a, b, e->normal);
  }
  
  return FEENOX_OK;
}


unsigned int feenox_mesh_compute_local_node_index(element_t *element, size_t global_index) {
  unsigned int local_index = -1;
  
  for (unsigned int j = 0; j < element->type->nodes; j++) {
    if (element->node[j]->tag == global_index+1) {
      local_index = j;
      break;
    }
  }
  
  return local_index;
}


void feenox_mesh_add_node_parent(node_relative_t **parents, int index) {
  node_relative_t *parent = calloc(1, sizeof(node_relative_t));
  parent->index = index;
//  LL_APPEND(*parents, parent);
  LL_PREPEND(*parents, parent);
  return;
}

void feenox_mesh_compute_coords_from_parent(element_type_t *element_type, int j) {
  node_relative_t *parent;
  int den = 0;
  
  LL_FOREACH(element_type->node_parents[j], parent) {
    den++;
    for (unsigned int d = 0; d < element_type->dim; d++) {
      element_type->node_coords[j][d] += element_type->node_coords[parent->index][d];
    }
  }
  
  for (unsigned int d = 0; d < element_type->dim; d++) {
    if (den == 0) {
      fprintf(stderr, "le tomaron la leche al gato\n");
    }
    element_type->node_coords[j][d] /= den;
  }
  return;
}

int feenox_mesh_elements_info(void) {
  
  printf("# Elements supported by FeenoX\n\n");
  printf(" Gmsh type | Internal name | Dimension | Order | Nodes | Details\n");
  printf(":---------:|:-------------:|:---------:|:-----:|:-----:|:----------\n");
  
  for (unsigned int i = 0; i < NUMBER_ELEMENT_TYPE; i++) {
    element_type_t *element_type = &feenox.mesh.element_types[i];
    if (element_type->id != 0 && element_type->nodes != 0) {
      printf("    % 2d    |   `%s`   |   %d    |   %d   |   % 2d   |  @sec-%s\n",
              element_type->id, element_type->name, element_type->dim,  element_type->order,element_type->nodes, element_type->name);
    }
  }
  printf("\n");

  for (unsigned int i = 0; i < NUMBER_ELEMENT_TYPE; i++) {
    element_type_t *element_type = &feenox.mesh.element_types[i];
    if (element_type->id != 0 && element_type->nodes != 0) {
      printf("## %s {#sec-%s} \n\n", element_type->name, element_type->name);

      printf("### Nodes and shape functions\n\n");
      
      if (feenox_mesh_elements_info_geo(&feenox.mesh.element_types[i])) {
        printf("![%s](%s){#fig-%s}\n\n", element_type->name, element_type->name, element_type->name);
      }
      
      if (element_type->ascii_art != NULL) {
        printf("```\n%s\n```\n\n", element_type->ascii_art);
      }
      
/*
<text fill="#000000" x="1600" y="958" font-size="15" text-anchor="start" dy="0" font-family="Helvetica">ξ</text>
<text fill="#000000" x="1570" y="928" font-size="15" text-anchor="start" dy="0" font-family="Helvetica">η</text>
<text fill="#000000" x="1570" y="958" font-size="15" text-anchor="start" dy="0" font-family="Helvetica">ζ</text>
*/      
      
      printf(" $j$%s%s%s%s\n", (element_type->dim > 0) ? " | $\\xi$ " : "", 
                                   (element_type->dim > 1) ? " | $\\eta$ " : "",
                                   (element_type->dim > 2) ? " | $\\zeta$ " : "",
                                   (element_type->h_latex != NULL) ? " | $h_j(\\symbf{\\xi})$ " : "");
      printf(":-------:%s%s%s%s\n",     (element_type->dim > 0) ? "|:------:" : "", 
                                   (element_type->dim > 1) ? "|:------:" : "", 
                                   (element_type->dim > 2) ? "|:------:" : "",
                                   (element_type->h_latex != NULL) ? "|:----------------------------------" : "");
      for (unsigned int j = 0; j < element_type->nodes; j++) {
        printf("  % 2d ", j);
        if (element_type->dim > 0) {
          if (fabs(element_type->node_coords[j][0]) > 1e-4) {
            printf("| %+g ", (element_type->node_coords[j][0]));
          } else {
            printf("|  0 ");
          }
          if (element_type->dim > 1) {
            if (fabs(element_type->node_coords[j][1]) > 1e-4) {
              printf("| %+g ", (element_type->node_coords[j][1]));
            } else {
              printf("|  0 ");
            }
            if (element_type->dim > 2) {
              if (fabs(element_type->node_coords[j][2]) > 1e-4) {
                printf("| %+g ", (element_type->node_coords[j][2]));
              } else {
                printf("|  0 ");
              }
            }
          }
        }
        if (element_type->h_latex != NULL) {
          printf("| $%s$", element_type->h_latex(j));
        }
        printf("\n");
      }
      printf("\n");
    

      printf("### Quadrature points\n\n");
      
      printf(" $q$ | $\\omega_q$%s%s%s\n", (element_type->dim > 0) ? " | $\\xi_q$ " : "", 
                                             (element_type->dim > 1) ? " | $\\eta_q$ " : "",
                                             (element_type->dim > 2) ? " | $\\zeta_q$ " : "");
      printf(":-------:|:-------:%s%s%s\n",     (element_type->dim > 0) ? "|:------:" : "", 
                                                  (element_type->dim > 1) ? "|:------:" : "", 
                                                  (element_type->dim > 2) ? "|:------:" : "");
      for (unsigned int q = 0; q < element_type->gauss[0].Q; q++) {
        printf("  % 2d  |  %g  ", q, element_type->gauss[0].w[q]);
        
        if (element_type->dim > 0) {
          if (fabs(element_type->gauss[0].xi[q][0]) > 1e-4) {
            printf("| %+g ", (element_type->gauss[0].xi[q][0]));
          } else {
            printf("|  0 ");
          }
          if (element_type->dim > 1) {
            if (fabs(element_type->gauss[0].xi[q][1]) > 1e-4) {
              printf("| %+g ", (element_type->gauss[0].xi[q][1]));
            } else {
              printf("|  0 ");
            }
            if (element_type->dim > 2) {
              if (fabs(element_type->gauss[0].xi[q][2]) > 1e-4) {
                printf("| %+g ", (element_type->gauss[0].xi[q][2]));
              } else {
                printf("|  0 ");
              }
            }
          }
        }
        printf("\n");
      }
      printf("\n");
    }
    printf("\n");
  }
  
  return FEENOX_OK;
}

int feenox_mesh_elements_info_geo(element_type_t *element_type) {
  
  if (element_type->doc_edges == NULL) {
    return 0;
  }
    
  char *geo_name = NULL;
  feenox_check_minusone(asprintf(&geo_name, "%s.geo", element_type->name));
  FILE *geo = fopen(geo_name, "w");
  if (geo == NULL) {
    return 0;
  }
  for (int j = 0; j < element_type->nodes; j++) {
    fprintf(geo, "Point (%d) = {%g, %g, %g};\n", j, (element_type->dim > 0) ? element_type->node_coords[j][0] : 0,
                                                    (element_type->dim > 1) ? element_type->node_coords[j][1] : 0,
                                                    (element_type->dim > 2) ? element_type->node_coords[j][2] : 0);
  }
  fprintf(geo, "\n");

  if (element_type->dim > 0) {  
    for (int l = 0; l < element_type->doc_n_edges; l++) {
      fprintf(geo, "Line (%d) = {%d, %d};\n", 1+l, element_type->doc_edges[l][0], element_type->doc_edges[l][1]);
    }
    fprintf(geo, "\n");

    if (element_type->dim > 1) {
      for (int f = 0; f < element_type->doc_n_faces; f++) {
        fprintf(geo, "Curve loop (%d) = {%d", 1+f, element_type->doc_faces[f][0]);
        for (int l = 1; l < 8; l++) {
          if (element_type->doc_faces[f][l] != 0) {
            fprintf(geo, ", %d", element_type->doc_faces[f][l]);
          }
        }
        fprintf(geo, "};\n");
        fprintf(geo, "Plane Surface(%d) = {%d};\n", 1+f, 1+f);
      }
      fprintf(geo, "\n");
      
/*     
      if (element_type->dim > 2) {
        fprintf(geo, "xxx\n");
      }
*/
    }
  }
  
  fprintf(geo, "Geometry.PointLabels = 1;\n");
//  fprintf(geo, "Geometry.PointType = 1;\n");
  fprintf(geo, "Geometry.LabelType = 1;\n");
  fprintf(geo, "Geometry.PointSize = 20;\n");
  fprintf(geo, "Geometry.CurveWidth = 5;\n");
  fprintf(geo, "Geometry.Points = 1;\n");
  fprintf(geo, "Geometry.Surfaces = 0;\n");
  fprintf(geo, "Geometry.Volumes = 0;\n");

  fprintf(geo, "Geometry.Color.Points = {0,0,0};\n");
  fprintf(geo, "Geometry.Color.Curves = {0,0,0};\n");
// Geometry.SurfaceType = 2;


  
  fclose(geo);
  
  return 1;

}
