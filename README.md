pointviewer
===========

A 4D ([x,y,z] + temporal) point cloud visualisation tool.

Implemented with [OpenGL](http://www.opengl.org) and [SDL](http://www.libsdl.org).

Features


Install
-------

This version tested on MacOS (it used to work on Linux just fine...)

Resolve dependencies - e.g. via brew:

```
brew install sdl
```

Then configure:

```
./configure --without-x
```

On Linux you probably don't want the --without-x

Then to build:

```
make 
```

Followed by the following (if you have permission to install):

```
make install
```

You can use the `--prefix=/usr/local/bin` or similar when running `./configure` to change the install location.


TODO:
----

Where to start?

* Linux and Windows confirmation.
* More/robust file reading.
* Better interaction/controls. 
* "Splatting".
* Massive point cloud data-sets (on the cloud?)
* Deltas - descriping derivatives of motion and local surfaces.  


Colophon
--------

In case ever to do this again, how to reset the automake toolchain... 

```
brew install aclocal automake and libtool
create configure.ac
create Makefile.am
add the m4 directory containing the macros for sdl and opengl
aclocal && automake --gnu --add-missing && autoconf
```
