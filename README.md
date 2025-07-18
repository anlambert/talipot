# Talipot [![license](https://img.shields.io/github/license/anlambert/talipot.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/ubuntu-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/ubuntu-build.yml)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/archlinux-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/archlinux-build.yml)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/appimage-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/appimage-build.yml)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/macos-macports-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/macos-macports-build.yml)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/macos-homebrew-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/macos-homebrew-build.yml)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/windows-mingw64-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/windows-mingw64-build.yml)
[![Build Status](https://github.com/anlambert/talipot/actions/workflows/windows-msvc-build.yml/badge.svg?branch=master)](https://github.com/anlambert/talipot/actions/workflows/windows-msvc-build.yml)

[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/talipot?pypiBaseUrl=https%3A%2F%2Ftest.pypi.org)](https://test.pypi.org/project/talipot)

Talipot is an open source, cross-platform, data visualization framework mainly
dedicated to the analysis, the drawing and the visualization of very large graphs
(up to the million of nodes and edges).

It is a fork of [Tulip](https://github.com/Tulip-dev/tulip) created by
[David Auber](https://www.labri.fr/perso/auber/david_auber_home_page/),
from [LaBRI](https://www.labri.fr) (Laboratoire Bordelais de Recherche en Informatique)
and [University of Bordeaux](https://www.u-bordeaux.fr).

![alt text](screenshots/talipot_screenshot_01.png "Talipot software GUI")

## Features

The Talipot framework offers numerous features, notably:

  * An efficient graph data model in terms of memory usage for storing large networks
  and the attributes of their elements (called properties in the Talipot semantics).
  It is also one of the few that offer the possibility to efficiently define and
  navigate graph hierarchies or cluster trees (nested subgraphs).

  * Several graph file formats for serializing such a model to disk, notably the
  [TLP format](https://tulip.labri.fr/site/?q=tlp-file-format) based on a
  Lisp syntax for easy parsing but also the TLP binary format for faster graph
  saving and loading.

  * A large variety of graphs algorithms: clustering, metric, layout ... As Talipot
  is dedicated to graph visualization, it is provided with numerous state of the art
  graph layout algorithms but also a bridge to the
  [Open Graph Drawing Framework](https://ogdf.uos.de).

  * A hardware accelerated graph rendering engine written in [OpenGL](https://www.opengl.org),
  highly customizable in terms of visual encoding for graph nodes and edges, in order
  to efficiently generate aesthetic and interactive visualizations.

  * Multiple visualization components (called views in the Talipot semantics) for
  analyzing graph data using other representations than the classical node link
  diagram one: matrix, histograms, scatter plots, parallel coordinates, ...

  * [Python](https://www.python.org) bindings for the main Talipot C++ API, giving to
  Talipot scripting facilities for manipulating graphs loaded from its main graphical
  user interface. The bindings can also be obtained from the Python Packaging Index.

  * A plugin based architecture for easily extend the capability of the framework with
  new graph import mechanisms, graph algorithms, visualization components, ...
  Talipot plugins can be written in C++ or Python.

  * A graphical user interface, based on the [Qt](https://www.qt.io) framework, enabling
  to easily interact and manipulate the different components of the framework.


## Installing Talipot

Talipot is a cross-platform framework and can be compiled or installed on Debian, Fedora,
OpenSUSE, Ubuntu, MacOS and Windows.

### Precompiled binaries

For each release, Talipot offers precompiled binaries for Linux (using
[AppImage](https://github.com/AppImage/AppImageKit)), MacOS (dmg bundles) or Windows
([NSIS](https://nsis.sourceforge.io/Main_Page) based installers).

### Compiling from scratch

Talipot can be easily compiled on every supported platforms. However, that process can take
some times depending on your system configuration.

The following dependencies are required to build Talipot:

  * [CMake](https://cmake.org) >= 3.10
  * A [C++20](https://en.wikipedia.org/wiki/C%2B%2B20) compiler : [GCC](https://gcc.gnu.org) >= 8
  [Clang](https://clang.llvm.org) >= 9 or
  [Microsoft Visual Studio](https://www.visualstudio.com) >= 2019
  * [FreeType](https://www.freetype.org)
  * [FriBidi](https://github.com/fribidi/fribidi)
  * [Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/)
  * [zlib](https://zlib.net)
  * [zstd](https://github.com/facebook/zstd)
  * [libgit2](https://libgit2.org/)
  * [Qt](https://www.qt.io) >= 5.12.0
  * [OpenGL](https://www.opengl.org) >= 2.0
  * [GLEW](https://github.com/nigels-com/glew) >= 1.4
  * [Qhull](https://github.com/qhull/qhull)
  * [Graphviz](https://graphviz.org/)
  * [QuaZIP](https://github.com/stachenov/quazip)
  * [yajl](https://lloyd.github.io/yajl) >= 2.0
  * [Python](https://www.python.org) >= 3.0
  * [SIP](https://github.com/Python-SIP/sip) >= 6.8.5

In order to generate the documentation, the following tools must be installed:

  * [Sphinx](https://www.sphinx-doc.org) to build the User Manual, Developer Handbook and
    Python bindings documentation
  * [Doxygen](https://www.doxygen.nl) to build the C++ API documentation

If you are a Linux user, all these dependencies can be installed with the package manager
of your distribution.

If you are a MacOS user, we recommend to use [MacPorts](https://www.macports.org) or
[Homebrew](https://brew.sh) in order to easily install all these dependencies.

If you are a Windows user, we recommend to use [MSYS2](https://www.msys2.org) as it
greatly facilitates the build of Talipot on that platform (notably by providing up
to date compilers and precompiled dependencies).

Hints on how to build Talipot for these three platforms can be found in the continuous
integration setup for [GitHub Actions](https://github.com/features/actions):

  * [ubuntu-build.yml](.github/workflows/ubuntu-build.yml)
  * [macos-homebrew-build.yml](.github/workflows/macos-homebrew-build.yml)
  * [macos-macports-build.yml](.github/workflows/macos-macports-build.yml)
  * [windows-mingw64-build.yml](.github/workflows/windows-mingw64-build.yml)
  * [windows-msvc-build.yml](.github/workflows/windows-msvc-build.yml)


## License

Talipot is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3
of the License, or (at your option) any later version.

Talipot is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
