#!/bin/bash

cmake \
    -S . \
    -B .build/mac \
    -G "Xcode"
    