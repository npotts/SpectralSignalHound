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
