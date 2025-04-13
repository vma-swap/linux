#!/bin/bash

DEBUG_MODE=0

# Check for the -g flag
while getopts "g" opt; do
  case $opt in
    g)
      DEBUG_MODE=1
      ;;
    *)
      ;;
  esac
done

# Build the QEMU command
QEMU_CMD="qemu-system-x86_64 \
        -m 5G \
        -smp 2 \
        -kernel ./arch/x86/boot/bzImage \
        -append \"console=ttyS0 root=/dev/sda earlyprintk=serial net.ifnames=0 nokaslr\" \
        -drive file=image/bullseye.img,format=raw \
        -drive file=/scratch/vm_swap.img,format=raw,if=virtio \
        -netdev user,id=net0,hostfwd=tcp:127.0.0.1:10021-:22 \
        -device virtio-net-pci,netdev=net0 \
        -enable-kvm \
        -nographic \
        -pidfile vm.pid"

# Add debugging options if -g is passed
if [ $DEBUG_MODE -eq 1 ]; then
  QEMU_CMD="$QEMU_CMD -s -S"
fi

# Run the QEMU command and log output
eval $QEMU_CMD 2>&1 | tee vm.log