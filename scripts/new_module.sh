#!/bin/bash

# @todo compile tools
echo "### Creation of a new module"

yes-no-p() {
    local isok=0
    local default="$2"
    local message="$1"
    local res=""
    local ret=""
    while [[ $isok == 0 ]]
    do
        read -p "$message" res
        if [[ $res == "y" ]] || [[ $res == "Y" ]]
        then
            isok=1
            ret=1
        elif [[ $res == "n" ]] || [[ $res == "N" ]]
        then
            isok=1
            ret=0
        elif [[ $res == "" ]]
        then
            isok=1
            ret=$default
        fi
    done
    echo $ret
}

read -p "Module name: " -e name
read -p "Module base dir: " -e -i "$name" module_dir
read -p "Module export dir: " -e -i "$name" export_dir
read -p "Module type (shared_library|executable|headers_only): " -e -i "shared_library" type
is_third_party=$(yes-no-p "Is a third party module? [y/N] " 0)

if [[ "$is_third_party" == 1 ]]
then
    read -p "Git repository of the module: " -e git_repo
else
    read -p "Module dependencies (comma separated list): " -e deps
fi

echo -e "\n##########"
echo "Generate module"
echo "Name: $name"
echo "Module dir: $module_dir"
echo "Export dir: $export_dir"
if [[ "$is_third_party" == 1 ]]
then
    echo "From: '$git_repo'"
else
    echo "With dependencies: $deps"
fi
generate=$(yes-no-p "Generate this module? [y/N] " 0)

if [[ "$generate" == 1 ]]
then
    if [[ "$is_third_party" == 1 ]]
    then
        extra_options=("-third-party")
    elif [[ "$deps" != "" ]]
    then
        extra_options=("-dependencies" "$deps")
    fi
    echo running ./tools/bin/create-module -name "$name" -module-directory "$module_dir" -module-export-dir "$export_dir" -type "$type" ${extra_options[@]}
    ./tools/bin/create-module -name "$name" -module-directory "$module_dir" -module-export-dir "$export_dir" -type "$type" ${extra_options[@]}
fi
