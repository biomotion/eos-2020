#!/bin/sh

set -x
# set -e

# rmmod -f mydev
# insmod mydev.ko

./writer BENSON &
./reader 10.42.0.1 8989 /dev/mydev
