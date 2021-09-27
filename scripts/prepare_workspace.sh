#!/bin/bash

make -C tools/modules/
PATH=$(realpath tools/modules/bin):$PATH
export PATH=$PATH
export WINEPATH="/usr/x86_64-w64-mingw32/bin/;$(realpath build/lib)"
export LD_LIBRARY_PATH="$(realpath build/lib)"
