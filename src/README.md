SpectralSignalHound Sources
===========================

This folder contains all the C++ code and headers needed to compile the project.

SignalHound.cpp/.h
------------------

Contains the class SignalHound which wraps around the SignalHound 
API to provide an easy way to perform sweeps.  Makes use of the 
[easyloggingpp](https://github.com/easylogging/easyloggingpp) library from github 
as a git submodule.

SignalHoundCLI.cpp/.h
---------------------

Command line parsing and orchastrating module.  main() enters here ASAP.  Uses [boost::program_options] (http://www.boost.org/doc/libs/1_57_0/doc/html/program_options.html) to parse the command line args.



SHBackend*.cpp/.h
-----------------

Backend storage for result data. Currently only CSV and a rough SQLite database are implemented. The SQLite database is rough due to the huge number of data that can be returned based on sweep parameters so returned
data is joined into a huge CSV string and shoved into a single table cell.  The SQLite backend make use of
the [kompex-sqlite-wrapper](https://github.com/Aethelflaed/kompex-sqlite-wrapper/) library.

SHLAPI.h
--------

Header for the signalhound.  Taken directly from [here] (https://www.signalhound.com/sigdownloads/SA44B/linux_shapi_x86_64.zip) from [this page](https://www.signalhound.com/support/downloads/sa44b-sa124b-downloads).


TODO:
-----

- ~~Logging~~
	- ~~Turn on/off debugging logs on terminal~~
	- ~~Make sure file logging works~~
	- ~~make --quiet and --verbose flags work as expected on STDOUT~~
	- ~~Set the defaults on loggers when getting a new logger.  Changing date formats is quite annoying~~
	- ~~make debug log message a little more condensed.~~
- ~~Condense SHBackend so that it contains the bare minimum.~~
- ~~CSV output~~
	- ~~whole thing~~
- ~~SQLite output~~
	- ~~verify working~~
- Add functionality to dump the cal data to a file
- Plotting
	- How to display data eloquently and usefully.