cmake ^
    -A Win32 ^
    -S . ^
    -B "%cd%/out/build/win-x86" ^
    -G "Visual Studio 17 2022" ^
    -DCMAKE_INSTALL_PREFIX:PATH="%cd%/out/install/win-x86"
