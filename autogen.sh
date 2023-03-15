#!/bin/sh
# 
# Execute this script to generate a configure script
#
# This file is free software: you are free to change and redistribute it.
# There is NO WARRANTY, to the extent permitted by law.
#

# check for needed tools
for i in git autoconf m4 make; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

if test ! -d ".git"; then
  echo "error: this tree is not a repository (did you download instead of clone?)" 
  exit 1
fi

./autoclean.sh
cat << EOF > ./src/help.h
#define FEENOX_HELP_ONE_LINER     "$(cat doc/help-one-liner.txt)"
#define FEENOX_HELP_USAGE         "$(cat doc/help-usage.txt)"
#define FEENOX_HELP_OPTIONS_BASE  "\\
$(awk '{printf("%s\\n\\\n", $0)}' doc/help-options-base.txt)
"
#define FEENOX_HELP_OPTIONS_PDE   "\\
$(awk '{printf("%s\\n\\\n", $0)}' doc/help-options-pde.txt)
"
#define FEENOX_HELP_EXTRA          "\\
$(awk '{printf("%s\\n\\\n", $0)}' doc/help-extra.txt)
"
EOF


# check if there are tags
if [ ! -z "$(git tag)" ]; then
  what="tags"
else
  what="all"
fi
version=$(git describe --${what} | sed 's/-/./' | cut -d- -f1 | tr -d v)
echo "define(feenoxversion, ${version})dnl" > version.m4

rm -f config_links.m4
for i in tests/*.sh tests/*.fee tests/*.geo tests/*.msh tests/*.dat tests/*.ref; do
 echo "AC_CONFIG_LINKS([${i}:${i}])" >> config_links.m4
done

# do not add doc to src unless they ask for it
echo -n "creating Makefile.am... "
if [ "x${1}" = "x--doc" ]; then
  cp Makefile-doc.am Makefile.am
else
  echo "SUBDIRS = src" > Makefile.am
fi

cat Makefile-base.am >> Makefile.am
touch doc/feenox-desc.texi
echo "ok"


# detect the sources in pdes
echo -n "creating src/Makefile.am... "

cd src/pdes

automatic="// automatically generated by autogen.sh on $(date)"

echo ${automatic} > methods.h
echo ${automatic} > parse.c
echo ${automatic} > available.h

cat << EOF >> parse.c
#include "feenox.h"

int feenox_pde_parse_problem_type(const char *token) {

///kw_pde+PROBLEM+desc Ask FeenoX to solve a partial differential equation problem.
///kw_pde+PROBLEM+usage PROBLEM
///kw_pde+PROBLEM+detail Currently, FeenoX can solve the following types of PDE-casted problems:
///kw_pde+PROBLEM+detail @
EOF

echo "#define AVAILABLE_PDES \"\\" >> available.h

first=1
for pde in *; do
 if [ -d ${pde} ]; then
  if [ -e ${pde}/methods.h ] && [ -e ${pde}/init.c ]; then

    echo "#include \"${pde}/methods.h\"" >> methods.h
    echo "${pde}\\\\n\\" >> available.h

    if [ ${first} -eq 0 ]; then
      sep="|"
    else
      sep="{"
    fi
  
    echo "///kw_pde+PROBLEM+usage ${sep} ${pde}" >> parse.c
    grep "///kw_pde+PROBLEM+detail" ${pde}/init.c >> parse.c

    if [ ${first} -eq 0 ]; then
      echo -n "  } else " >> parse.c
    else
      echo -n "  " >> parse.c
    fi
    cat << EOF >> parse.c
if (strcasecmp(token, "${pde}") == 0) {
    feenox.pde.parse_particular = feenox_problem_parse_problem_${pde};
    feenox.pde.init_parser_particular = feenox_problem_init_parser_${pde};
    
EOF
    
    first=0
  else
    echo "warning: ${pde} does not contain methods.h and/or init.c"
  fi  
 fi
done

cat << EOF >> parse.c      
  } else {
    feenox_push_error_message("unknown problem type '%s'", token);
    return FEENOX_ERROR;
      
  }
  
///kw_pde+PROBLEM+usage }@
///kw_pde+PROBLEM+detail @
///kw_pde+PROBLEM+detail > If you are a programmer and want to contribute with another problem type, please do so!
///kw_pde+PROBLEM+detail > Check out the [programming guide in the FeenoX repository](https://github.com/seamplex/feenox/blob/main/doc/programming.md).
///kw_pde+PROBLEM+detail @
    
  return FEENOX_OK;
}
EOF
echo "\"" >> available.h

cd ..

cp Makefile-base.am Makefile.am
find pdes \( -name "*.c" -o -name "*.h" \) | xargs echo -n >> Makefile.am
cd ..
echo "ok"

# changelog
if [ ! -z "$(which pandoc)" ]; then
  pandoc ChangeLog.md -t plain -o ChangeLog
else
  cp ChangeLog.md ChangeLog
fi

echo "calling autoreconf... "
autoreconf -i
echo "done"


