#!/bin/bash
cd $PILGRIM_PATH/src/Hierarchical-Trace-Format
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PILGRIM_PATH
make && make install
cd $PILGRIM_PATH/src/eztrace
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PILGRIM_PATH
make && make install
