# MovieCatalog
A program to catalog a private collection of movies inside a SQLite3 database using wxWidgets.
This program compiles and builds in MSVC (win32) and in linux(-mint).



**Updates in 3.0.1.5 :**
- It seems __WX_DEBUG__ is allways set, even if in Release mode, so I had to figure out some hack to have some debug-messages to hide in Release-build, but not when in Debug build.
- When opening the dialog in MoviesView to edit the URL, there was a hidden focus onto the OK button (which you won't see if the dialog is launched). I fixed this by forcing a setfocus on the first edit-control.
- I installed Ghostwriter (flatpack) to edit this readme.md file.

**Updates in 3.0.1.0 :**
- OpenImgDialog will open current image (if any) already in preview
- Covers inside TMovies will be prefixed by a prefix-path (if set) in '[Movies]Covers' in the INI
- Programmaticly possible to resort the covers on the local disk by setting [Compact]NewCovers and (if "[Movies]Covers" not set) setting [Compact]OldCovers.
  -> 'NewCovers' directs to a non-existing path where the covers will be copied to
  -> 'OldCovers' directs to the location where all Covers are currently stored
  -> if either 'NewCovers' and/or 'OldCovers' does not contain any path-seperators, the program assumes it's a subdir for the default 'AppDataDir'
 - if '[Compact]Dry' is set when doing a Compact, only the covers are resorted (while the real Compact can take a very long time to complete)
- All Views will keep its current position on the screen, which are set in the config/ini file, so next time that view will come at the last position

The sources requires wxWidgets 3.2.4 (or higher) to compile and operate properly. Version 324 is by default installed in linux using apt.

First build (v3.0.0.0) had vcpkg in msw which worked, though in current rebuild (v3.0.0.1 and up) vcpkg is broken, at least I cannot get a proper wxWidgets package.
So using CMake is only supported when building for Linux.
For MSW we must use MS Visual Studio with the packages as described below, under [MSW].

Do a git clone https://github.com/lhksoft/MovieCatalog.git
The contents of msvs should be copied into your Projects folder of MS Visual Studio.
Then copy the dirs (with contents) of 'MoviesCatalogs', 'lkControls' and 'lkSQLite3' into that folder.

LINUX :
- requires 'libwxbase3.2-1t64'    (this probably will be at your os by default, if not install it)
- requires 'libwxgtk-gl3.2-1t64'  (this probably will be at your os by default, if not install it)
- requires 'libwxgtk3.2-1t64'     (this probably will be at your os by default, if not install it)
- requires 'libwxgtk3.2-dev'      (sudo apt update & sudo apt install libwxgtk3.2-dev)
- requires 'libsqlite3-0'         (sudo apt update & sudo apt install libsqlite3-0)
- requires 'libsqlite3-dev'       (sudo apt update & sudo apt install libsqlite3-dev)
- requires 'libgtk-3-0t64'        (this probably will be at your os by default, if not install it)
- requires 'libgtk-3-dev'         (sudo apt update & sudo apt install libgtk-3-dev)

How to build (with CMake)
## Configure a release build

```
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release
```

> If you want the binary to come into your local bin-folder (not globaly installed), use following (do not add /bin, it will come in bin anyhow) :
```
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release --install-prefix=/home/<your-user-name>
```

## Build release binaries

```
cmake --build build/
```

## Install

```
cmake --install build/
```



MSW :
* For wxWidgets :
- Download https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.4/wxMSW-3.2.4_vc14x_x64_Dev.7z
- Download https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.4/wxWidgets-3.2.4-headers.7z
- -> Unpack into 'basefolder-of-MoviesCatalog'\wx324
- Download https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.4/wxMSW-3.2.4_vc14x_x64_ReleaseDLL.7z
- -> Unpack into 'basefolder-of-MoviesCatalog'\wx324\lib\Release
- Now copy the *.dll and *.pdb from 'basefolder-of-MoviesCatalog'\wx324\lib\vc14x_x64_dll into (create if not yet existing) 'basefolder-of-MoviesCatalog'\X64\Debug
- also copy the *.dll from 'basefolder-of-MoviesCatalog'\wx324\lib\Release  into (create if not yet existing) 'basefolder-of-MoviesCatalog'\X64\Release
* For sqlite3 :
- Download https://sqlite.org/2025/sqlite-amalgamation-3500400.zip
- -> Unpack only the HEADERS to e.g. C:\sqlite
- Download https://sqlite.org/2025/sqlite-dll-win-x64-3500400.zip
- -> Unpack to same path as previous, e.g. C:\sqlite
- Start 'Developper Command Prompt' from "Start|Visual Studio 2022|Developper Command Prompt for VS 2022"
- -> goto same path as previous, e.g. C:\sqlite
- -> run : lib /def:sqlite3.def /machine:X64
-   -> this will create a .lib file to include in MSVS project
- In the 'basefolder-of-MoviesCatalog' create "sqlite3"
- -> create following dirs inside that folder ('basefolder-of-MoviesCatalog'\sqlite3) : bin, include, lib
- From the tempory path (e.g. C:\sqlite) copy following :
- -> the .dll into sqlite3\bin
- -> all .h   into sqlite3\include
- -> the .lib (and if available also the .exp) into sqlite3\lib
- now copy the .dll from sqlite3\bin into both 'basefolder-of-MoviesCatalog'\X64\Debug and 'basefolder-of-MoviesCatalog'\X64\Release
- Be sure you've done previous before opening the solution in MSVC, because the solution depends on the property file which will be extracted into 'basefolder-of-MoviesCatalog'\wx324


*This file is rewritten in Ghostwriter*