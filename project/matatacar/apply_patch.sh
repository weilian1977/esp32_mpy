#!/bin/bash
cd ../../submodule/micropython
git apply ../../project/oak_lite/tools/patch/patch_micropython.diff

cd ../../submodule/esp-idf
git apply ../../project/oak_lite/tools/patch/patch_esp-idf.diff