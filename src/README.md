# Source code:

## main.c
Implementation of the main functionality of the application.

## kernel.c/h
Implementation of the compute kernel: drives the geometric Brownian motion path
calculation and the derived put/call option, Value-at-Risk, and simulated-returns
outputs, dispatching to either the UxHw or the Monte Carlo implementation below.

## geometric-brownian-motion-uxhw.c/h
Implementation of the geometric Brownian motion algorithm and option calculations
using the Signaloid UxHw API (or, for native builds, the UxHw compatibility shim).

## geometric-brownian-motion-monte-carlo.c/h
Monte Carlo implementation of the same geometric Brownian motion algorithm and
option calculations, used for native benchmarking runs (`-M`).

## utilities.c/h
These contain utility methods for parsing, setting, and reporting
the usage of demo-specific command-line arguments of C/C++ demo applications.
These methods call similar methods from `common.c` for handling
command-line arguments common to all of our C/C++ demo applications.

## common.c/h
These contain utility methods for parsing, setting, and reporting
the usage of command-line arguments common to all of our C/C++ demo applications,
as well as other methods that we commonly use across our
C/C++ demo applications, e.g., standard methods for I/O handling. These
source files are symlinks to the original files contained in the repository
[Signaloid-Demo-CommonUtilityRoutines](https://github.com/signaloid/Signaloid-Demo-CommonUtilityRoutines)
which is included as a submodule in `submodules/common`.

## uxhw.c/h
These contain methods that implement the probabilistic versions of the methods
in the UxHw API (e.g., `UxHwDoubleGaussDist`) and uses the GNU Scientific Library (GSL)
random number generators to achieve that. This allows building our C/C++ demo applications
natively (i.e., on conventional architectures) and running native Monte Carlo evaluations
of our C/C++ demo applications without modifying the source code.
These source files are symlinks to the original files and are contained in the repository
[UxHwCompatibility](https://github.com/signaloid/UxHwCompatibility)
which is included as a submodule in `submodules/compat`.

## config.mk
Signaloid cores use this file to identify the source codes they will use when
building the C/C++ demo application.

# To Build Natively on Non-Signaloid Platforms

From the repository root, build with:
```
make local-build
```

This produces the native Monte Carlo executable `demo-native-mc` at the repository
root. See the `Prerequisites` section of the top-level `README.md` for how to install
the build dependencies on macOS and Linux.
