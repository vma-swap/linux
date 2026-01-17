#!/bin/bash
# if install was passed
if [ "$1" == "install" ]; then
    echo -e "\033[0;32mInstalling release kernel...\033[0m"
    sudo make modules_install
    sudo make install
    exit 0
fi
if [ "$1" == "release" ]; then
# echo in color green
    echo -e "\033[0;32mBuilding release kernel...\033[0m"
    make ARCH=x86_64 -j$(nproc)
    if [ $? -ne 0 ]; then
        echo "Build failed"
        exit 1
    fi
    sudo make ARCH=x86_64 -j$(nproc) modules_install
    sudo make install
    exit 0
fi
echo -e "\033[0;32mBuilding dev kernel...\033[0m"
make ARCH=x86_64 -j$(nproc) bzImage
make ARCH=x86_64 -j$(nproc) modules
#check return code and exit prematurely if it fails
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi