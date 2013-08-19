pointviewer
===========

A 4D ([x,y,z] + temporal) point cloud visualisation tool.


MacOS
-----

./configure --without-x

Bootstrap
---------

In case I have to do this again, this is how I reset the toolchain... 

brew install aclocal automake and libtool

create configure.ac

create Makefile.am

add the m4 directory containing the macros for sdl and opengl

aclocal && automake --gnu --add-missing && autoconf
