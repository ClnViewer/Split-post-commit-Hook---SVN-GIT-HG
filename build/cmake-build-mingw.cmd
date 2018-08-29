
@rmdir /S /Q mingw
@mkdir mingw
@cd mingw
cmake -G "MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config "Release"
cmake --build . --target "install"
@cd ..
