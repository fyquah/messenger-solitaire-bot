# robot_cpp
C++ Interface to java Robot AWT Class

## Usage

Edit your files in `test/main.cpp` - entry_point has similar signature to standard main functions - The runtime is wrapped
around by a actual JVM instance. This is becasue java AWT doesn't work (or at least takes a lot of effort) headless.

Compiling:

~~~bash
make
~~~

Running:

~~~bash
make run
~~~
