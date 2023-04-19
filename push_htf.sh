#!/bin/bash
rsync -r src bran:software/src/Hierarchical-Trace-Format
ssh bran "source .env &&
  cd software/src/Hierarchical-Trace-Format &&
  rm -rf build && mkdir build && cd build &&
  cmake .. -DCMAKE_INSTALL_PREFIX=\$PILGRIM_PATH &&
  make && make install"
ssh bran "source .env &&
  cd software/src/eztrace &&
  rm -rf build && mkdir build && cd build &&
  cmake .. -DCMAKE_INSTALL_PREFIX=\$PILGRIM_PATH &&
  make && make install"
