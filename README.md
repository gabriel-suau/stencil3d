# stencil3d

Various (more or less) optimized and parallel C implementations of the same stencil kernel.

## Compilation and execution
Source code can be compiled using GNU Make. A Makefile with several targets already exists, and you are free to add new targets and build your own recipes. Here is a list of all currently available compilation commands (assuming you are in the project's top directory) :

```shell
make all
make gcc_debug
make gcc_autovec
make clang_debug
make clang_autovec
```

Each target correspond to a specific compiler with specific options. Please make sure that these compilers are installed on your system and that all the options are supported by your compiler version. The `make all` command will build all the targets of the Makefile. At the end of the compilation process, the `bin` directory contains the output binaries, the `asm` directory contains the generated assembly codes, and the `log` directory contains log files (with a `.comp` suffix) containing all compilation logs. To clear the `bin`, `asm` and `log` directories, use the `make clean` command. To run one of the executable, just type :

```shell
./bin/executable_name kernel_name
```

where `kernel_name` is the name of the kernel. The available kernels are : `base`, `inv_loop`, `inv_loop_onediv`, `inv_loop_onediv_tiled` and `inv_loop_onediv_omp`. For more information about what each kernel does, please read the Doxygen documentation or the source code.

## Automatic performance tests

To run the performance tests for all kernels of a given executable, use the `launch_tests.sh` script located at the project's root directory :
```shell
./launch_tests.sh /path/to/executable
```

This script uses the `perf` command-line tool to collect performance data. For more information on how to install and use the tool, please see the [`perf` wiki](https://perf.wiki.kernel.org/index.php/Main_Page). You may have to change the `perf` command-line options inside the bash script in order to gather the relevant data for your needs. The script will produce a `target.run` (where target corresponds to the make target) under the `log` directory containing the run logs.

## Credits
All credits go to Robin Colombier and Gabriel Suau.

## License
This project is distributed under the [GNU-GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) license. A copy of the whole license is included in the repository.
