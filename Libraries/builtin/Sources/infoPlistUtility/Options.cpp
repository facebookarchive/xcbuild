/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/infoPlistUtility/Options.h>

using builtin::infoPlistUtility::Options;

Options::
Options() :
    _expandBuildSettings(false)
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

    if (arg == "-genpkginfo") {
        return libutil::Options::NextString(&_genPkgInfo, args, it);
    } else if (arg == "-resourcerulesfile") {
        return libutil::Options::NextString(&_resourceRulesFile, args, it);
    } else if (arg == "-expandbuildsettings") {
        return libutil::Options::MarkBool(&_expandBuildSettings, arg, it);
    } else if (arg == "-format") {
        return libutil::Options::NextString(&_format, args, it);
    } else if (arg == "-platform") {
        return libutil::Options::NextString(&_platform, args, it);
    } else if (arg == "-requiredArchitecture") {
        std::string architecture;
        std::pair<bool, std::string> result = libutil::Options::NextString(&architecture, args, it);
        if (result.first) {
            _requiredArchitectures.push_back(architecture);
        }
        return result;
    } else if (arg == "-additionalcontentfile") {
        std::string additional;
        std::pair<bool, std::string> result = libutil::Options::NextString(&additional, args, it);
        if (result.first) {
            _additionalContentFiles.push_back(additional);
        }
        return result;
    } else if (arg == "-infofilekeys") {
        return libutil::Options::NextString(&_infoFileKeys, args, it);
    } else if (arg == "-infofilevalues") {
        return libutil::Options::NextString(&_infoFileValues, args, it);
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

