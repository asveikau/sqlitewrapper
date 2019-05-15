# libsqlitewrapper

This is a small library to give sqlite3's C API a slightly friendlier interface,
with error handling and logging consistent with [some of my other projects][1].

## Building

Building happens via [the makefiles submodule][2].

    $ git submodule update --init
    $ make                             # or "gmake" on some platforms, like BSD

On Unix, the project builds with g++ 8.0 or higher (7 and earlier won't work!)
or clang++.

On Windows, GNU make, nasm and msysgit should be on PATH, and the project is
typically tested with VS2015 with Windows SDK 10586.

By default, this will typically try to use the sqlite3 library already
installed.  However, if the `SQLITE_STATIC` makefile variable is defined
[which it will be for Windows builds], then sqlite3 is built from the
`third_party` directory and linked statically.

[1]: https://github.com/asveikau/common
[2]: https://github.com/asveikau/makefiles

