# AsyncData

This library contains classes and templates designed to support asynchronous
and lock-free communication and data exchange between different components in a
larger system.

## Usage

The `AsyncData::Dispatcher` class operates a worker thread which executes
*jobs*.  A *job* consists of a function to be called by the worker thread and a
mechanism through which another thread can poll or wait for the function call
to be completed and obtain the return value, if any.

The `AsyncData::MultiProducerMultiConsumerStack` template implements a stack of
arbitrary items, where it's safe to push onto and pull items off of the stack
from different threads without the use of locking mechanisms such as mutexes.

The `AsyncData::MultiProducerSingleConsumerQueue` template implements a
unidirectional queue of arbitrary items, where it's safe to push items onto the
tail of the queue from different threads and pop items off the head of the
queue from a single thread without the use of locking mechanisms such as
mutexes.

## Supported platforms / recommended toolchains

This is a portable C++11 library which depends only on the C++11 compiler and
standard library, so it should be supported on almost any platform.  The
following are recommended toolchains for popular platforms.

* Windows -- [Visual Studio](https://www.visualstudio.com/) (Microsoft Visual C++)
* Linux -- clang or gcc
* MacOS -- Xcode (clang)

## Building

This library is not intended to stand alone.  It is intended to be included in
a larger solution which uses [CMake](https://cmake.org/) to generate the build
system and build applications which will link with the library.

There are two distinct steps in the build process:

1. Generation of the build system, using CMake
2. Compiling, linking, etc., using CMake-compatible toolchain

### Prerequisites

* [CMake](https://cmake.org/) version 3.8 or newer
* C++11 toolchain compatible with CMake for your development platform (e.g.
  [Visual Studio](https://www.visualstudio.com/) on Windows)

### Build system generation

Generate the build system using [CMake](https://cmake.org/) from the solution
root.  For example:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A "x64" ..
```

### Compiling, linking, et cetera

Either use [CMake](https://cmake.org/) or your toolchain's IDE to build.
For [CMake](https://cmake.org/):

```bash
cd build
cmake --build . --config Release
```
