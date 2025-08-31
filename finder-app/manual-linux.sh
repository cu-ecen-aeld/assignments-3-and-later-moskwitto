#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
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

<<<<<<< HEAD
    if [ -f ${FINDER_APP_DIR}/conf/.config ]
    then
    echo "Copying existing .config file from ${FINDER_APP_DIR}/conf/"
        # Copy config file
        cp ${FINDER_APP_DIR}/conf/.config ./.config
    else
        echo "No pre-generated .config file found, clean and apply configuration"
        # Clean configuration
        make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
        # Default configuration
        make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
        # If any extra kernel configurations needed, this is a good point to configure them

        echo "Copying generated .config file to ${FINDER_APP_DIR}/conf/"
        cp .config ${FINDER_APP_DIR}/conf/.config
    fi

    # Compile the kernel
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
    # Compile modules
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules
    # Build the device tree
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs

    echo "Copying output files to ${OUTDIR}"
    # Copy output files to outdir
    cd ${OUTDIR}/linux-stable/arch/${ARCH}/boot/
    cp Image ${OUTDIR}
=======
    # TODO: Add your kernel build steps here
>>>>>>> assignments-base/assignment6
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

<<<<<<< HEAD
# Create necessary base directories
mkdir ${OUTDIR}/rootfs
cd ${OUTDIR}/rootfs
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var usr/bin usr/lib usr/sbin var/log
=======
# TODO: Create necessary base directories
>>>>>>> assignments-base/assignment6

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
<<<<<<< HEAD
    # Configure busybox
    make distclean
    make defconfig
=======
    # TODO:  Configure busybox
>>>>>>> assignments-base/assignment6
else
    cd busybox
fi

<<<<<<< HEAD
# Make and install busybox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX="${OUTDIR}/rootfs" ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install
cd ${OUTDIR}/rootfs
=======
# TODO: Make and install busybox
>>>>>>> assignments-base/assignment6

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

<<<<<<< HEAD
# Add library dependencies to rootfs
COMPILER_PATH=$(dirname $(which ${CROSS_COMPILE}gcc))
cd ${COMPILER_PATH}/../${CROSS_COMPILE%-}/libc
cp lib/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib/
cp lib64/libm.so.6 lib64/libresolv.so.2 lib64/libc.so.6 ${OUTDIR}/rootfs/lib64/


# Make device nodes
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/null c 1 3
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/console c 5 1

# Clean and build the writer utility
cd ${FINDER_APP_DIR}
make clean
make CROSS_COMPILE=${CROSS_COMPILE}

# Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp writer ${OUTDIR}/rootfs/home/
cp finder.sh writer.sh finder-test.sh autorun-qemu.sh ${OUTDIR}/rootfs/home/
mkdir ${OUTDIR}/rootfs/home/conf
cp ./conf/assignment.txt ./conf/username.txt ${OUTDIR}/rootfs/home/conf

# Chown the root directory
cd ${OUTDIR}/rootfs
sudo chown -R root:root *

# Create initramfs.cpio.gz
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ${OUTDIR}
gzip -f initramfs.cpio
=======
# TODO: Add library dependencies to rootfs

# TODO: Make device nodes

# TODO: Clean and build the writer utility

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs

# TODO: Chown the root directory

# TODO: Create initramfs.cpio.gz
>>>>>>> assignments-base/assignment6
