# HTF

HTF provides an interface to write and read trace data.

## Building

```
cmake . -DCMAKE_INSTALL_PREFIX=$INSTALLATION_DIR
make install
```

## Using HTF

HTF implements a subset of the [OTF2](https://www.vi-hps.org/projects/score-p) API.
It also implements the [Murmur3 hasing function](https://github.com/PeterScott/murmur3).

Here are the env variables you can change to modify how HTF works:

- VERBOSE: error, quiet, normal, verbose, debug or help
- COMPRESSION: ZSTD, MASKING, MASKING_ZSTD, NONE
- STORE_TIMESTAMPS: TRUE or anything else
- STORE_HASHING: FALSE or anything else


## Contributing

Contribution to HTF are welcome. Just send us a pull request.

## License
OTF2-Profile is distributed under the terms of both the BSD 3-Clause license.

See [LICENSE](LICENSE) for details
