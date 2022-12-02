#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}


    # TODO: Add your kernel build steps here
    #cd linux-stable
    #if [! -e crosstool-ng ]
    #then
    #git clone https://github.com/crosstool-ng/crosstool-ng
    #fi
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- INSTALL_MOD_PATH=${OUTDIR}
    
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
    mkdir ~/rootfs
    cd ~/rootfs
    mkdir bin dev etc lib proc sbin sys tmp usr var
    mkdir usr/bin usr/lib usr/sbin
    mkdir var/log
    mkdir bin/sh
    cp /bin/bash ./bin/bash

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
    git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
    make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi-

else
    cd busybox
fi

# TODO: Make and install busybox
    make distclean
    make defconfig
    make ARCH=arm CROSS_COMPILE=arm-unknown-linux-gnueabi-
    make ARCH=arm CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf- install

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
    cd ~/rootfs

    arm-cortex_a8-linux-gnueabihf-readelf -a bin/busybox | grep "program interpreter"
    arm-cortex_a8-linux-gnueabihf-readelf -a bin/busybox | grep "Shared library"
    export SYSROOT=$("arm-cortex_a8-linux-gnueabihf-gcc -print-sysroot")
    cp -a $SYSROOT/lib/ld-linux-armhf.so.3 lib
    cp -a $SYSROOT/lib/ld-2.22.so lib
    cp -a $SYSROOT/lib/libc.so.6 lib
    cp -a $SYSROOT/lib/libc-2.22.so lib
    cp -a $SYSROOT/lib/libm.so.6 lib
    cp -a $SYSROOT/lib/libm-2.22.so lib
# TODO: Make device nodes
    cd ~rootfs
    sudo mknod -m 666 dev/null c 1 3
    sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
    cmake -S ${FINDER_APP_DIR} -B ${OUTDIR}/rootfs/home
    make

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
    cp ${FINDER_APP_DIR}/finder.sh ${OUTDIR}/rootfs/home
    cp ${FINDER_APP_DIR}/conf/username.txt ${OUTDIR}/rootfs/home
    cp ${FINDER_APP_DIR}/autorun-quemu.sh ${OUTDIR}/rootfs/home

# TODO: Chown the root directory
    cd ~/rootfs
    sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
    cd ~rootfs
    find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
    gzip initramfs.cpio
    mkimage -A arm -O linux -T ramdisk -d initramfs.cpio.gz uRamdisk


    cd ${FINDER_APP_DIR}
    .${FINDER_APP_DIR}/start-quemu-terminal.sh
