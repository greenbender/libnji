### libnji ###

libnji is a library that can be used to ease use of Java JNI in a native
application. It consists of two parts:

 1. njic - The code generation tool.
 2. libnji - The runtime library for NJI.


#### njic ####

njic takes JSON based NJI files that describe Java classes and outputs C source
code that provides a simple and conistsent interface to Java. The templates it
uses to do this can be found in `tools/njic/src/nji/templates`. If you want use
a custom template you can. Check the help for njic for details. Take care to
implement all the required parts of the template.

##### Requirements #####

The following must be installed:

 1. javap
 2. Python 2.7 (or reasonably new Python 2.x at least - Python 3 support pending)

##### Installation #####
    
    cd tools/njic
    sudo python setup.py install

This will install njic and its dependencies (jinja2) system wide. If you want
to use a virtualenv you can.

##### Usage #####

See the README.md in tools/njic for usage.


#### libnji ####

This is the runtime component. It takes care of getting a JavaVM, resolving
classes and methods, and handling exceptions. It also provides an abstraction
layer on top of JNI that handles the details on the Java environment.

You will need to statically link against this library to use the source files
created by ```njic```.

The best way to learn how to use this is through the examples.

#### Requirements ####

 1. The Java Developement Kit (I used the `openjdk-9-jdk-headless` package)

This is required for `jni.h` and to link the test program against `libjvm`
 
##### NOTE #####

In libnji the jenv.c file is responisible for abstacting away the details of
acquiring a ```JNIEnv*``` (Java Environment for each thread). The method of
acquiring the ```JNIEnv*``` can vary by platform, as such, you may (will) find
that modifying this code it necessary. The code provided here is targeting the
a standard Java runtime (such as would be found in linux).


#### The Example ####

The code under `test/` provides an example of NJI. It uses only classes from
the Java standard library and has been tested on Ubuntu 18.04. It simply
enumerates threads and prints some output about each thread and its ClassLoader
if is has one.

### Build System ###

Currently SCons is supported. I *may* looking at adding support for other build
tools out of the box at some point, however, if you don't want to use SCons the
library is pretty small and porting to another build tool is fairly straight
forward.
