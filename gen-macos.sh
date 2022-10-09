#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/.build/mac" \
    -G "Xcode" \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/mac"
    