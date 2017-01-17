#!/bin/bash
function abort {
    echo Make failed
    exit
}
cd libstarlight
make install || abort
cd ../testbed
make clean
make send || abort
cd ..
