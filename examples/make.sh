#!/bin/bash

out=examples.md
rm -f ${out}
for i in lorenz nafems-le10 maze fibo_formula fibo_vector fibo_iterative; do
  echo ${i}
  grep '#\.' ${i}.fee | sed 's/#.//' | sed 's/^[ \t]*//' | \
    pandoc -t markdown   --lua-filter=../doc/include-files.lua \
                         --lua-filter=../doc/include-code-files.lua \
                         --lua-filter=../doc/not-in-format.lua \
                         --lua-filter=../doc/only-in-format.lua \
                         --lua-filter=../doc/img-width.lua >> ${out}
  echo >> ${out}
  
  echo >> ${out}
  echo '```feenox' >> ${out}
  cat ${i}.fee | grep -v '#\.' | grep -v '#\$' | grep -v '#>' >> ${out}
  echo '```' >> ${out}
  echo >> ${out}

  
  echo >> ${out}
  echo '```terminal' >> ${out}
  grep '#\$' ${i}.fee | sed 's/#\$//' | sed 's/^[ \t]*//' >> ${out}
#   bash ${i}-post.sh
#   rm -f ${i}-post.sh
  echo '```' >> ${out}
  echo >> ${out}
  
  echo >> ${out}
  grep '#>' ${i}.fee | sed 's/#>//' | sed 's/^[ \t]*//' >> ${out}
  echo >> ${out}
done
