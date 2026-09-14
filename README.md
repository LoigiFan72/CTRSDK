# CtrSDK

This is a decompilation of the CtrSDK, the standard SDK for the nintendo 3Ds.

The objective is to recreate the SDK as accurately as possible.

File names, function names and the file organization come from debugging symbols, assertions and information in all of the aforementioned titles.
Nobody except Nintendo has the source code of sead, not even third-party developers.

Note that many names (especially for inlined, templated functions) are just plain guesses.

Also note, macros, and other are user generated. All macros are custom and **must** not be obtained via illegal methods, please.

## Folder Structure

* **/LIBRARY_ROOT/CTR_SDK/**

    |____ **include/GLES2** - GL Headers used in the *CtrSDK*.

    |____ **include/nn** - nn Headers used for the *CtrSDK*.

    |____ **library** - Libraries used by the *CtrSDK*.

    |____ **profiler/sources** - 3DS CPU Profiler used in debug builds.

    |____ **sources/libraries** - Module source code.

## Libraries

* **ishio** - The Intelligent Systems HostIO Communicator
* **kmc** - KMC HostIO Helper used for CTR.

## nn Libraries

For progress, refer to [the GitHub project page](https://github.com/LoigiFan72/sead). Several modules currently fail to build for Switch.

* **applet** - Application (Initialization, sleep, finalization etc.)
* **camera** - Camera
* **cec** - Streetpass
* **cfg** - device Config
* **crt0** - C++ Runtime
* **codec** - Code Decryption (IR Helper, cfg helper)
* **crypto** - Hash handler (SHA block, general hash reading, etc.)
* **CTR** - Program ID handler (Misc mainly)
* **cx** - Context (LZ11 / ZLIB File handlers)
* **dbg** - Debug (Printing, halting, device panicing)
* **dbm** - File Server helper
* **dev** - Developer HostIO Initialzation
* **drivers** - GX Drivers
* **dsp** - DSP Audio
* **err** - Error
* **erreula** - Error EULA (For EULAS)
* **fnd** - Foundation
* **font** - Font (Identical to `nw::font`)
* **fs** - File Server I/O
* **fslow** - Device file I/O backend
* **gr** - Geometry
* **gx** - GX manager for GL
* **gxlow** - GX backend manager for GL
* **hardware** - Register access (ARMv11)
* **hid** - Human Interactable Device
* **hidlow** - Human Interactable Device backend
* **hio** - HostIO (Communication with PCs)
* **init** - crt0 Initializers
* **ir** - IR sensor
* **math** - Maths utilities (vector, matrix, etc.)
* **mic** - Microphone
* **ndm** - Wifi manager
* **nstd** - NintendoStandard (std print, Memory moving / copying)
* **os** - Operating System (Initializing, OS things)
* **pl** - Pedometer helper
* **random** - Random number generator
* **resource** - Resource (loading, decompressing, etc.)
* **svc** - Device svc
* **ubl** - Black list library
* **ulcd** - ULCD Left/Right stereo manager
* **util** - Utilities (Since this uses C++03 these contain some pass arrounds)

### Version specific source

Different features of sead can be implemented/left out in conjunction to which game the library is being used for:

Set `NN_VERSION` to:
- `NN_VERSION_CUSTOM`     (0): Starters for making a new configuration. 
- `NN_VERSION_MILLI4C`    (1): Mario & Luigi Dream Team
- `NN_VERSION_REDPEPPER`  (2): Super Mario 3D Land
- `NN_VERSION_CTRDASH`    (3): Mario Kart 7
- `NN_VERSION_GARDEN`     (4): Animal Crossing New Leaf: Welcome Amiibo!
- `NN_VERSION_STICKSTR`   (5): Paper Mario: Sticker Star

Presets and features for more games can be added if desired.

## Building

Building this project requires:

- ARM C++ Complier (ARMCC) Version 4.0/4.1/5.0 [which can be found here.](https://github.com/RE-Pepper/data/releases/tag/dasdasdsa)

### Configuration

sead can be configured with several compile-time defines:

* `NN_BUILD_DEBUG`: Enables assertions. (Note: Debug builds use ARM flags `-O0` `-Otime`)
* `NN_BUILD_DEVELOPMENT`: Enables assertions but builds optimized. (Note: Development builds use ARM flags `-O3` `-Otime`)
* `NN_BUILD_RELEASE`: Disables assertions. (Note: Release builds use ARM flags `-O3` `-Otime`)
* `NN_PLATFORM_HAS_MMU`: If the device uses `nn::srv::Initialize` in its `nninitSystem` function enable this.

Other platforms (generic Unix, iOS, Android, CTR) are not supported.

## Contributing

### Non-inlined functions
When **implementing non-inlined functions**, please compare the assembly output against the original function and make it match the original code. At this scale, that is pretty much the only reliable way to ensure accuracy and functional equivalency.

However, given the large number of functions, certain kinds of small differences can be ignored when a function would otherwise be equivalent:

* Regalloc differences.

* Instruction reorderings when it is obvious the function is still semantically equivalent (e.g. two add/mov instructions that operate on entirely different registers being reordered)

When ignoring minor differences, add a `// NOT_MATCHING: explanation` comment and explain what does not match.

### Header utilities or inlined functions
For **header-only utilities** (like container classes), use pilot/debug builds, assertion messages and common sense to try to undo function inlining. For example, if you see the same assertion appear in many functions and the file name is a header file, or if you see identical snippets of code in many different places, chances are that you are dealing with an inlined function. In that case, you should refactor the inlined code into its own function.

Also note that introducing inlined functions is sometimes necessary to get the desired codegen.

If a function is inlined, you should try as hard as possible to make it match perfectly. For inlined functions, it is better to use weird code or small hacks to force a match as differences would otherwise appear in every single function that inlines the non-matching code, which drastically complicates matching other functions. If a hack is used, wrap it inside a `#ifdef MATCHING_HACK_CTR` (see above for a list of defines).