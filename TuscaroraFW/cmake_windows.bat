cd build
del /Q *.*
:: cmake -G "Visual Studio 12"  -D CMAKE_CXX_COMPILER="D:\Main\Repos\TestSystem\Compilers\GCC5.4.1\bin\arm-none-eabi-g++.exe" ..#
:: cmake -G "Unix Makefiles" ..
:: cmake -G "MSYS Makefiles" ..
cmake -G "MinGW Makefiles" ..

cd ..