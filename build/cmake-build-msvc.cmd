
@rmdir /S /Q msvs
@mkdir msvs
@cd msvs
cmake -G"Visual Studio 15 2017" .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config "Release" --target "ALL_BUILD"
@cd ..
