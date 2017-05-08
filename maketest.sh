#!/bin/bash
function abort {
    echo Make or send failed
    exit
}
mode=send
if [ "$1" = "sc" ]; then
    mode=send-cia
elif [ "$1" = "c" ]; then
    mode=run
fi
cd libstarlight
make install || abort
cd ../testbed
make clean
make $mode || abort
cd ..
