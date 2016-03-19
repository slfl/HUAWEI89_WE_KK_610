#!/bin/bash

###############################################################################
# To all DEV around the world via Mansi :)                                    #
#                                                                             #
# 1.) use the "bash"                                                          #
# chsh -s /bin/bash `whoami`                                                  #
#                                                                             #
# 2.) delete old files                                                        #
# rm -r out                                                                   #
#                                                                             #
# 3.) now you can build my kernel                                             #
# ./build.sh                                                                  #
#                                                                             #
# Have fun and update me if something nice can be added to my source.         #
###############################################################################

OUT_DIR="out"
KERNEL_DIR=$PWD
TYPE=huawei89_we_kk
KERN_IMG=${OUT_DIR}/target/product/${TYPE}/obj/KERNEL_OBJ/arch/arm/boot/zImage
BUILD_START=$(date +"%s")

#Set Color
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'

###############################################################################
# Set configs
###############################################################################

# Name a kernel
export KBUILD_BUILD_USER="Mansi"
export KBUILD_BUILD_HOST="MSI"

# Arch
export ARCH=arm
export SUBARCH=arm

# gcc 4.7.7 
export CROSS_COMPILE=~/toolchains/arm-linux-androideabi-4.7.7/bin/arm-linux-androideabi-
#export CROSS_COMPILE=~/toolchains/arm-linux-androideabi-4.9.4/bin/arm-cortex_a7-linux-gnueabihf-

# Other
export JAVA_HOME=/usr/lib/jvm/java-6-oracle
export USE_CCACHE=1

mkdir ${OUT_DIR}


###############################################################################
# Build system
###############################################################################
echo -e "$cyan Clean old files $nocol";
	rm ${OUT_DIR}/target/product/${TYPE}/obj/KERNEL_OBJ/arch/arm/boot/Image
	rm ${KERN_IMG}
	rm ${KERNEL_DIR}/Mansi/Output/zImage

echo -e "$cyan Build kernel $nocol";
	./makeMtk -t -o=TARGET_BUILD_VARIANT=user ${TYPE} n k

if ! [ -a $KERN_IMG ]; then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol";
	exit 1
fi


###############################################################################
# Other configs, after build
###############################################################################

echo -e "$cyan Copy kernel $nocol";
	cp ${KERN_IMG}  ${KERNEL_DIR}/Mansi/Output/zImage
	cd ${KERNEL_DIR}/Mansi/Output/

echo -e "$cyan Build flash file $nocol";
	zipfile="Mansi_M40_($(date +"%d-%m-%Y(%H.%M%p)")).zip"
	zip -r ../${zipfile} ramdisk anykernel.sh zImage patch tools META-INF -x *kernel/.gitignore*

	BUILD_END=$(date +"%s")
	DIFF=$(($BUILD_END - $BUILD_START))

echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol";

echo "Enjoy Mansi kernel";
