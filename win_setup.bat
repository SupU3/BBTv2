@echo off

if not exist build mkdir build

cd build

cmake -DSDL_STATIC=On -DSDL_SHARED=Off ..

cd ..

pause
