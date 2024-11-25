#!/usr/bin/env bash

#if $CONDA_PREFIX is not set, ask user to activate conda environment
if [ -z "$CONDA_PREFIX" ]; then
  echo "Please activate conda environment first."
  exit 1
fi

cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX
cmake --build build --target install

# clean up
rm -rf build

echo "Success."
