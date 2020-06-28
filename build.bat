@echo off

rem Prepare environment.
set build_dir=build

mkdir %build_dir%
cd %build_dir%
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
cd ..
