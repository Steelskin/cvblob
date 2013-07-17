# cvBlob

This library allows you to find and track blobs on a picture, using OpenCV.

## Dependencies

In order to compile cvBlob you need the following:

* CMAKE http://www.cmake.org
* OpenCV http://opencvlibrary.sourceforge.net
* A recent C++ compiler, that supports some C++11 features

## Compiling and installing

(This section is a Work In Progress)

In Linux, if you have unpacked the source in $CVBLOB, enter in a console:

	cd $CVBLOB
	cmake .
	make
	sudo make install

If you are encountering issues, please read [this](http://www.cmake.org/HTML/RunningCMake.html).

For Windows, a Visual Studio 2012 solution is provided in the vc subdirectory.

### OpenCV path

You can tell CMake where OpenCV is installed, in case it can't find it, using the `OpenCV_DIR` variable:

	cmake . -DOpenCV_DIR=<path_to_OpenCV>

### Installation path

To change the destination path for the installation, set the `CMAKE_INSTALL_PREFIX` variable:

	cmake . -DCMAKE_INSTALL_PREFIX=<installation_path>

### Problems

Encountered an issue? [Tell us about it!](https://github.com/Steelskin/cvblob/issues)

## Information

The official home page of cvBlob is https://github.com/Steelskin/cvblob

## Author and contributors

Comments, suggestions and, why not, donations to:

* [Fabrice de Gans-Riberi](mailto:fabrice.degans@gmail.com), current maintainer
* [Cristóbal Carnero Liñán](mailto:grendel.ccl@gmail.com), first author
* [Stefan Gachter](http://gachter.name)
* Kouichi Nishino
