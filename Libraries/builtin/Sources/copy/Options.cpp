/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/copy/Options.h>

using builtin::copy::Options;

Options::
Options() :
    _verbose(false),
    _preserveHFSData(false),
    _ignoreMissingInputs(false),
    _resolveSrcSymlinks(false),
    _stripDebugSymbols(false),
    _bitcodeStrip(BitcodeStripMode::None)
{
}

Options::
~Options()
{
}

std::pair<bool, std::string> Options::
parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string const &arg = **it;

    if (arg == "-V") {
        return libutil::Options::MarkBool(&_verbose, arg, it);
    } else if (arg == "-preserve-hfs-data") {
        return libutil::Options::MarkBool(&_preserveHFSData, arg, it);
    } else if (arg == "-ignore-missing-inputs") {
        return libutil::Options::MarkBool(&_ignoreMissingInputs, arg, it);
    } else if (arg == "-resolve-src-symlinks") {
        return libutil::Options::MarkBool(&_resolveSrcSymlinks, arg, it);
    } else if (arg == "-exclude") {
        std::string exclude;
        std::pair<bool, std::string> result = libutil::Options::NextString(&exclude, args, it);
        if (result.first) {
            _excludes.push_back(exclude);
        }
        return result;
    } else if (arg == "-strip-debug-symbols") {
        return libutil::Options::MarkBool(&_stripDebugSymbols, arg, it);
    } else if (arg == "-strip-tool") {
        return libutil::Options::NextString(&_stripTool, args, it);
    } else if (arg == "-bitcode-strip") {
        std::string mode;
        std::pair<bool, std::string> result = libutil::Options::NextString(&mode, args, it);
        if (result.first) {
            if (mode == "none") {
                _bitcodeStrip = BitcodeStripMode::None;
            } else if (mode == "replace-with-marker") {
                _bitcodeStrip = BitcodeStripMode::ReplaceWithMarker;
            } else if (mode == "all") {
                _bitcodeStrip = BitcodeStripMode::All;
            } else {
                return std::make_pair<bool, std::string>(false, "unknown bitcode strip mode");
            }
        }
        return result;
    } else if (arg == "-bitcode-strip-tool") {
        return libutil::Options::NextString(&_bitcodeStripTool, args, it);
    } else if (!arg.empty() && arg[0] != '-') {
        if (*it == std::prev(args.end())) {
            _output = arg;
        } else {
            _inputs.push_back(arg);
        }
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "unknown argument " + arg);
    }
}

