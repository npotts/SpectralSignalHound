SpectralSignalHound Sources
===========================

This folder contains all the C++ code and headers needed
to compile the project.

SignalHound.cpp/.h
------------------

Contains the class SignalHound which wraps around the SignalHound 
API to provide an easy way to perform sweeps.

args.cpp/.h
-----------

boost-based command line option parsing. Verbose and yet so consise compared to getopts()

SHLAPI.h
--------

Header for the signalhound.  Taken directly from [here] (https://www.signalhound.com/sigdownloads/SA44B/linux_shapi_x86_64.zip) from [this page](https://www.signalhound.com/support/downloads/sa44b-sa124b-downloads).


TODO:
-----

- Logging
	- Turn on/off debugging logs on terminal
	- Make sure file logging works
	- make --quiet and --verbose flags work as expected on STDOUT
	- set the defaults on loggers when getting a new logger.  Changing date formats is quite annoying
	- make debug log message a little more condensed.
- Condense SHBackend so that it contains the bare minimum.
- CSV output
	- whole thing
- SQLite output
	- verify working
- Plotting
	- How to display data eloquently and usefully.