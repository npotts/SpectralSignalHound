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

Timed Runs
==========

The documentation for the signal hound calls out that providing the cal data at
the onset should help speed up initialization, which is hard to tell,  I ended up
manually running instances for ~5 times with the following results.

With Provided Cal data
----------------------

```
 time ./sh-spectrum -v --start 400e6 --stop 406e6 --csv withcal.csv --db caltests.db -n 5 --log sh-spectrum.log --caldata cal.dat
 ```

- 6.55s user 4.20s system 22% cpu 47.377 total
- 7.24s user 3.47s system 22% cpu 47.391 total
- 6.65s user 4.05s system 22% cpu 47.378 total
- 6.83s user 3.50s system 21% cpu 46.968 total
- 7.18s user 3.61s system 22% cpu 47.082 total

With out providing cal data
---------------------------

```
time ./sh-spectrum -v --start 400e6 --stop 406e6 --csv withoutcal.csv --db nocaltests.db -n 5 --log sh-spectrum.log
```
  
- 7.43s user 4.24s system 21% cpu 53.783 total
- 7.69s user 3.98s system 21% cpu 54.121 total
- 7.59s user 3.83s system 21% cpu 53.776 total
- 7.17s user 4.00s system 20% cpu 53.919 total
- 6.79s user 4.92s system 21% cpu 54.117 total

Results
-------

Eyeballing the numbers it appears that shave off 6 seconds from the process, which may be worth it.

TODO / Feature Creep:
=====================

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
- ~~Add functionality to dump the cal data to a file~~
- Read a .ini config file to configure sweeps
	- named sweeps with multiple settings in a config file