#!/bin/bash

prefix=$(dirname $(realpath $0))

LD_PRELOAD="$prefix/liblock.so" $@
