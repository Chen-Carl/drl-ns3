# Distributed Rate Limiting Simulation

## 1 System Prerequisites

### 1.1 NS-3 Simulator

The simulator uses `PkgConfig` to load NS-3 libraries.

### 1.2 spdlog

CMake will automatically download the [spdlog repository](https://github.com/gabime/spdlog.git) to `build/_deps`.

### 1.3 Build

First, modify lines 43 and 45 to specify the ns-3 include path and pkgconfig path.

Second, export `CXX_COMPILER_PATH` to specify the C++ compiler that supports C++ 20.

``` bash
mkdir build && cd build
cmake ..
make
```

`bin` is the execution output folder. Run
``` bash
../bin/main
```
to start the network simulation.