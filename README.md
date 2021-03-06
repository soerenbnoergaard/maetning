# maetning

Saturation plugin for Linux, MacOS, and Windows.

The plugin contains multiple saturation algorithms to choose from.

## Saturation algorithms

Below is a list and graph of each saturation algorithm included.

### Saturation 0

![Saturation 0](https://raw.githubusercontent.com/soerenbnoergaard/maetning/master/doc/sat0_io.png)

### Saturation 1

![Saturation 1](https://raw.githubusercontent.com/soerenbnoergaard/maetning/master/doc/sat1_io.png)

### Saturation 2

This is the DistTube algorithm from [reverse-camel](https://github.com/soerenbnoergaard/reverse-camel).

![Saturation 2](https://raw.githubusercontent.com/soerenbnoergaard/maetning/master/doc/sat2_io.png)

### Saturation 3

This is the DistMech algorithm from [reverse-camel](https://github.com/soerenbnoergaard/reverse-camel).

![Saturation 3](https://raw.githubusercontent.com/soerenbnoergaard/maetning/master/doc/sat3_io.png)

### Saturation 4

![Saturation 4](https://raw.githubusercontent.com/soerenbnoergaard/maetning/master/doc/sat4_io.png)

### Saturation 5

![Saturation 5](https://raw.githubusercontent.com/soerenbnoergaard/maetning/master/doc/sat5_io.png)

## Download

Releases are found in the [Github release page](https://github.com/soerenbnoergaard/maetning/releases).

## Building

On Linux for Linux:

    make

On Linux for Windows with `multiarch/crossbuild`:

    docker pull multiarch/crossbuild
    docker run -it --rm -v $(pwd):/workdir -e CROSS_TRIPLE=x86_64-w64-mingw32  multiarch/crossbuild make WIN32=true

On Linux for MacOS with `multiarch/crossbuild`:

    docker pull multiarch/crossbuild
    docker run -it --rm -v $(pwd):/workdir -e CROSS_TRIPLE=x86_64-apple-darwin  multiarch/crossbuild make MACOS=true CXX=c++ CXXFLAGS=-stdlib=libc++

On Linux for Windows with mingw-w64:

    sudo apt install mingw-w64
    make WIN32=true CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++

On MacOS for MacOS:

    brew install pkg-config
    make

