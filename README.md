pointviewer
===========

A 4D ([x,y,z] + time) point cloud visualisation tool.

Features:

* Loads ascii XYZ (with or without colour).
* Loads PointViewModel PVM files built with PVMC tool, in turn can read:
	- Floating point TIFF with colour imagery
	- C3D format
* Display either *spatially* or *temporally* (e.g all points - or play back)
* Show axis.
* Center on axis.
* Normalise points.
* Make points larger/smaller.

Implemented with [OpenGL](http://www.opengl.org) and [SDL](http://www.libsdl.org).


Install
-------

This version tested on MacOS and Linux. It needs automake version >=1.13.

Resolve SDL dependencies - e.g. via brew:

```
brew install sdl
```

Or, with some Linux package manager - e.g. debian/ubuntu apt-get:

```
apt-get install libsdl1.2-dev
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

The resulting `pv` executable can be run.

Or (if you have permission to install):

```
make install
```

You can use the `--prefix=/usr/local/bin` or similar when running `./configure` to change the install location.


TODO:
----

Where to start?

* Windows confirmation.
* Prebuilt dists?
* More/robust file reading - resurrect the C3D input!
* The Point Cloud Library came out after I wrote this - use some of it's features now...
* Better interaction/controls - esp. playback. 
* "Splatting" and other visualisation.
* Massive point cloud data-sets (on the cloud?)
* Deltas - descriping derivatives of motion and local surfaces.  


Colophon
--------

This was a handy wee tool developed during my PhD - [Qualifying 4D Deforming Surfaces by Registered Differential Features](https://www.era.lib.ed.ac.uk/handle/1842/2830). Thanks must go to Craig for the original pointviewer concept.

