#!/bin/bash

cd $(dirname "$0")/..
source scripts/prepare_workspace.sh

if test $# -ge 1
then
    SESSIONNAME="$1"
else
    SESSIONNAME="engine"
fi

tmux new -s "$SESSIONNAME" \; \
     setenv PATH $PATH \; \
     setenv WINEPATH "$WINEPATH" \; \
     setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH"
