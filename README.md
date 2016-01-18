# xcbuild

`xcbuild` is an Xcode-compatible build tool. It's major goal is to be compatible with the `xcodebuild` command-line tool. Additional goals are to make faster builds, document the build process in code, and to run on multiple platforms (especially OS X and Linux).

In its current state, xcbuild can build many Xcode projects completely.

#### xcbuild and xctool

Both xcbuild and [xctool](https://github.com/facebook/xctool) are alternatives to the `xcodebuild` tool. With the release of xcbuild we plan to slowly deprecate xctool's build features wrapping `xcodebuild`. For complex use cases, xcbuild's standalone implementation can be customized as needed. For simple `xcodebuild` formatting, [xcpretty](https://github.com/supermarin/xcpretty) is a good choice. Ultimately, this will leave xctool as a smaller project dedicated to running tests, which it does very well!

#### xcbuild and Buck

The main build system from Facebook is [Buck](https://buckbuild.com), which also supports iOS and OS X projects. Buck has a stronger architecture to enable advanced features like artifact caching, while having a much simpler build definition language. If you have a new project or one where Buck's features are worth converting to use, it's highly recommended.

## Usage

xcbuild uses [git submodules](https://www.kernel.org/pub/software/scm/git/docs/git-submodule.html) for dependencies, so be sure to run `git submodule update --init` after cloning the repository.

[CMake](http://www.cmake.org) and [Ninja](https://martine.github.io/ninja/) (or [llbuild](https://github.com/apple/swift-llbuild)) are used to build. You'll need to install both, then type `make` to build. Output is put into the `build` directory.

Run xcbuild directly out of the build directory: `./build/xcbuild`. The command line options are identical to those for `xcodebuild`. You'll need a copy of Xcode installed to build successfully.

To use the much faster Ninja (or llbuild) executor, pass the extension option `-executor ninja` to `xcbuild`. Then, run `ninja -f` (or `llbuild ninja build -f`) with the path printed.

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
