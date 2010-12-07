#!/bin/sh

module="dm5408_mod"
device="dm5408"
mode="664"

insmod ./$module.ko $* || exit 1

rm -f /dev/${device}[0-3]

major=$(awk "/$device/ {print \$1}" /proc/devices)

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

group="staff"
grep -q '^staff:' /etc/group || group="wheel"
chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]
