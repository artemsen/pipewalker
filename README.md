# PipeWalker

This is a puzzle game in which you need to combine the components into
a single circuit: connect all computers to a network server, bring water
to the taps, etc.

![Screenshot](https://raw.githubusercontent.com/artemsen/pipewalker/master/.github/screenshot.png)

## Install

[![Packaging status](https://repology.org/badge/tiny-repos/pipewalker.svg)](https://repology.org/project/pipewalker/versions)

List of supported distributives can be found on the [Repology page](https://repology.org/project/pipewalker/versions).

## Build

To build a project you need a C++ compiler, meson, libraries SDL2 and SDL2 image:
```
sudo apt install build-essential libsdl2-dev libsdl2-image-dev
meson setup --buildtype=release build
ninja -C build
sudo ninja -C build install
```
