/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/copyStrings/Options.h>

using builtin::copyStrings::Options;

Options::
Options() :
    _validate(false)
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

    if (arg == "--validate") {
        return libutil::Options::MarkBool(&_validate, arg, it);
    } else if (arg == "--inputencoding") {
        return libutil::Options::NextString(&_inputEncoding, args, it);
    } else if (arg == "--outputencoding") {
        return libutil::Options::NextString(&_outputEncoding, args, it);
    } else if (arg == "--outdir") {
        return libutil::Options::NextString(&_outputDirectory, args, it);
    } else if (arg == "--") {
        // TODO(grp): Set a flag to avoid parsing later arguments as options.
        return std::make_pair(true, std::string());
    } else if (!arg.empty() && arg[0] != '-') {
        _inputs.push_back(arg);
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "unknown argument " + arg);
    }
}

