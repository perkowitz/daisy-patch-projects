#!/bin/sh


echo $1

if [ "$1" == "" ]; then
	echo "Usage: dfu.sh <binfile>"
	exit
fi

#dfu-util -a 0 -s 0x08000000:leave -D $1 -d ,0483:df11
dfu-util -a 0 -s 0x08000000:leave -D ../libDaisy/core/dsy_bootloader_v6_2-intdfu-2000ms.bin -d ,0483:df11
