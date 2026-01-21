# Source code:

## main.c
Implementation of the main functionality of the application.

## geometric-brownian-motion.*
Implementation of the Geometric Motion algorithm as well as put and call option calculation.

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
[Signaloid-Demo-UxHwCompatibilityForNativeExecution](https://github.com/signaloid/Signaloid-Demo-UxHwCompatibilityForNativeExecution)
which is included as a submodule in `submodules/compat`.

## config.mk
Signaloid cores use this file to identify the source codes they will use when
building the C/C++ demo application.

# To Build Natively on Non-Signaloid Platforms

## On MacOS (with MacPorts)
```
gcc -O3 -I. -I/opt/local/include main.c utilities.c common.c uxhw.c geometric-brownian-motion.c -L/opt/local/lib -lgsl -lgslcblas
```

## On Linux
```
gcc -O3 -I. -I/opt/local/include main.c utilities.c common.c uxhw.c geometric-brownian-motion.c -L/opt/local/lib -lgsl -lgslcblas -lm
```
