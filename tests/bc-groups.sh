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

checkpde mechanical
checkgmsh

gmsh -v 0 -3 ${dir}/cube.geo || exit $?

for i in $(seq 1 4); do
  answerdiff bc-groups${i}.fee --ast
  exitifwrong $?
done

