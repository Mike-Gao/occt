#!/bin/bash

if [ -f cmake_gen_custom.sh ]; then
  . cmake_gen_custom.sh
fi

DEB=
if [ -z "$BUILD_DIR" ]; then BUILD_DIR=build; fi
if [ -z "$GENERATOR" ]; then GENERATOR="Unix Makefiles"; fi
if [ -z "$OCCT3RDPARTY" ]; then OCCT3RDPARTY=`pwd`/../3rdparty; fi
if [ -z "$INSTALL_DIR" ]; then INSTALL_DIR=$OCCT3RDPARTY/occt740; fi
if [ -z "$FREETYPE_DIR" ]; then FREETYPE_DIR=$OCCT3RDPARTY/freetype-2.7.1; fi

CMAKE_BUILD_TYPE=Release
if [ "$1" = "-d" ]; then
  DEB=d
  BUILD_DIR=${BUILD_DIR}-deb
  BUILD_TYPE=Debug
fi

if [ -z "$USE_TBB" ]; then USE_TBB=ON; fi
if [ -z "$DIR_BIN" ]; then DIR_BIN=lin64/gcc/bin$DEB; fi
if [ -z "$DIR_LIB" ]; then DIR_LIB=lin64/gcc/lib$DEB; fi

USE_FPE_SIGNAL=ON
BUILD_DOC=OFF

if [ ! -d  $BUILD_DIR ]; then mkdir $BUILD_DIR; fi
cd $BUILD_DIR
 
cmake -G "$GENERATOR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DINSTALL_DIR_BIN:STRING=$DIR_BIN \
 -DINSTALL_DIR_LIB:STRING=$DIR_LIB -D3RDPARTY_DIR:STRING=$OCCT3RDPARTY \
 -DINSTALL_DIR_LAYOUT:STRING=Windows -DINSTALL_DIR:STRING=$INSTALL_DIR \
 -DBUILD_ENABLE_FPE_SIGNAL_HANDLER:BOOL=$USE_FPE_SIGNAL -DUSE_TBB:BOOL=$USE_TBB \
 -D3RDPARTY_FREETYPE_DIR:PATH=$FREETYPE_DIR -DBUILD_DOC_Overview:BOOL=$BUILD_DOC \
 $AUX_ARGS ..
