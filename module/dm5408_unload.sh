#!/bin/sh

module="dm5408_mod"
device="dm5408"

rmmod $module $* || exit 1

rm -f /dev/${device} /dev/${device}[0-3]

