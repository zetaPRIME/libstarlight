
# <p align=center> - libstarlight -

### <p align=center> 3DS Homebrew Application Framework
---
(Pardon the dust, both the library and this readme are still a work in progress)

## What is this?
libstarlight is a fully-featured application framework for 3DS homebrew written in C++14.

Features include:
* Fully themable UI framework with rich widgets designed for the 3DS's touchscreen
* Underlying graphics framework built on citro3d, using premultiplied alpha blending
<br> (textures automatically converted on load)
* Streamlined, easy-to-use onscreen keyboard
* Centralized configuration system for application-specific settings and storage, complete with optional auto-saving on exit
* Filesystem helpers to streamline working with SD and romfs contents
* Various basic datatypes for convenience and interoperability

Please keep in mind that while libstarlight is still in beta (pre-1.0.0) not everything is final. Expect occasional (usually minor) breaking changes as things settle.

## Requirements
libstarlight requires devkitARM r46 or later, along with:
* reasonably up-to-date libctru
* citro3d

Additionally, libstarlight contains versions of the following bundled within:
* [lodepng](https://github.com/lvandeve/lodepng)
* [nlohmann::json (JSON For Modern C++)](https://github.com/nlohmann/json)

## Okay, so how do I use this?
(section WIP, take a look at the testbed for a somewhat messy example)

To ensure your application runs properly without themes installed to the SD card, it is recommended to include a copy of the default theme (or any theme with no fallback) at `romfs:/.fallback_theme`.
 
## License
* MIT (see [license.md](license.md))

Bundled dependencies (see [\_incLib](libstarlight/source/starlight/_incLib)) have their own terms and/or notices, as listed in their respective files.
