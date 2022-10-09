cmake ^
    -A x64 ^
    -S . ^
    -B "%cd%/.build/win-x64-static" ^
    -G "Visual Studio 17 2022" ^
    -DPOTTO_BUILD_STATIC=ON ^
    -DCMAKE_INSTALL_PREFIX:PATH="%cd%/out/install/win-x64-static"
