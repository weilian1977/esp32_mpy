#!/bin/bash

function check_cmd() {
    local command=("$@")
 
    if "${command[@]}"; then
        return  0
    else
        return  1
    fi
}

cd ../../submodule/micropython
check_cmd git apply --check ../../project/matatacar/tools/patch/patch_micropython.diff
if [ $? -eq  0 ]; then
    echo "micropython patch check ok, then apply it"
    git apply ../../project/matatacar/tools/patch/patch_micropython.diff
else
    echo "micropython patch check failed!"
fi

cd ../../submodule/esp-idf
check_cmd git apply --check ../../project/matatacar/tools/patch/patch_esp-idf.diff
if [ $? -eq  0 ]; then
    echo "esp-idf patch check ok, then apply it"
    git apply ../../project/matatacar/tools/patch/patch_esp-idf.diff
else
    echo "esp-idf patch check failed!"
fi