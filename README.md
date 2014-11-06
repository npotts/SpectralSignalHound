SpectralSignalHound
===================

A RF Spectral Logger using a Signal Hound SA44B SDR based off the Basic Linux API. EG, the files
downloaded from [https://www.signalhound.com/support/downloads/sa44b-sa124b-downloads](https://www.signalhound.com/support/downloads/sa44b-sa124b-downloads):

![API Used](/README.d/api-used.png "Location of API used")

License
=======

This software is licensed under a BSD license.  You are not required to, but encouraged, 
to let the original author know if you are using this.  Please submit any bug fixes or 
improvements.

Building
========

This application requires a couple external (git submodules) and some mostly modern version of boost::program_options. My intent is to eventually setup a Travis-CI instance.

Get the sources
---------------

```
git clone --recursive git@github.com:npotts/SpectralSignalHound.git
```

Build the Kompex Libraries
--------------------------

This uses a static built library from the Kompex SQLite Wrapper.  This builds the library.

```
cd SpectralSignalHound/ext/kompex
./configure && make
cd ../../
```

Build sh-spectrum
-----------------

```
cd SpectralSignalHound
make
```

TODO
====
- See [src/Readme.md](src/)





