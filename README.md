# FL Lighting Bug Fix
Yet another [lighting bug](https://www.moddb.com/games/freelancer/downloads/freelancer-broken-interior-lighting-fix) fix for the game Freelancer. This one is implemented by directly hooking the `IDirect3DDevice8::SetLight` call in Freelancer's render pipeline. As a result, the implementation is entirely plugin-based and does not make use of any DirectX hooks (d3d8.dll). Hence this fix is easier to combine with existing DirectX-related enhancements such as the [Subtitles project](https://the-starport.com/forums/topic/6015/subtitles-revisited-beta). Additionally, it works with any version of Windows.

## Installation instructions
Download the latest `LightingBugFix.dll` from [Releases](https://github.com/FLHDE/fl-lighting-bug-fix/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation. Next, open the `dacom.ini` file and append `LightingBugFix.dll` to the `[Libraries]` section.

## Build instructions
Build the project using the `NMAKE` tool from Microsoft Visual C++ 7.0 (VC7) + the VC6 runtime libraries (you may also need the D3D8 header files).
This will output the `LightingBugFix.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.
