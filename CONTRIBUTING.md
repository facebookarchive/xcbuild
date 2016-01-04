# Contributing

xcbuild actively welcomes contributions from the community. If you run into problems, please open an issue. Pull requests are great too!

## Getting Started

To learn about xcbuild, start with the techincal overview above. Each of the components of xcbuild is documented in the `Headers` directory, with both module and API documentation available.

If you're new to xcbuild, we have a [list of issues](https://github.com/facebook/xcbuild/labels/starter) designed for new contributors to get started in the codebase. We're always happy to help people working on these issues and learning about xcbuild.

## Style

C++ can be complex, so xcbuild tries to stick with a reasonable subset modeled after Swift, Rust, and other modern langages. Prefer immutable structures and classes. Prefer passing by const reference, and avoid non-const references. Templates are avoided when possible. Exceptions and RTTI are not used.

## Contributor License Agreement ("CLA")

In order to accept your pull request, we need you to [submit a CLA](https://code.facebook.com/cla). You only need to do this once to work on any of Facebook's open source projects.

## Code of Conduct

This project adheres to the [Open Code of Conduct](http://todogroup.org/opencodeofconduct/#xcbuild/opensource@fb.com). By participating, you are expected to honor this code.

## License

By contributing to xcbuild you agree that your contributions will be licensed under its BSD license.
