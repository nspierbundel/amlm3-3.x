#! /bin/bash

#make UIMAGE_COMPRESSION=none uImage -j
make uImage -j
#make modules

make meson3_skt.dtd
make meson3_skt.dtb

#cd ../root/g18
#find .| cpio -o -H newc | gzip -9 > ../ramdisk.img

#rootfs.cpio -- original buildroot rootfs, busybox
#m8rootfs.cpio -- build from buildroot
ROOTFS="rootfs.cpio"

#cd ..
./mkbootimg --kernel ./arch/arm/boot/uImage --ramdisk ./${ROOTFS} --second ./arch/arm/boot/dts/amlogic/meson3_skt.dtb --output ./m3boot.img
ls -l ./m3boot.img
echo "m3boot.img done"
