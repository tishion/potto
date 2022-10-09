cmake ^
    -A Win32 ^
    -S . ^
    -B "%cd%/.build/win-x86-static" ^
    -G "Visual Studio 17 2022" ^
    -DPOTTO_BUILD_STATIC=ON ^
    -DCMAKE_INSTALL_PREFIX:PATH="%cd%/out/install/win-x86-static"
