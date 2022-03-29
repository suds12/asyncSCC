# asyncSCC
## Getting Started
### Requirements
- C++17 - GCC versions 8, 9 and 10 are tested. Your mileage may vary with other compilers.
- Cereal - C++ serialization library
- MPI
- Optionally, Boost 1.77 to enable Boost.JSON support.
### Build and run
```
mkdir build
cd build
cmake ..
make
```
This will build all dependencies in /build/_deps and write the executable 'main' to build/src
