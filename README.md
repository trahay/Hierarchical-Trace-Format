# HTF

HTF provides an interface to write and read trace data.

## Building

```
mkdir build && cd dir
cmake .. -DCMAKE_INSTALL_PREFIX=$INSTALLATION_DIR
make -j 14 && make install
```

If you want to enable SZ and ZFP, you should install them, and then add `-DSZ_ROOT_DIR=<your SZ installation>`
and `-DZFP_ROOT_DIR=<your ZFP installation>` to the cmake command line.

## About

HTF implements a subset of the [OTF2](https://www.vi-hps.org/projects/score-p) API.
It also implements the [Murmur3 hashing function](https://github.com/PeterScott/murmur3).

## Usage

After compiling HTF, install [ezTrace](https://eztrace.gitlab.io/eztrace).
Make sure you enable the relevant modules, and make sure to have the compiled HTF libs in your library path.
Then trace any program by running `mpirun -np N eztrace -t <your modules> <your programm>`.
The trace file will be generated in the `<your programm>_trace` folder.

## Configuration

A JSON Config file can be given to HTF with the CONFIG_FILE_PATH environment variable.
If that variable is empty, HTF will try to load a config.json in the current directory.
If that file does not exist, a default config will be loaded.
That file is a mirror image of the ParameterHandler class in `libraries/htf/src/ParameterHandler.h`
An example config file is given here as config.json.

Here are the configuration options with specific values:

- `compressionAlgorithm`: Specifies which compression algorithm is used for storing the timestamps. Its values are:
  - `None`
  - `ZSTD`
  - `SZ`
  - `ZFP`
  - `Histogram`
- `encodingAlgorithm`: Specifies which encoding algorithm is used for storing the timestamps. If the specified
  compression algorithm is lossy, this is defaulted to None. Its values are:
  - `None`
  - `Masking`
  - `LeadingZeroes`
- `loopFindingAlgorithm`: Specifies what loop-finding algorithm is used. Its values are:
  - `None`
  - `Basic`
  - `BasicTruncated`
  - `Filter`

Here are the configuration options with number values:

- `zstdCompressionLevel`: Specifies the compression level used by ZSTD. Integer.
- `maxLoopLength`: Specifies the maximum loop length, if using a truncated loop finding algorithm. Integer.

You can also override each of these configuration manually with an environment variable.
Here are the default values for each of them:

| JSON Name            | Env Variable Name | Default Value  |
|----------------------|-------------------|----------------|
| compressionAlgorithm | HTF_COMPRESSION   | None           |
| encodingAlgorithm    | HTF_ENCODING      | None           |
| loopFindingAlgorithm | HTF_LOOP_FINDING  | BasicTruncated |
| zstdCompressionLevel | HTF_ZSTD_LVL      | 3              |
| maxLoopLength        | HTF_LOOP_LENGTH   | 100            |

## Contributing

Contribution to HTF are welcome. Just send us a pull request.

## License

OTF2-Profile is distributed under the terms of both the BSD 3-Clause license.

See [LICENSE](LICENSE) for details
