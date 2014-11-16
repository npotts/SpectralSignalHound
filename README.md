# SpectralSignalHound

Spectral Signal Hound is a RF Spectral Logger using a SA44B Signal Hound Software 
Defined Radio (SDR).  This is the first iteration that used the Basic Linux API 
and only supports the intel architecture.  For the 2.x series, click [here](../linuxapi/).
It is represented in the linuxapi branch. This version (1.x.y series) allows
for fine grain control of the SignalHound. The particular API used can be found
[here](https://www.signalhound.com/support/downloads/sa44b-sa124b-downloads).
The image below shows the location on the download page.

![API Used](/README.d/api-used.png "Location of API used")

## What is Excels At

- Fine grain control of sweeps
- Slow Sweep times
- Huge sweep resolution

##What is Sucks At


- Saving data to a SQL database (due to enormous size)
- Running on ARM processors
- Running Fast (~couple seconds) sweeps.  Sweeping the whole band in slow mode takes 30 minutes per pass

# License


This software is licensed under a BSD license.  You are not required to, but encouraged, 
to let the original author know if you are using this.  Please submit any bug fixes or 
improvements by forking, and pull requests.

# Building

```
git clone --recursive git@github.com:npotts/SpectralSignalHound.git
cd SpectralSignalHound/ext/kompex
./configure && make
cd ../../
make
```

# sh-spectrum Usage


sh-spectrum has a pretty functional built-in --help listing. As of 2014-11-10:

```
% sh-spectrum --help

sh-spectrum-logger: A Signal Hound (SA44B) Spectrum Analyzer Logger

Usage: sh-spectrum-logger <Arguments>
General Options:
  -h [ --help ]           Show this message
  -V [ --version ]        Print version information and quit
  --nostdout              Nothing will be printed to stdout. Return value will 
                          indicate fatal errors. If you want to see errors, you
                          can still use --log below.
  -l [ --log ] arg        Write program log to file specified by arg. Defaults 
                          to stdout/stderr.
  -v [ --verbose ]        Setting this will cause a gratuitous amount of babble
                          to be displayed.  This overrides --quiet.
  --caldata arg           Use this file as the calibration data for the signal 
                          hound.  This should radically spead up 
                          initialization.  Use 'sh-extract-cal-data' to extract
                          this calibration data and reference it here.
  --attenuation arg (=10) Set the internal input attenuation.  Must be one of 
                          the following values: 0.0, 5.0, 10.0 (default), or 
                          15.0.  Any other value will revert to the default.
  --low-mixer             If flag is set, this will change the front end down 
                          converter to work with frequencies below 150MHz. If 
                          your frequency range will traverse above 150MHz, do 
                          not set this flag.
  --sensitivity arg (=0)  Set the sensitivity of the Signal Hound.  0 (default)
                          is lowest sensitivity, 2 is the highest.
  --decimate arg (=1)     Sample Rate is set to 486.111Ksps/arg.  Must be 
                          between [1, 16].  Resolution bandwidth is calculated 
                          from this and fft (below).
  --alt-iflo              If flag is set, this forces selection of the 2.9MHz 
                          Intermediate Frequency (IF) Local Oscillator (LO).  
                          The default is 10.7MHz and has higher selectivity but
                          lower sensitivity.  The 2.9MHz IF LO which features 
                          higher sensitivity yet lower selectivity.
  --alt-clock             If flag is set, this forces selection of the 22.5MHz 
                          ADC clock.  The default uses a 23-1/3 MHz clock, but 
                          changing this is helpful if the signal you are 
                          interested in is a multiple of a 23-1/3MHz.
  --device arg (=0)       Select which Signal Hound Device to use.  Up to 8 can
                          be connected to the same computer.  This seems to be 
                          disabled in the linux API
  --preset                If flag is set, the Signal Hound will be preset 
                          immediately after initialzing and prior to sampling. 
                          This does set the Signal Hound to a known state, but 
                          it also adds another 2.5 seconds to start up time.
  --extref                If flag is set, the Signal Hound will attempt to use 
                          a 10MHz external reference.  Input power to the 
                          Signal Hound must be greater than 0dBm in order for 
                          this to be used.
  --trigger arg (=0)      Change the trigger mode.  0 (default) triggers 
                          immediately. 1  will only trigger on an external 
                          logic high. 2 will cause the trigger signal to pulse 
                          high when data collection begins.
  --preamp                If flag is set, will attempt to activate the built in
                          RF preamplifier.  Only available on a Signal Hound 
                          SA44B.

RF Options:
  --start arg (=1000000)     Lower bound frequency to use for the spectral 
                             sweep.
  --stop arg (=10000000)     Upper bound frequency to use for the spectral 
                             sweep.  Due to rounding, you may get measured 
                             values past this value.
  --image-rejection arg (=0) Configure Image Rejection.  Default of 0 masks 
                             both high and low side injection.  Value of 1 only
                             apply high side injection.  Value of 2 only 
                             applies low side injection.
  --fft arg (=-1)            Size of the FFT. Default value of -1 will 
                             autoselect the prefered FFT window. This and the 
                             decimation setting are used to calculate the RBW. 
                             In --slow mode, may be 16-65536 in powers of 2 
                             while the default resolves to 1024.  In --fast 
                             mode, may be 1, 16-256 in powers of 2 while the 
                             default resolves to 16.
  --average arg (=16)        Only used in --slow sweep.  Arg is the number of 
                             FFTs that get averaged together to produce the 
                             output. The value of (average*fft) must be an 
                             integer multiple of 512.

Operational Modes:
  --extract-caldata arg     Gather the calibration data from the Signal Hound 
                            and save it to the file pointed to by <arg>
  --info [=arg(=2)]         Calculated parameters and dumps a list of what 
                            would be done.  This is helpful if you want to see 
                            the Resolution Bandwidth (RBW) or other RF 
                            parameters.  Due to limitations in the SignalHound 
                            API, some parameters will not be correct until the 
                            unit is initialized
  --fast [=arg(=1)]         Run a fast sleep. Fast sweep captures a single 
                            sweep of data. The start_freq, and stop_freq are 
                            rounded to the nearest 200KHz. If fft=1, only the 
                            raw power is sampled, and samples are spaced 200KHz
                            apart. If fft > 1, samples are spaced 200KHz.  RBW 
                            is set solely on FFT size as the decimation is 
                            equal to 1 (fixed internally)
  --slow [=arg(=0)]         Run a slow sweep. Slow sweep is which is more 
                            thorough and not bandwidth limited.  Data points 
                            will be spaced 486.111KHz/(fft*decimation).  Each 
                            measurement cycle will take: (40 + 
                            (fft*average*decimation)/486)*(stop_freq - 
                            start_freq)/201000 milliseconds, rounded up. 
                            Furthermore, fft*average must be a integer multiple
                            of 512.
  --delay arg (=0)          In order to limit on the rediculously large file 
                            sizes, how long should this program pause between 
                            sweeps in milliseconds
  -n [ --sweeps ] arg (=-1) How many sweeps should be done before exiting.  
                            Default of -1 means sweep forever (well... at least
                            until Ctrl-C hit or power cycled)

Data Output:
  --db arg              Write data into a sqlite database specified by the arg.
  --csv arg             Produce a comma seperated file with data specified by 
                        arg.

```

# Further Reading


- Details on (C++) sh-spectrum: [src/Readme.md](src/)
- Plotting utilities: [plotting/Readme.md](plotting/)
