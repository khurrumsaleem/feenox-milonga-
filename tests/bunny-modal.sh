#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
  exit 1;
fi

checkpde modal
checkslepc
checkgmsh

if [ ! -e ${dir}/bunny.msh ]; then
  if [ ! -e ${dir}/Stanford_Bunny.stl ] ; then
   if [ ! -z "$(which wget)" ]; then
     wget https://upload.wikimedia.org/wikipedia/commons/4/43/Stanford_Bunny.stl -O ${dir}/Stanford_Bunny.stl || exit $?
   else
     return 77
   fi
  fi 
  gmsh -3 ${dir}/bunny.geo || exit $?
fi

# gmsh -v 0 -3 ${dir}/bunny.geo -order 2 -o bunny-2nd.msh || exit $?

answer1 bunny-modal.fee fixed "0.97 1.10 1.57 1.63 2.05 2.28"
exitifwrong $?

answer1 bunny-modal.fee free  "1.56 1.67 2.22 2.42 3.26 3.39"
exitifwrong $?

answer1 bunny-modal.fee rest  "1.56 1.67 2.23 2.43 3.32 3.48"
exitifwrong $?
