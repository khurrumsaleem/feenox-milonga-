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

checkslepc

answer modal-beam-cantilever.fee "43.8 61.6 114.7 158.0 211.2 232.0"
exitifwrong $?

