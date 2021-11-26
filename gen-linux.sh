#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/out/build/linux" \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/linux"
