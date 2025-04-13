#!/bin/bash
make -j$(nproc)
#check return code and exit prematurely if it fails
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi
sudo make modules_install
sudo make install