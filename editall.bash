#! /bin/bash

export PREFIX=$(cat << EOM
# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

EOM
)

export POSTFIX=""

function digest_file {
  local file=$1
  local contents=$(cat $file)
  echo -e "${PREFIX}\n"      >  $file
  echo    "${contents}"      >> $file
  echo -e "\n${POSTFIX}"     >> $file
  echo "Processed file $file"
}

export -f digest_file

# find . -name "*.cpp" -exec bash -c 'digest_file "$0"' {} \;
# find . -name "*.hpp" -exec bash -c 'digest_file "$0"' {} \;
# find . -name "*.c"   -exec bash -c 'digest_file "$0"' {} \;
# find . -name "*.h"   -exec bash -c 'digest_file "$0"' {} \;

find . -name "CMakeLists.txt" -exec bash -c 'digest_file "$0"' {} \;
