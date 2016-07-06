#!/bin/bash

TOOLCHAIN="/opt/buildroot-gcc342/bin"
RUN_PLATFORM="mipsel-linux"

CC="$TOOLCHAIN/mipsel-linux-gcc"
LD="$TOOLCHAIN/mipsel-linux-ld"
AR="$TOOLCHAIN/mipsel-linux-ar"
STRIP="$TOOLCHAIN/mipsel-linux-strip"
NM="$TOOLCHAIN/mipsel-linux-nm"
RANLIB="$TOOLCHAIN/mipsel-linux-ranlib"
READELF="$TOOLCHAIN/mipsel-linux-readelf"
OBJCOPY="$TOOLCHAIN/mipsel-linux-objcopy"
CXX="$TOOLCHAIN/mipsel-linux-g++"
export CC LD AR STRIP NM RANLIB READELF OBJCOPY          	
	
./configure --host=mipsel-linux --target=mipsel-linux --build=x86_64-linux-gnu
