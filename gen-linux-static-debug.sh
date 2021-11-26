#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/out/build/linux-static-debug" \
    -G "Unix Makefiles" \
    -DPOTTO_BUILD_STATIC=ON \
    -DCMAKE_BUILD_TYPE:STRING="Debug" \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/linux-static-debug"
