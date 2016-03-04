/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/productPackagingUtility/Options.h>

using builtin::productPackagingUtility::Options;

Options::
Options()
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

    if (arg == "-removefile") {
        return libutil::Options::MarkBool(&_removeFile, arg, it);
    } else if (arg == "-entitlements") {
        return libutil::Options::MarkBool(&_entitlements, arg, it);
    } else if (arg == "-resourcerules") {
        return libutil::Options::MarkBool(&_resourceRules, arg, it);
    } else if (arg == "-o") {
        return libutil::Options::NextString(&_output, args, it);
    } else if (!arg.empty() && arg[0] != '-') {
        if (_input.empty()) {
            _input = arg;
            return std::make_pair(true, std::string());
        } else {
            return std::make_pair(false, "multiple inputs");
        }
    } else {
        return std::make_pair(false, "unknown argument " + arg);
    }
}

