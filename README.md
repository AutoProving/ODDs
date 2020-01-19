# ODDs

A C++ library for Ordered Decision Diagrams.

## Build instructions

External dependencies of the library are exported as git submodules in `ext/` subdirectory. Currently there are two dependencies:

* [RapidJSON](http://rapidjson.org/) is used for JSON serialization/deserialization of ODDs.
* [Google Test](https://github.com/google/googletest/) framework is used for unit tests.

To clone the dependencies to supposed locations, just type the following command after cloning this repository.

```
$ git submodule update --init
```

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
