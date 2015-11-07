# xcbuild

`xcbuild` is an Xcode-compatible build tool. It's major goal is to be compatible with the `xcodebuild` command-line tool. Additional goals are to make faster builds, document the build process in code, and to run on multiple platforms (especially OS X and Linux).

In its current state, xcbuild can build many Xcode projects completely.

## Usage

xcbuild uses [git submodules](https://www.kernel.org/pub/software/scm/git/docs/git-submodule.html) for dependencies, so be sure to run `git submodule update --init` after cloning the repository.

[CMake](http://www.cmake.org) and [Ninja](https://martine.github.io/ninja/) are used to build. You'll need to install both, then type `make` to build. Output is put into the `build` directory.

Run xcbuild directly out of the build directory: `./build/xcbuild`. The command line options are identical to those for `xcodebuild`. You'll need a copy of Xcode installed to build apps sucessfully.

## Overview

To make xcbuild useful as more than just a build tool, it's built as a collection of libraries. Each of these libraries has its headers under `Headers` and `PrivateHeaders`, source code under `Sources`, and unit tests under `Tests`. Under `Tools` are the executables that use the xcbuild libraries, including `xcbuild` itself.

High level overview of each component library:

 - `libutil`: Common utilities and platform abstractions.
 - `plist`: Read property list files. All formats (text, binary, and XML) are supported.
 - `pbxsetting`: Build setting parsing, levels, and resolution.
 - `xcsdk`: Loads the available platforms, SDKs, and toolchains in a developer root.
 - `pbxspec`: Parses available build settings, tools, and file types into strongly-typed structures.
 - `pbxproj`: Parses project files into strongly-typed structures.
 - `xcworkspace`: Parses workspaces into strongly-typed structures.
 - `xcscheme`: Parses schemes into strongly-typed structures.
 - `builtin`: Built-in tools used for building, like file copying and validation.
 - `xcbuild`: Performs the build. Made up of a number of subcomponents:
   - Top-level context for the scheme, targets, and action for a build.
   - Implementations for the various build phases (compile, link, copy, etc).
   - Tool implementations for generating command line invocations.
   - Build execution engine and log formatting.
 - `xcdriver`: Parses command line options and drives the build.

## Contributing

xcbuild actively welcomes contributions from the community. If you run into problems, please open an issue. Pull requests are great too!

### Getting Started

To learn about xcbuild, start with the techincal overview above. Each of the components of xcbulid is documented in the `Headers` directory, with both module and API documentation available.

If you're new to xcbuild, we have a [list of xissues](https://github.com/facebook/xcbuild/labels/help%20wanted) designed for new contributors to get started in the codebase. We're always happy to help people working on these issues and learning about xcbuild.

### Style

C++ can be complex, so xcbuild tries to stick with a reasonable subset modeled after Swift, Rust, and other better langages. Prefer immutable structures and classes. Prefer passing by const reference, and avoid non-const references. Exceptions and RTTI are not used.

### Contributor License Agreement ("CLA")

In order to accept your pull request, we need you to [submit a CLA](https://code.facebook.com/cla). You only need to do this once to work on any of Facebook's open source projects.

### Code of Conduct

This project adheres to the [Open Code of Conduct](http://todogroup.org/opencodeofconduct/#xcbuild/opensource@fb.com). By participating, you are expected to honor this code.

### License

By contributing to xcbuild you agree that your contributions will be licensed under its BSD license.

## Thanks

xcbuild is built on build system documentation from the community. In particular, thanks to these people for their writing:

 - [Samantha Marshall](http://pewpewthespells.com)
 - [Damien Bobillot](http://maxao.free.fr/xcode-plugin-interface/)
 - [Michele Titolo](http://michele.io)
 - [Laurent Etiemble](http://www.monobjc.net/xcode-project-file-format.html)
 - [Apple Developer](https://developer.apple.com/legacy/library/documentation/DeveloperTools/Conceptual/XcodeBuildSystem/Xcode_Build_System.pdf)

Third-party licenses are listed in the `LICENSE` document.
