# SpectralSignalHound

Spectral Signal Hound is a RF Spectral Logger using a SA44B Signal Hound Software 
Defined Radio (SDR).  There are two iterations of the software. The first is found
in the  based off the Basic Linux API and only supports the intel architecture and 
is present in the linuxapi branch.  The second iteration is based on the headless
API and it supports the intel and arm architectures.  


# [linuxapi](README-1.x.md)

This version (1.x.y series) allows for fine grain control of the 
SignalHound via the 'linuxapi' release.  This, as far as I can tell,
will never be supported on ARM, and only supports intel archs.  The
particular API is found [here] (https://www.signalhound.com/support/downloads/sa44b-sa124b-downloads).
The image below shows the location on the download page.

![API Used](/README.d/1.x-api-used.png "Location of API used")

## What is Excels At


- Fine grain control of sweeps
- Slow Sweep times
- Huge sweep resolution
- Might be able to compile on windows.  Havent Tried

## What is Sucks At

- Saving data to a SQL database (due to enormous size)
- Running on ARM processors
- Running Fast (~couple seconds) sweeps.  Sweeping the whole band in slow mode takes 30 minutes per pass

# [headless](https://github.com/npotts/SpectralSignalHound/blob/master/README.md)


This is the the 2.x.y release series, and it allows for a more
"spectrum analyzer" type sweep methods in addition to controlling
more of the fundamental parameters for you.  This does allow for better
storing of the data in SQLite databases, different sweep patterns, as well
as the ability to run on ARM.

![API Used](/README.d/2.x-api-used.png "Location of API used")

## What is Excels At

- Shoving data and metadata into SQLite database
- Fast Sweep times
- Sweep Modes
  - Fast
  - Slow
  - 5MHz Fixed Bandwidth
  - Zero Span
  - Phase Noise
- Runs on ARM

## What is Sucks At

- High Resolution data
- Some features are not tweakable, and underlying library can overwrite configured settings

# License

This software is licensed under a BSD license.  You are not required to, but encouraged, 
to let the original author know if you are using this.  Please submit any bug fixes or 
improvements by forking, and pull requests.

# Building

This build process ONLY describes the 2.x series. In order to view the [old 1.x linuxapi you will need to browse here](README-1.x.md).

## Required Libraries

The following C++ libraries are required.  Some are included as git submodules.

- boost::program_options
- SignalHound API
- Kompex Libraries (git submodule)
- EasyLoggingpp (git submodule)


## Build Workflow


```shell
git clone --recursive git@github.com:npotts/SpectralSignalHound.git
#This uses a static built library from the Kompex SQLite Wrapper.  This builds the library.
cd SpectralSignalHound/ext/kompex
./configure && make
cd ../../
make
```

# sh-spectrum2 Usage


## Built in Help

sh-spectrum2 has a pretty functional built-in --help listing.

```shell
% sh-spectrum2 --help

sh-spectrum: A Signal Hound (SA44B) Spectrum Analyzer Logger

Usage: sh-spectrum <Arguments>
General Options:
  -h [ --help ]         Show this message
  -V [ --version ]      Print version information and quit
  --nostdout            Nothing will be printed to stdout. Return value will 
                        indicate fatal errors. If you want to see errors, you 
                        can still use --log below.
  -l [ --log ] arg      Write program log to file specified by arg.
  -v [ --verbose ]      Setting this will emit a gratuitous amount of babble.

Data Output:
  --db arg              Write data into a sqlite database specified by the arg.
  --csv arg             Produce a comma seperated file with data specified by 
                        arg.

RF Options:
  --preamp                  If flag is set, will attempt to activate the built 
                            in RF preamplifier.  Only available on a Signal 
                            Hound SA44B.
  --extref                  If flag is set, the Signal Hound will attempt to 
                            use a 10MHz external reference.  Input power to the
                            Signal Hound must be greater than 0dBm in order for
                            this to be used.
  --center arg (=403000000) Set the center frequency in Hz.  Acceptable values 
                            are similar to 4e3, 4000, 4000.0 which all resolve 
                            to 4kHz.
  --span arg (=6000000)     Set the span in Hz.  This value is ignored in 
                            --zspan and --phase-noise modes.
  --start arg (=-1)         Set the starting sweep frequency in Hz. If this and
                            --stop are both not the defaults, these values will
                            be used for the start and stop frequencies. This 
                            value is completely ignored in --zspan and 
                            --phase-noise modes, which rely on --center only.
  --stop arg (=-1)          Set the stop sweep frequency in Hz. If this and 
                            --start are both not the defaults, these values 
                            will be used for the start and stop frequencies. 
                            Simlar to --start, this value is completely ignored
                            in --zspan and --phase-noise modes and strictly 
                            relies on --center only.
  --reflevel arg (=-10)     Set the reference level in dBm.  Valid values are 
                            between +10.0 and -150.0
  --attenuation arg (=3)    Set the input attenuation.
                            0 = 0dB
                            1 = 5dB
                            2 = 10dB
                            3 = 15dB
  --rbw arg (=-1)           Attempt to set the resolution bandwidth (RBW).  The
                            actual RBW will be altered to reflect what it 
                            physically possible. Default of -1 will 
                            automatically select a RBW. Allowed values are -1, 
                            and values between 2 and 24 (inclusive).
                            2 = 5 MHz
                            3 = 250 kHz
                            4 = 100 kHz
                            5 = 50 kHz
                            6 = 25 kHz
                            7 = 12.5 kHz
                            8 = 6.4 kHz
                            9 = 3.2 kHz
                            10 = 1.6 kHz
                            11 = 800 Hz
                            12 = 400 Hz
                            13 = 200 Hz
                            14 = 100 Hz
                            15 = 50 Hz
                            16 = 25 Hz
                            17 = 12.5 Hz
                            18 = 6.4 Hz
                            19 = 3.2 Hz
                            20 = 1.6 Hz
                            21 = .8 Hz
                            22 = .4 Hz
                            23 = .2 Hz
                            24 = .1 Hz
  --vbw arg (=-1)           Attempt to set the Video bandwidth (RBW).  The VBW 
                            needs to be smaller than the RBW, and is not used 
                            at all in Zero Span Sweep Mode. Default of -1 will 
                            automatically select a VBW. The values of VBW are 
                            the same as RBW.
  --no-img-reject           If set, it will remove the image rejection that is 
                            normally applied.
  --sweep-speed arg (=0)    Suggested Sweep speed. Allowed values are -1 to 4. 
                            Use -1 sparingly, as documentation mentions it 
                            should rarely be set to -1.
                            -1: Ub3r fast (b0rk3d?)
                            0: (default) Fast
                            ...
                            4: Slow.
                            Values outside this range will fall back to the 
                            default.
  --vdmode arg (=4)         Set the mode for Video Detector Processing. 
                            Documentation is rather sparse, so the details here
                            are gathered from varible names.
                            1 = Process as power
                            2 = Process as Voltage
                            3 = Process as Log
                            4 (default) = Bypass Video Processing.
  --vdmma arg (=4)          Set the mode for Video Detector Min Max. 
                            Documentation is rather sparse, so the details here
                            are gathered from varible names.
                            1 = Min-Max
                            2 = Min Only
                            3 = Max Only
                            4 = Average Only.

Operational Modes:
  --delay arg (=0)      In order to limit on the rediculously large file sizes,
                        how long should this program pause between sweeps in 
                        milliseconds
  --sweeps arg (=-1)    How many sweeps should be done before exiting.  Default
                        of -1 means sweep forever (well... at least until 
                        Ctrl-C hit or power cycled)
  --slow                Runs a slow sweep
  --fast                Runs a fast sweep
  --rbw5MHz             Sweep with a fixed RBW of 5MHz
  --zspan               Run a Zero Span Sweep
  --phase-noise         Run phase-noise measurement
  --tracking-gen        Run a Tracking Generator sweep - Completely untested as
                        I do not have a tracking generator signal hound.

Zero Span Options:
  --zs-mode arg (=0)       Zero Span Demodulation Mode.  0=Amplitude, 
                           1=Frequency, 2=Phase, all others will revert to 
                           default.
  --zs-ifbw arg (=0)       Select the IF bandwidth. 1=240kHz, 2=120kHz, 
                           4=60kHz, 8=30kHz, 16=15kHz.  Other values will fall 
                           back to the default.
  --zs-sweep-time arg (=1) Set the sweep time (in seconds) when in zero span 
                           mode.

Phase Noise Options:
  --pn-decade-start arg (=1) Phase Noise Decade Start
  --pn-decade-stop arg (=5)  Phase Noise Decade Stop

```

## Slow and Fast Sweeps

Use a fast sweep from 400 to 406MHz enabling the preamp attemping to use a
10MHz external reference.  Data will be stored in the SQLite database 
"sweep.db".

```shell
sh-spectrum2 --fast --start 400e6 --stop 406e6 --preamp --extref --db sweep.db
```

Sweep from 400 to 406MHz slowly saving the data to sweep.csv.
```shell
sh-spectrum2 --start 400e6 --stop 406e6 --slow --csv sweep.csv

```

#[Plotting Tools](plotting/)

So now that you just collected some huge amount of data stored in a text file,
what can you do with it?  There are some simple plotting tools that use python
and the matplotlib library for making medicore plots like below from csv files:

![Plot of Metband connected to a signal generator](/README.d/metband.png "Plot of Metband connected to a signal generator")

# Further Reading

- Details on (C++) sh-spectrum: [src/Readme.md](src/)
- Plotting utilities: [plotting/Readme.md](plotting/)





