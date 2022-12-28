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

checkgmsh

if [ ! -e ${dir}/i-beam-hex.msh ]; then
  gmsh -3 ${dir}/i-beam-hex.geo || exit $?
  gmsh -3 ${dir}/i-beam-tet.geo || exit $?
fi

answer1zero moment-of-inertia.fee hex
exitifwrong $?

answer1zero moment-of-inertia.fee tet
exitifwrong $?
