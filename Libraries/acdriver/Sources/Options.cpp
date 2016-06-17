/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/Options.h>

using acdriver::Options;

Options::
Options() :
    _version(false),
    _printContents(false),
    _warnings(false),
    _errors(false),
    _notices(false),
    _compressPNGs(false),
    _targetDevice({}),
    _enableOnDemandResources(false),
    _enableIncrementalDistill(false)
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

    if (arg == "--version") {
        return libutil::Options::MarkBool(&_version, arg);
    } else if (arg == "--print-contents") {
        return libutil::Options::MarkBool(&_printContents, arg);
    } else if (arg == "--compile") {
        return libutil::Options::NextString(&_compile, args, it);
    } else if (arg == "--output-format") {
        return libutil::Options::NextString(&_outputFormat, args, it);
    } else if (arg == "--warnings") {
        return libutil::Options::MarkBool(&_warnings, arg);
    } else if (arg == "--errors") {
        return libutil::Options::MarkBool(&_errors, arg);
    } else if (arg == "--notices") {
        return libutil::Options::MarkBool(&_notices, arg);
    } else if (arg == "--export-dependency-info") {
        return libutil::Options::NextString(&_exportDependencyInfo, args, it);
    } else if (arg == "--optimization") {
        return libutil::Options::NextString(&_optimization, args, it);
    } else if (arg == "--compress-pngs") {
        return libutil::Options::MarkBool(&_compressPNGs, arg);
    } else if (arg == "--platform") {
        return libutil::Options::NextString(&_platform, args, it);
    } else if (arg == "--minimum-deployment-target") {
        return libutil::Options::NextString(&_minimumDeploymentTarget, args, it);
    } else if (arg == "--target-device") {
        std::string targetDevice;
        auto result = libutil::Options::NextString(&targetDevice, args, it);
        if (result.first) {
            _targetDevice.push_back(targetDevice);
        }
        return result;
    } else if (arg == "--output-partial-info-plist") {
        return libutil::Options::NextString(&_outputPartialInfoPlist, args, it);
    } else if (arg == "--app-icon") {
        return libutil::Options::NextString(&_appIcon, args, it);
    } else if (arg == "--launch-image") {
        return libutil::Options::NextString(&_launchImage, args, it);
    } else if (arg == "--enable-on-demand-resources") {
        return libutil::Options::NextBool(&_enableOnDemandResources, args, it, true);
    } else if (arg == "--enable-incremental-distill") {
        return libutil::Options::MarkBool(&_enableIncrementalDistill, arg);
    } else if (arg == "--target-name") {
        return libutil::Options::NextString(&_targetName, args, it);
    } else if (arg == "--filter-for-device-model") {
        return libutil::Options::NextString(&_filterForDeviceModel, args, it);
    } else if (arg == "--filter-for-device-os-version") {
        return libutil::Options::NextString(&_filterForDeviceOsVersion, args, it);
    } else if (!arg.empty() && arg[0] != '-') {
        _inputs.push_back(arg);
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "unknown argument " + arg);
    }
}

