# Megatech Vulkan Dispatch

Designed as a successor to [VKFL](https://github.com/gn0mesort/vkfl), this project provides a simple way to configure
and generate dispatch tables for the [Vulkan](https://www.vulkan.org/) API. Like VKFL, the only dependencies for
building the library are a working Python environment, a similarly working <span class="nowrap">C++20</span>
environment (i.e., a toolchain and standard library), and a valid Vulkan XML specification file (e.g.,
[KhronosGroup/Vulkan-Docs/v1.3.289/xml/vk.xml](https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.3.289/xml/vk.xml)
). `libmegatech-vulkan-dispatch` has no dependency on the Vulkan loader (i.e., `libvulkan`).

## Dependencies

To build this library you need:

- A standard C++20 compiler
- Python 3.12 or newer (much older probably works)
- The Python dependencies described in [requirements.txt](requirements.txt)

To use the library at run-time you additionally need:

- A standard C++20 run-time library

For tests and benchmarks you additionally need:

- The standard Vulkan loader library (`libvulkan`)
- [Catch2](https://github.com/catchorg/Catch2)

## Preparing the Project

Before you can build the library, you need to prepare a working environment. The easiest way to do this is to create a
Python virtual environment and install the dependencies described by `requirements.txt`. If you are using a Bash
shell, the following commands will create a virtual environment, enter it, and install all of the necessary packages:

```sh
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
```

As long as everything runs successfully, you should have a working build environment.

## Building the Library

This project uses [Meson](https://mesonbuild.com/) to handle the generation of sources and the compilation of
`libmegatech-vulkan-dispatch`. Meson, along with [Ninja](https://ninja-build.org/), should have been installed when
you prepared your virtual environment.

to build the library, first ensure you are in a working build environment. Next, run:

```sh
meson setup build
```

At this point, the library is ready for configuration. Once you've configured the library as needed, run:

```sh
meson compile -C build
```

to compile the library.

## Installing the Library

Installing Meson projects is simple. Just run:

```sh
meson install -C build
```

This will install all of the required header files (including generated header files) under
`$PREFIX/include/megatech/vulkan` as well as the `libmegatech-vulkan-dispatch` library.

## Building HTML Documentation

[Doxygen](https://www.doxygen.nl/index.html) documentation can be built using the following command:

```sh
meson compile -C build documentation
```

## Tests and Benchmarks

You can enable tests by running:

```sh
meson configure build -Dtests=enabled
```

To compile and run tests, use:

```sh
meson test -C build
```

Tests additionally require the Vulkan loader as well as Catch2. Debian provides
packages for both. You can install them with:

```sh
apt install libvulkan-dev catch2
```

You can enable benchmarks by running:

```sh
meson configure build -Dbenchmarks=enabled
```

To compile and run benchmarks, use:

```sh
meson test -C build --benchmarks
```

Benchmarks use the same extra dependencies as the test programs.

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


