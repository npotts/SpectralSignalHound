SpectralSignalHound external libs
=================================

The following are external libs used by this application.

easyloggingpp
=============

Library used for logging across the platform.  My thoughts are that I 
cannot write a logger that is better, and it will be highly buggy, so
just use something better. :-)

kompex
======

Library for writing SQLite databases.  Essentially a wrapper around the
standard SQLite database.  SQLite is included in the database.  I chose
this because it supports SQLITE_THREADSAFE whereas my normal go-to library
of https://github.com/SRombauts/SQLiteCpp is only SQLITE_THREADSAFE=2, 
multi-thread safe (only one process can access database at the same time);