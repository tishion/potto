#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/out/build/mac" \
    -G "Xcode" \
    -DQN_BUILD_STATIC=ON \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/mac"
    