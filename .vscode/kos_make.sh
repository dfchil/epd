#!/usr/bin/env bash


#set the KOS environtment variables
source /opt/toolchains/dc/kos/environ.sh

TARGET=mortarlity.elf

DCTRACE=1 ENJ_DEBUG=1 make clean
# SINGLEDEMO=0 DCPROF=1 SINGLEDEMO=7 
ENJ_DEBUG=1 ENJ_SHOWFRAMETIMES=1 ENJ_SUPERSAMPLING=1 ENJ_DEBUG=1 OPTLEVEL=g ENJ_CBASEPATH=/pc/mortarlity make -j 44
exit