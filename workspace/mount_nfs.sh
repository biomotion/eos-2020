#!/bin/sh

ifconfig eth0 up
ifconfig eth0 10.42.0.100 broadcast 10.42.0.255 netmask 255.255.255.0
route add default gw 10.42.0.1

portmap&
mount 10.42.0.1:/home/biomotion/eos-2020/workspace /mnt