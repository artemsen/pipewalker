# PipeWalker

This is a puzzle game in which you need to combine the components into
a single circuit: connect all computers to a network server, bring water
to the taps, etc.

## Install

[![Packaging status](https://repology.org/badge/tiny-repos/pipewalker.svg)](https://repology.org/project/pipewalker/versions)

List of supported distributives can be found on the [Repology page](https://repology.org/project/pipewalker/versions).

## Build

The project uses Meson build system:
```
meson setup --buildtype=release build
ninja -C build
sudo ninja -C build install
```
