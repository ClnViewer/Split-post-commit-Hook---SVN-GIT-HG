
@rmdir /S /Q msvc
@mkdir msvc
@cd msvc
# cmake -G"Visual Studio 10 2010" .. -DCMAKE_BUILD_TYPE=Release
cmake -G "Visual Studio 15 2017" .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config "Release" --target "ALL_BUILD"
@cd ..
