#!/bin/bash

TARGET=$1

dd if=/dev/zero of=$TARGET bs=512 count=2880 >/dev/null

STAGE2_SIZE=$(stat -c%s ${BUILD_DIR}/stage2.bin)
echo ${STAGE2_SIZE}
STAGE2_SECTORS=$(( ( ${STAGE2_SIZE} + 511 ) / 512 ))
echo ${STAGE2_SECTORS}

sudo mkfs.fat -F 12 -R ${STAGE2_SECTORS} -n "NBOS" $TARGET >/dev/null
dd if=${BUILD_DIR}/stage1.bin of=$TARGET conv=notrunc bs=1 count=3 >/dev/null
dd if=${BUILD_DIR}/stage1.bin of=$TARGET conv=notrunc bs=1 seek=62 skip=62 >/dev/null
dd if=${BUILD_DIR}/stage2.bin of=$TARGET conv=notrunc bs=512 skip=1

mcopy -i $TARGET ${BUILD_DIR}/kernel.bin "::kernel.bin"
mcopy -i $TARGET test.txt "::test.txt"
mmd -i $TARGET "::mydir"
mcopy -i $TARGET test.txt "::mydir/test.txt"