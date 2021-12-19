# stencil3d

Various (more or less) optimized and parallel C implementations of the same stencil kernel.

## Compilation and execution
Source code can be compiled using GNU Make. Several targets already exist, but you are free to add new targets and build your own recipes. Here is a list of all currently available compilation commands (assuming you are in the project's top directory) :

```shell
make gcc_debug
make gcc_autovec
make clang_debug
make clang_autovec
```

They will produce an executable with the target name in the `bin` directory. To run the program, just type :

```shell
./bin/executable_name kernel_name
```

where `kernel_name` is the name of the kernel. The available kernels are : `base`, `inv_loop`, `inv_loop_onediv`. For more information about what each kernel does, please read the Doxygen documentation or the source code.

## Credits
All credits go to Robin Colombier and Gabriel Suau.

## License
This project is distributed under the [GNU-GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) license. A copy of the whole license is included in the repository.
