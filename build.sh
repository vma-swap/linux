#!/bin/bash
make -j$(nproc) M=mm
#check return code and exit prematurely if it fails
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi
make -j$(nproc) bzImage
#check return code and exit prematurely if it fails
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi
sudo make install