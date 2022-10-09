cmake ^
    -A x64 ^
    -S . ^
    -B "%cd%/.build/win-x64" ^
    -G "Visual Studio 17 2022" ^
    -DCMAKE_INSTALL_PREFIX:PATH="%cd%/out/install/win-x64"
