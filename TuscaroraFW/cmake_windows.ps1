param(
    [Parameter()][string] $plat = "eMote"   
)

$platOptions = @("eMote", "x86_64", "native", "RPi")

if ($plat -In $platOptions)
{}
else {
    Write-Host "Unrecognized or unsupported platform. Supported platform are: $platOptions"
    exit
}
 

Write-Host "Building for platform: $plat "


cd build
#del /Q *.*
Remove-Item .\* -recurse
# cmake -G "Visual Studio 12"  -D CMAKE_CXX_COMPILER="D:\Main\Repos\TestSystem\Compilers\GCC5.4.1\bin\arm-none-eabi-g++.exe" ..#
# cmake -G "Unix Makefiles" ..
# cmake -G "MSYS Makefiles" ..
cmake -G "MinGW Makefiles" -DARCH="$plat" ..

cd ..	