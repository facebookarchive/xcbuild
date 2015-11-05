/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/Options.h>

using xcdriver::Options;

Options::
Options() :
    _usage                     (false),
    _help                      (false),
    _verbose                   (false),
    _license                   (false),
    _checkFirstLaunchStatus    (false),
    _version                   (false),
    _allTargets                (false),
    _parallelizeTargets        (false),
    _jobs                      (0),
    _dryRun                    (false),
    _hideShellScriptEnvironment(false),
    _list                      (false),
    _showSDKs                  (false),
    _showBuildSettings         (false),
    _enableAddressSanitizer    (false),
    _enableCodeCoverage        (false),
    _exportArchive             (false),
    _skipUnavailableActions    (false),
    _exportLocalizations       (false),
    _importLocalizations       (false),
    _forceImport               (false)
{
}

Options::
~Options()
{
}

static std::pair<bool, std::string>
NextString(std::string *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string const &arg = **it;
    ++(*it);

    if ((*it) != args.end()) {
        if (result->empty()) {
            *result = **it;
            return std::make_pair(true, std::string());
        } else {
            return std::make_pair(false, "duplicate argument " + arg);
        }
    } else {
        return std::make_pair(false, "missing argument value for argument " + arg);
    }
}

static std::pair<bool, std::string>
NextInt(int *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string str = std::to_string(*result);
    std::pair<bool, std::string> success = NextString(&str, args, it);
    if (success.first) {
        *result = std::atoi(str.c_str());
    }
    return success;
}

static std::pair<bool, std::string>
NextBool(bool *result, std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string str = *result ? "YES" : "NO";
    std::pair<bool, std::string> success = NextString(&str, args, it);
    if (success.first) {
        if (str == "YES") {
            *result = true;
        } else if (str == "NO") {
            *result = false;
        } else {
            success = std::make_pair(false, "invalid value " + str + " for boolean argument");
        }
    }
    return success;
}

static std::pair<bool, std::string>
MarkBool(bool *result, std::string const &arg)
{
    if (!*result) {
        *result = true;
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "duplicate argument " + arg);
    }
}

std::pair<bool, std::string> Options::
parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string const &arg = **it;

    if (arg == "-usage") {
        return MarkBool(&_usage, arg);
    } else if (arg == "-help") {
        return MarkBool(&_help, arg);
    } else if (arg == "-verbose") {
        return MarkBool(&_verbose, arg);
    } else if (arg == "-license") {
        return MarkBool(&_license, arg);
    } else if (arg == "-checkFirstLaunchStatus") {
        return MarkBool(&_checkFirstLaunchStatus, arg);
    } else if (arg == "-project") {
        return NextString(&_project, args, it);
    } else if (arg == "-target") {
        return NextString(&_target, args, it);
    } else if (arg == "-alltargets") {
        return MarkBool(&_allTargets, arg);
    } else if (arg == "-workspace") {
        return NextString(&_workspace, args, it);
    } else if (arg == "-scheme") {
        return NextString(&_scheme, args, it);
    } else if (arg == "-configuration") {
        return NextString(&_configuration, args, it);
    } else if (arg == "-xcconfig") {
        return NextString(&_xcconfig, args, it);
    } else if (arg == "-arch") {
        return NextString(&_arch, args, it);
    } else if (arg == "-sdk") {
        return NextString(&_sdk, args, it);
    } else if (arg == "-toolchain") {
        return NextString(&_toolchain, args, it);
    } else if (arg == "-destination") {
        return NextString(&_destination, args, it);
    } else if (arg == "-destinationtimeout") {
        return NextString(&_destinationTimeout, args, it);
    } else if (arg == "-parallelizeTargets") {
        return MarkBool(&_parallelizeTargets, arg);
    } else if (arg == "-jobs") {
        return NextInt(&_jobs, args, it);
    } else if (arg == "-dryrun" || arg == "-n") {
        return MarkBool(&_dryRun, arg);
    } else if (arg == "-hideShellScriptEnvironment") {
        return MarkBool(&_hideShellScriptEnvironment, arg);
    } else if (arg == "-showsdks") {
        return MarkBool(&_showSDKs, arg);
    } else if (arg == "-showBuildSettings") {
        return MarkBool(&_showBuildSettings, arg);
    } else if (arg == "-list") {
        return MarkBool(&_list, arg);
    } else if (arg == "-find" || arg == "-find-executable") {
        return NextString(&_findExecutable, args, it);
    } else if (arg == "-find-library") {
        return NextString(&_findLibrary, args, it);
    } else if (arg == "-version") {
        return MarkBool(&_version, arg);
    } else if (arg == "-enableAddressSanitizer") {
        return NextBool(&_enableAddressSanitizer, args, it);
    } else if (arg == "-resultBundlePath") {
        return NextString(&_resultBundlePath, args, it);
    } else if (arg == "-derivedDataPath") {
        return NextString(&_derivedDataPath, args, it);
    } else if (arg == "-archivePath") {
        return NextString(&_archivePath, args, it);
    } else if (arg == "-exportArchive") {
        return MarkBool(&_exportArchive, arg);
    } else if (arg == "-exportOptionsPlist") {
        return NextString(&_exportOptionsPlist, args, it);
    } else if (arg == "-enableCodeCoverage") {
        return NextBool(&_enableCodeCoverage, args, it);
    } else if (arg == "-exportPath") {
        return NextString(&_exportPath, args, it);
    } else if (arg == "-skipUnavailableActions") {
        return MarkBool(&_skipUnavailableActions, arg);
    } else if (arg == "-exportLocalizations") {
        return MarkBool(&_exportLocalizations, arg);
    } else if (arg == "-importLocalizations") {
        return MarkBool(&_importLocalizations, arg);
    } else if (arg == "-localizationPath") {
        return NextString(&_localizationPath, args, it);
    } else if (arg == "-exportLanguage") {
        return NextString(&_exportLanguage, args, it);
    } else if (arg == "-forceImport") {
        return MarkBool(&_forceImport, arg);
    } else if (arg.find('=') != std::string::npos) {
        _settings.push_back(pbxsetting::Setting::Parse(arg));
        return std::make_pair(true, std::string());
    } else if (!arg.empty() && arg[0] != '-') {
        _actions.push_back(arg);
        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "unknown argument " + arg);
    }
}

std::pair<Options, std::string> Options::
Parse(std::vector<std::string> const &args)
{
    Options options;
    std::string error;

    for (auto it = args.begin(); it != args.end(); ++it) {
        options.parseArgument(args, &it);
    }

    return std::pair<Options, std::string>(options, error);
}

