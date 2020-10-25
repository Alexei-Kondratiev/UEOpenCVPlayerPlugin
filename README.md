# UEOpenCVPlayerPlugin

![UEOpenCVPlayerPlugin - Unreal Editor.png](https://github.com/Alexei-Kondratiev/UEOpenCVPlayerPlugin/blob/master/Videos/UEOpenCVPlayerPlugin%20-%20Unreal%20Editor.png?raw=true)
Unreal Engine 4 Media Framework plug-in using the OpenCV 3 (Open Source Computer Vision Library) library.


## Videos
![UEOpenCVPlayerPlugin - Unreal Editor.mp4](https://github.com/Alexei-Kondratiev/UEOpenCVPlayerPlugin/blob/master/Videos/UEOpenCVPlayerPlugin%20-%20Unreal%20Editor.mp4?raw=true)


## About

This plug-in is still under development and likely has a lot of remaining issues
to be fixed. Use in production is not yet recommended.

Make sure to pull the *Tag* that matches your Unreal Engine version. If you sync
to *Master* the code may not compile, because it may depend on Engine changes
that are not yet available in the UE4 Master branch.


## Supported Platforms

This plug-in was last built against **Unreal Engine 4.19** and tested
against the following platforms:

- Linux (Ubuntu 18.04 LTS (Bionic Beaver))
- ~~Mac~~
- Windows

**IMPORTANT**: Please note that this repository contains pre-compiled binaries
for opencv_ffmpeg, which are licensed under LGPL. This means that you
cannot create monolithic builds of your game without violating LGPL, the UE4
EULA or both. The opencv_ffmpeg libraries must remain dynamic libraries that are bound
at run-time - static linking is not allowed - and the licensing related files must be retained.

This also means that this plug-in cannot work on platforms that do not support
dynamically linked libraries (i.e. iOS, HTML5) or do not currently implement
support for it.


## Prerequisites

A relatively recent version of OpenCV 3 is required. The Ubuntu default stable release
is not sufficient.

### Linux (Ubuntu 18.04 LTS (Bionic Beaver))

A suitable version of OpenCV 3 must be installed or compiled from source. If
you ship your game on Linux, you will likely want to include OpenCV 3 library with it, so
that users don't have to install it themselves. I am including those
binaries in this repository, although it is not clear what distros should be
supported and where the builds are coming from.

Use clang as default C/C++ compiler for Ubuntu 18.04 LTS.

### Mac, Windows

All required libraries and plug-ins are included in the *Binaries* directory
and work out of the box.


## Dependencies

This plug-in requires Visual Studio 2017 and either a C++ code project or the full
Unreal Engine 4 source code from GitHub. If you are new to programming in UE4,
please see the official [Programming Guide](https://docs.unrealengine.com/latest/INT/Programming/index.html)! 


## Usage

You can use this plug-in as a project plug-in, or an Engine plug-in.

If you use it as a project plug-in, clone this repository into your project's
*/Plugins* directory and compile your game in Visual Studio 2017. A C++ code project
is required for this to work.

If you use it as an Engine plug-in, clone this repository into the
*/Engine/Plugins/Media* directory and compile your game. Full Unreal Engine 4
source code from GitHub is required for this.


## References

* [Introduction to UE4 Plugins](https://ue4community.wiki/topic/plugins)
