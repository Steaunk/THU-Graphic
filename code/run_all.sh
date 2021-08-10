#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
else
    rm -r build
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

# Build project.
cd build
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
# bin/work testcases/scene01_basic.txt output/scene01.bmp
# bin/work testcases/scene02_cube.txt output/scene02.bmp
# bin/work testcases/scene03_sphere.txt output/scene03.bmp
# bin/work testcases/scene04_axes.txt output/scene04.bmp
# bin/work testcases/scene05_bunny_200.txt output/scene05.bmp 10
# bin/work testcases/scene06_bunny_1k.txt output/scene06.bmp 5
# bin/work testcases/scene07_shine.txt output/scene07.bmp
# bin/work testcases/scene08_core.txt output/scene08.bmp
# bin/work testcases/scene09_norm.txt output/scene09.bmp
# bin/work testcases/vase.txt output/vase.bmp
# bin/work testcases/scene10_wineglass.txt output/scene10.bmp
# time bin/work testcases/test.txt output/test.bmp 100
time bin/work testcases/A.txt output/A.bmp 2000
# time bin/work testcases/twoSphere.txt output/twoSphere.bmp 1000
# time bin/work testcases/earth.txt output/earth.bmp 10000
# time bin/work testcases/metal.txt output/metal.bmp 100
