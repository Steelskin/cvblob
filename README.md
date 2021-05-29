# cvBlob

This library allows you to find and track blobs on a picture, using OpenCV.

## Dependencies

In order to compile cvBlob you need the following:

* CMAKE https://cmake.org
* OpenCV  https://opencv.org
* A recent C++ compiler, that supports some C++11 features

## Compiling and installing

### Linux

In Linux, Meson or CMake can be used.

#### Meson

```shell
cd $CVBLOB
meson builddir
cd builddir
ninja
(sudo) ninja install
```

#### CMake

```shell
cd $CVBLOB
mkdir build
cd build
cmake ..
ninja
(sudo) ninja install
```

### Windows

A Visual Studio 2012 solution is provided in the vc subdirectory. CMake can also be used.

### Installation path

#### Meson

When configuring with meson, simply provide a `--prefix=/path` like
```shell
...
cd builddir
meson --prefix=~/.local ..
...
```

#### CMake

To change the destination path for the installation, set the `CMAKE_INSTALL_PREFIX` variable:
```shell
cmake . -DCMAKE_INSTALL_PREFIX=<installation_path>
```

### Problems

Encountered an issue? [Tell us about it!](https://github.com/Steelskin/cvblob/issues)

## Information

The official home page of cvBlob is https://github.com/Steelskin/cvblob

## Author and contributors

Comments, suggestions and, why not, donations to:

* [Fabrice de Gans](mailto:fabrice.degans@gmail.com), current maintainer
* [Cristóbal Carnero Liñán](mailto:grendel.ccl@gmail.com), first author
* [Stefan Gachter](http://gachter.name)
* Kouichi Nishino
* [Giandomenico De Sanctis](http://gidesa.altervista.org/)
* [Michael de Gans](https://www.mdegans.dev)
