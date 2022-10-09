#!/bin/bash

cmake \
    -S . \
    -B "$(pwd)/.build/linux-debug" \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE:STRING="Debug" \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/install/linux-debug"
