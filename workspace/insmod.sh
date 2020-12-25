#!/bin/sh


mknod /dev/lcd c 120 0
insmod /lib/modules/2.6.15.3/kernel/drivers/char/creator-pxa270-lcd.ko