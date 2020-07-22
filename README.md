# ODDs

A C++ library for Ordered Decision Diagrams.

## Build instructions

[CMake](https://cmake.org/) build system is required to build the project.
After cloning the submodules, build the project with the following commands.

```
$ cd build/
$ cmake ..
$ make
```

You can also run tests with the following command.

```
$ make test
```

## Building documentation

The header files are provided with [Doxygen](http://www.doxygen.nl/) documentation.
To build it, install Doxygen and type

```
$ doxygen Doxyfile
```

The home page of documentation will be available at `doc/html/index.html`.

## Contributors

See the [Contributors.md](Contributors.md) file.
