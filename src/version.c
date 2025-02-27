/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX version and description routines
 *
 *  Copyright (C) 2009--2025 Jeremy Theler
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
#include "feenox.h"

#include "help.h"
#include "pdes/available.h"

#include <stdio.h>
#include <gsl/gsl_version.h>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif

#ifdef HAVE_SUNDIALS
#include <sundials/sundials_version.h>
#endif

#ifdef HAVE_PETSC
#include <petscsys.h>
#include <petscconfiginfo.h>
#endif
#ifdef HAVE_SLEPC
#include <slepcsys.h>
#endif

void feenox_show_help(const char *progname, int extra) {
  // in parallel runs only print from first processor
  if (feenox.mpi_rank != 0) {
    return;
  }
  
  // TODO: use a defined string with %s
  printf("usage: %s %s\n\n", progname, FEENOX_HELP_USAGE);
  printf("%s\n", FEENOX_HELP_OPTIONS_BASE);

  if (extra) {
    printf("%s\n", FEENOX_HELP_OPTIONS_PDE);
    printf("%s\n", FEENOX_HELP_EXTRA);
  } else {
    printf("Run with --help for further explanations.\n");
  }
  
  if (extra) {
    printf("Report bugs at https://github.com/seamplex/feenox/issues\n");
    printf("Ask questions at https://github.com/seamplex/feenox/discussions\n");
    printf("Feenox home page: https://www.seamplex.com/feenox/\n");
  }  
  
  return;
}


void feenox_show_version(int version) {
  
  // in parallel runs only print from first processor
  if (feenox.mpi_rank != 0) {
    return;
  }
  
  if (version == version_available_pdes) {
#ifdef HAVE_PETSC
    printf(AVAILABLE_PDES);
#else
    printf("FeenoX was not linked against PETSc. No PDEs are available.\n");
#endif
    return;
  } else if (version == version_elements_info) {
    feenox_mesh_element_types_init();
    feenox_mesh_elements_info();
    return;
  }
  
  feenox_shortversion();

  if (version) {
    printf("\n");
  }
     
  switch (version) {
    case version_copyright:
      feenox_copyright();
    break;
    case version_info:
      feenox_longversion();
    break;
  }
    

  fflush(stdout);

  return;
}

void feenox_shortversion(void) {

  // in parallel runs only print from first processor
  if (feenox.mpi_rank != 0) {
    return;
  }
  
  printf("FeenoX ");
#ifdef FEENOX_GIT_BRANCH
  printf("%s %s\n", FEENOX_GIT_VERSION, strcmp(FEENOX_GIT_BRANCH, "main") ? FEENOX_GIT_BRANCH : "");
#else
  printf("%s\n", PACKAGE_VERSION);
#endif

  printf("%s\n", FEENOX_HELP_ONE_LINER);

  return;
}


void feenox_copyright(void) {

  // in parallel runs only print from first processor
  if (feenox.mpi_rank != 0) {
    return;
  }

  /* It is important to separate the year from the rest of the message,
     as done here, to avoid having to retranslate the message when a new
     year comes around.  */  
  printf("Copyright © %d--%d Jeremy Theler, https://seamplex.com/feenox\n\
GNU General Public License v3+, https://www.gnu.org/licenses/gpl.html. \n\
FeenoX is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n", 2009, 2025); 
}

void feenox_longversion(void) {
  
  // in parallel runs only print from first processor
  if (feenox.mpi_rank != 0) {
    return;
  }

#ifdef FEENOX_GIT_DATE
  printf("Last commit date   : %s\n", FEENOX_GIT_DATE);
#endif
  
// these guys here prevent reproducible builds in debian
/*
#ifdef FEENOX_COMPILATION_DATE
  printf("Build date         : %s\n", FEENOX_COMPILATION_DATE);
#endif
#ifdef FEENOX_COMPILATION_USERNAME
  printf("Builder            : %s@%s\n", FEENOX_COMPILATION_USERNAME, FEENOX_COMPILATION_HOSTNAME);
#endif  
*/
#ifdef FEENOX_COMPILER_ARCH
  printf("Build architecture : %s\n", FEENOX_COMPILER_ARCH);
#endif
#ifdef FEENOX_COMPILER_VERSION
  printf("Compiler version   : %s\n", FEENOX_COMPILER_VERSION);
#endif
#ifdef FEENOX_COMPILER_SHOW
  printf("Compiler expansion : %s\n", FEENOX_COMPILER_SHOW);
#endif
#ifdef FEENOX_COMPILER_CFLAGS
  printf("Compiler flags     : %s\n", FEENOX_COMPILER_CFLAGS);
#endif
  
  printf("GSL version        : %s\n", gsl_version);
  
#ifdef HAVE_SUNDIALS
  char sundials_version[BUFFER_TOKEN_SIZE];
  SUNDIALSGetVersion(sundials_version, BUFFER_TOKEN_SIZE-1);
#else
  char *sundials_version = "N/A";
#endif
  printf("SUNDIALS version   : %s\n", sundials_version);
  
//   printf("Readline version   : %s\n", 
// #if HAVE_READLINE
//          rl_library_version
// #else
//          "N/A"
// #endif
//   );


#ifdef HAVE_PETSC
  char petscversion[BUFFER_TOKEN_SIZE];
  char petscarch[BUFFER_TOKEN_SIZE];
  
  PetscGetVersion(petscversion, BUFFER_TOKEN_SIZE);
  printf("PETSc version      : %s\n", petscversion);
  PetscGetArchType(petscarch, BUFFER_TOKEN_SIZE);
  if (strlen(petscarch) > 0) {
    printf("PETSc arch         : %s\n", petscarch);
  }  
  printf("PETSc options      : %s\n", petscconfigureoptions);
#else
  printf("PETSc version      : N/A\n");
#endif      
  
#ifdef HAVE_SLEPC
  char slepcversion[BUFFER_TOKEN_SIZE];
  SlepcGetVersion(slepcversion, BUFFER_TOKEN_SIZE);  
#else
  char *slepcversion = "N/A";
#endif
  printf("SLEPc version      : %s\n", slepcversion);

  return;
}
