#!/bin/sh

MAJOR=242
for i in `seq 0 31`
do
  mknod /dev/so2011_$i c $MAJOR $i
  chmod 666 /dev/so2011_$i
done
mknod /dev/so2011_ctrl c $MAJOR 32
chmod 666 /dev/so2011_ctrl

exit 0

