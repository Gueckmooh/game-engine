#!/bin/bash

P3XML=$(find modules/3p -name "*.xml" -print)

if ! [[ -f ./tools/modules/bin/get-3p-module ]]
then
    make -C ./tools/module
fi

for file in "$P3XML"
do
    ./tools/modules/bin/get-3p-module "$file"
done
