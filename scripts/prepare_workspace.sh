#!/bin/bash

ROOT=${PWD}

PATH=${ROOT}/scripts/make_wrapper:$PATH
export PATH=$PATH
export WINEPATH="/usr/x86_64-w64-mingw32/bin/;${ROOT}/build/lib"
export LD_LIBRARY_PATH="${ROOT}/build/lib"
