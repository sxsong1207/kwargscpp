#!/usr/bin/env bash

# get folder of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR

# build and install the package into `install` directory
cmake -Blibbuild -DBUILD_PYTHON=OFF -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=libinstall ../..
cmake --build libbuild --target install

# run the installed package
cmake -Bexebuild -Dkwargscpp_DIR=../libinstall/lib/cmake/kwargscpp/ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON caller
cmake --build exebuild

# run the and check with the expected output

EXPECTED_OUTPUT="{\"key\": \"value\"}"
OUTPUT=$(./exebuild/kwargscpp_caller)

echo "EXPECTED_OUTPUT: $EXPECTED_OUTPUT"
echo "OUTPUT: $OUTPUT"

if [ "$EXPECTED_OUTPUT" == "$OUTPUT" ]; then
  echo "Test passed"
  # cleanup 
  rm -rf libbuild exebuild libinstall

  exit 0
else
  echo "Test failed"
  exit 1
fi
