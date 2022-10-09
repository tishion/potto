#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/.build/linux-static" \
    -G "Unix Makefiles" \
    -DPOTTO_BUILD_STATIC=ON \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/linux-static"
