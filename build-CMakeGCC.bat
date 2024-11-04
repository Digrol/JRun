cmake -S . -B ./~buildWinGCC -DCMAKE_TOOLCHAIN_FILE=./i686.cmake -G "MSYS Makefiles"
::cmake -S . -B ./~buildWinGCC -DCMAKE_TOOLCHAIN_FILE=./x86_64.cmake -G "MSYS Makefiles"
cmake --build ./~buildWinGCC
