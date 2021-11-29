#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/out/build/mac" \
    -G "Xcode" \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/mac"
    