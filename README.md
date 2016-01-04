# xcbuild

`xcbuild` is an Xcode-compatible build tool. It's major goal is to be compatible with the `xcodebuild` command-line tool. Additional goals are to make faster builds, document the build process in code, and to run on multiple platforms (especially OS X and Linux).

In its current state, xcbuild can build many Xcode projects completely.

## Usage

xcbuild uses [git submodules](https://www.kernel.org/pub/software/scm/git/docs/git-submodule.html) for dependencies, so be sure to run `git submodule update --init` after cloning the repository.

[CMake](http://www.cmake.org) and [Ninja](https://martine.github.io/ninja/) (or [llbuild](https://github.com/apple/swift-llbuild)) are used to build. You'll need to install both, then type `make` to build. Output is put into the `build` directory.

Run xcbuild directly out of the build directory: `./build/xcbuild`. The command line options are identical to those for `xcodebuild`. You'll need a copy of Xcode installed to build sucessfully.

To use the (much faster) Ninja (or llbuild) executor, pass the extension option `-executor ninja` to `xcbuild`. Then, run `ninja -f` (or `llbuild ninja build -f`) with the path printed.

## Overview

To make xcbuild useful as more than just a build tool, it's built as a collection of libraries. Each of these libraries has its headers under `Headers` and `PrivateHeaders`, source code under `Sources`, and unit tests under `Tests`. Under `Tools` are the executables that use the xcbuild libraries, including `xcbuild` itself.

High level overview of each component library:

 - `plist`: *Standalone.* Reader, writer, and model for ASCII, binary, and XML property lists.
 - `ninja`: *Standalone.* Generator for Ninja build files.
 - `libutil`: Common utilities and platform abstractions.
 - `pbxsetting`: Build setting parsing, levels, and resolution.
 - `xcsdk`: Loads the available platforms, SDKs, and toolchains in a developer root.
 - `pbxspec`: Parses available build settings, tools, and file types into strongly-typed structures.
 - `pbxproj`: Parses project files into strongly-typed structures.
 - `xcworkspace`: Parses workspaces into strongly-typed structures.
 - `xcscheme`: Parses schemes into strongly-typed structures.
 - `builtin`: Built-in tools used for building, like file copying and validation.
 - `pbxbuild`: Performs the build. Made up of a number of subcomponents:
   - `Build`: Top-level context for the scheme, targets, and action for a build.
   - `Target`: Creates the build environment and settings for each target.
   - `Phase`: Implementations for the various build phases (compile, link, copy, etc).
   - `Tool`: Tool implementations for generating command line invocations.
   - `Action`: Build execution engines and log formatting.
 - `xcdriver`: Parses command line options and drives the build.

## Contributing

xcbuild actively welcomes contributions from the community. If you're interested in contributing, be sure to check out the [contributing guide](https://github.com/facebook/xcbuild/blob/master/CONTRIBUTING.md). It includes some tips for getting started in the codebase, as well as important information about the code of conduct, license, and CLA.

## Thanks

xcbuild is built on build system documentation from the community. In particular, thanks to these people for their writing:

 - [Samantha Marshall](http://pewpewthespells.com)
 - [Damien Bobillot](http://maxao.free.fr/xcode-plugin-interface/)
 - [Michele Titolo](http://michele.io)
 - [Laurent Etiemble](http://www.monobjc.net/xcode-project-file-format.html)
 - [Apple Developer](https://developer.apple.com/legacy/library/documentation/DeveloperTools/Conceptual/XcodeBuildSystem/Xcode_Build_System.pdf)

Third-party licenses are listed in the `LICENSE` document.
