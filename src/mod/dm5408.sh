#!/bin/bash

### BEGIN INIT INFO
# Provides:         dm5408
# Required-Start:
# Required-Stop:
# Should-Start:
# Should-Stop:
# X-Start-Before:
# X-Stop-After:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     false
# Short-Description: dm5408 devices creation script.
# Description:       dm5408 devices creation script.
# Checks if dm5408 dkms module is loaded then it makes 4 DM5408 device
# nodes in /dev/ folder.
### END INIT INFO

module="dm5408"
device="dm5408"
mode="664"

function module_faild()
{
echo "Faild"
exit 1
}

function load_device()
{
modprobe ${module} || module_faild

rm -f /dev/${device}[0-3]

major=$(awk "/${device}/ {print \$1}" /proc/devices)

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

group="staff"
grep -q '^staff:' /etc/group || group="wheel"
chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]
}

function unload_device()
{
modprobe -r ${module} || module_faild

rm -f /dev/${device} /dev/${device}[0-3]
}

case "$1" in
  start)
     echo -n "Loading ${module} module..."
     load_device
     echo "OK"
     ;;
  stop)
     echo -n "Unloading ${module} module..."
     unload_device
     echo "OK"
     ;;
  force-reload|restart)
     echo -n "Reloading ${module} module..."
     unload_device
     load_device
     echo "OK"
     ;;
  status)
    echo "${module} module status..."
    echo "dkms:"
    dkms status -m ${module}
    echo "${device} device Major:"
    more /proc/devices | grep ${device}
    echo "${device} char devices:"
    ls -l /dev/${device}*
    ;;
  *)
    echo "Usage: $0 {start|stop|restart|force-reload|status}"
    exit 1
esac

exit 0
