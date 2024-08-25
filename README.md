# Megatech-Vulkan Dispatch

About dispatch, the [Vulkan](https://registry.khronos.org/vulkan/) specification says the following:

> Vulkan commands are not necessarily exposed by static linking on a platform.
> [...]
> Function pointers for all Vulkan commands **can** be obtained by calling [`vkGetInstanceProcAddr`.]
> `vkGetInstanceProcAddr` itself is obtained in a platform- and loader- specific manner. Typically, the loader
> library will export this command as a function symbol, so applications **can** link against the loader library, or
> load it dynamically and look up the symbol using platform-specific APIs.
> [[1]](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#initialization-functionpointers)

It goes on to say:

> In order to support systems with multiple Vulkan implementations, the function pointers returned by
> `vkGetInstanceProcAddr` **may** point to dispatch code that calls a different real implementation for different
> `VkDevice` objects or their child objects. The overhead of the internal dispatch for `VkDevice` objects can be
> avoided by obtaining device-specific function pointers for any commands that use a device or device-child object as
> their dispatchable object. Such function pointers can be obtained by calling [`vkGetDeviceProcAddr`.]
> [[1]](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#initialization-functionpointers)

To summarize, the Vulkan API does not guarantee applications convenient access to its commands. Although Vulkan
commands can be exposed in a convenient platform-specific way (e.g., by the Vulkan loader library), this isn't a hard
requirement for Vulkan support. Additionally, the Vulkan API does not guarantee direct dispatch of device-specific
commands unless they are explicitly resolved by the application. Device-specific commands that are not explicitly
resolved can create unnecessary overhead as the Vulkan API performs its own dispatch operations. This means that,
ideally, Vulkan applications provide their own dispatch solution.

The purpose of this library is to provide a dispatch solution for Vulkan applications.

## Dependencies

The Megatech-Vulkan Dispatch library has minimal dependencies. To build the library you will need the following:

- A standard C++20 compiler (e.g., a recent version of GCC or Clang) and run-time environment.
- A working Python 3 environment with `pip`.
- The [Megatech-Vulkan Dispatch library tools](https://github.com/gn0mesort/megatech-vulkan-dispatch-tools) Python
  package
- A working implementation of the [Meson](https://mesonbuild.com/) build system and a compatible build backend (e.g.,
  [Ninja](https://ninja-build.org/))

At run-time the library only requires a standard C++20 run-time environment.

To build the library's tests and benchmarks, this repository also requires:

- The standard Vulkan loader library (`libvulkan`)
- [Catch2](https://github.com/catchorg/Catch2)

To build HTML documentation, you will need the following:

- [Doxygen](https://doxygen.nl/)

## Building

### Preparing the Build Environment

Before building the library, it's important to ensure that the various development dependencies are properly installed.
The exact details of how to install acceptable C++20 and Python 3 environments on your platform are beyond the scope
of this document. You should consult the relevant documentation for your system. Once you have installed these
environments, you can prepare the build environment by installing the Python dependencies in
[`requirements.txt`](requirements.txt). You can install these dependencies system-wide, but I recommend installing
them in a [virtual environment](https://docs.python.org/3/library/venv.html). To prepare an acceptable Python
virtual environment you can run the following:

```sh
# Create a virtual Python environment in the ".venv" directory
python3 -m venv .venv
```

Once you have a virtual environment you will need to activate it. The exact method for activating the environment will
depend on your system. For a [Bash](https://www.gnu.org/software/bash/) shell, you can activate a virtual environment
at `.venv` by running:

```sh
source .venv/bin/activate
```

At this point, you can install Megatech-Vulkan Dispatch's Python dependencies. To do so, execute the following
command:

```sh
python3 -m pip install -r requirements.txt
```

After this, your build environment will be able to compile the library. If you would like to run tests or benchmarks,
you will need to install the additional C++ dependencies. The exact procedure for installing these libraries is
system-specific. As an example, on a [Debian](https://www.debian.org/) Bookworm machine you can run the following
command:

```sh
apt install catch2 libvulkan-dev
```

Other software for managing Python virtual environments, Python packages, and system packages will probably work too.
However, you will need to find documentation on them separately.

### Configuring the Build System

After preparing the build environment but before compiling the library, you might want to configure the build system.
To do that run:

```sh
# Create a directory called "build" and configure it using default options.
meson setup build
```

You can then display the various build system options by running:

```sh
meson configure build
```

This will display information for built-in options provided by Meson as well as library specific options.

To actually reconfigure the build system you will need to pass `meson` a list of new configuration options.
For example, to enable tests and benchmarks you would run:

```sh
meson configure build -Dtests=enabled -Dbenchmarks=enabled
```

If you are planning to install the library on your system with the default configuration, you should run:

```sh
meson configure build -Dbuildtype=release
```

For a testing, you should instead use:

```sh
meson configure build -Dbuildtype=debug -Dtests=enabled -Dbenchmarks=enabled -Dextra_defines=enabled
```

For benchmarking, you may want to change `-Dbuildtype=debug` to `-Dbuildtype=debugoptimized`.

### Compiling

When everything is configured, you can compile the library by executing:

```sh
# Compile the targets described in the "build" directory.
meson compile -C build
```

If you enabled them earlier, you can compile and run the tests by running:

```sh
# Compile the test targets described in the "build" directory and then run them.
meson test -C build

# Compile the benchmark targets described in the "build" directory and then run them.
meson test -C build --benchmark
```

## Installing

After building the library, you can install it by using:

```sh
# Install all the files described by the configuration located in "build".
# You can add --dry-run to determine what would be installed without actually performing the installation.
meson install -C build
```

This will install the library (`libmegatech-vulkan-dispatch`), all of the necessary development headers, and a
[`pkg-config`](https://www.freedesktop.org/wiki/Software/pkg-config/) file generated by Meson.

## HTML Documentation

To build HTML documentation, first ensure you have a working build environment. Next, ensure you have Doxygen
installed. Finally, you can build the documentation by running:


```sh
# Build the target "documentation" in the current Meson project as described in the "build" directory.
meson compile -C build ./documentation
```

The resulting documentation can be found at `build/documentation/html`.

## Library Usage

The Megatech-Vulkan Dispatch library exposes its functionality through dispatch table objects. Each level of
functionality (i.e., global-, instance-, and device-level Vulkan commands) has its own `table` type in the
corresponding namespace. These types are similar but not related by inheritance. That is to say, a
`megatech::vulkan::dispatch::device::table` is not a `megatech::vulkan::dispatch::instance::table`, and a
`megatech::vulkan::dispatch::instance::table` is not a `megatech::vulkan::dispatch::global::table`. Nonetheless,
these types expose roughly the same interfaces.

During construction, each dispatch table type will preload the appropriate Vulkan command function pointers.
Regardless of the level of functionality, all of the Vulkan commands that were enabled at compile-time will be loaded.
When a function is not enabled, the corresponding table entry will be set to `nullptr`. When you retrieve function
pointers from a dispatch table type, it is your responsibility to validate that they were known to the library and
enabled.

To enable you to retrieve Vulkan command function pointers, the library provides two slightly different APIs. First,
dispatch tables may be directly indexed using the values of the corresponding `command` enumeration. For example,
you may directly index a `megatech::vulkan::dispatch::global::table` using a
`megatech::vulkan::dispatch::global::command` value. In this case, the table will return a `const void*` to a table
entry of the type
[`PFN_vkVoidFunction`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#PFN_vkVoidFunction).
Second, dispatch tables may be accessed by mapping hash values to table entries. This API works similarly, but the
table will accept invalid hash values.

For projects which control how the Megatech-Vulkan Dispatch library is compiled, the direct indexing API is simpler,
faster, and requires fewer checks. For projects that cannot control how the Megatech-Vulkan Dispatch library is
compiled (e.g., because it is provided by someone else) the hash API should always be preferred. In either case,
you should prefer resolving Vulkan functions at compile-time (e.g., by indexing or via a `consteval` hash function)
whenever possible.

For more information, see the HTML documentation.

## Licensing

Copyright (C) 2024 Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see
<[https://www.gnu.org/licenses/](https://www.gnu.org/licenses/)>.


