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

std::pair<bool, std::string> Options::
parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it)
{
    std::string const &arg = **it;

    if (arg == "-usage") {
        return libutil::Options::MarkBool(&_usage, arg);
    } else if (arg == "-help") {
        return libutil::Options::MarkBool(&_help, arg);
    } else if (arg == "-verbose") {
        return libutil::Options::MarkBool(&_verbose, arg);
    } else if (arg == "-license") {
        return libutil::Options::MarkBool(&_license, arg);
    } else if (arg == "-checkFirstLaunchStatus") {
        return libutil::Options::MarkBool(&_checkFirstLaunchStatus, arg);
    } else if (arg == "-project") {
        return libutil::Options::NextString(&_project, args, it);
    } else if (arg == "-target") {
        return libutil::Options::NextString(&_target, args, it);
    } else if (arg == "-alltargets") {
        return libutil::Options::MarkBool(&_allTargets, arg);
    } else if (arg == "-workspace") {
        return libutil::Options::NextString(&_workspace, args, it);
    } else if (arg == "-scheme") {
        return libutil::Options::NextString(&_scheme, args, it);
    } else if (arg == "-configuration") {
        return libutil::Options::NextString(&_configuration, args, it);
    } else if (arg == "-xcconfig") {
        return libutil::Options::NextString(&_xcconfig, args, it);
    } else if (arg == "-arch") {
        return libutil::Options::NextString(&_arch, args, it);
    } else if (arg == "-sdk") {
        return libutil::Options::NextString(&_sdk, args, it);
    } else if (arg == "-toolchain") {
        return libutil::Options::NextString(&_toolchain, args, it);
    } else if (arg == "-destination") {
        return libutil::Options::NextString(&_destination, args, it);
    } else if (arg == "-destinationtimeout") {
        return libutil::Options::NextString(&_destinationTimeout, args, it);
    } else if (arg == "-parallelizeTargets") {
        return libutil::Options::MarkBool(&_parallelizeTargets, arg);
    } else if (arg == "-jobs") {
        return libutil::Options::NextInt(&_jobs, args, it);
    } else if (arg == "-dryrun" || arg == "-n") {
        return libutil::Options::MarkBool(&_dryRun, arg);
    } else if (arg == "-hideShellScriptEnvironment") {
        return libutil::Options::MarkBool(&_hideShellScriptEnvironment, arg);
    } else if (arg == "-showsdks") {
        return libutil::Options::MarkBool(&_showSDKs, arg);
    } else if (arg == "-showBuildSettings") {
        return libutil::Options::MarkBool(&_showBuildSettings, arg);
    } else if (arg == "-list") {
        return libutil::Options::MarkBool(&_list, arg);
    } else if (arg == "-find" || arg == "-find-executable") {
        return libutil::Options::NextString(&_findExecutable, args, it);
    } else if (arg == "-find-library") {
        return libutil::Options::NextString(&_findLibrary, args, it);
    } else if (arg == "-version") {
        return libutil::Options::MarkBool(&_version, arg);
    } else if (arg == "-enableAddressSanitizer") {
        return libutil::Options::NextBool(&_enableAddressSanitizer, args, it);
    } else if (arg == "-resultBundlePath") {
        return libutil::Options::NextString(&_resultBundlePath, args, it);
    } else if (arg == "-derivedDataPath") {
        return libutil::Options::NextString(&_derivedDataPath, args, it);
    } else if (arg == "-archivePath") {
        return libutil::Options::NextString(&_archivePath, args, it);
    } else if (arg == "-exportArchive") {
        return libutil::Options::MarkBool(&_exportArchive, arg);
    } else if (arg == "-exportOptionsPlist") {
        return libutil::Options::NextString(&_exportOptionsPlist, args, it);
    } else if (arg == "-enableCodeCoverage") {
        return libutil::Options::NextBool(&_enableCodeCoverage, args, it);
    } else if (arg == "-exportPath") {
        return libutil::Options::NextString(&_exportPath, args, it);
    } else if (arg == "-skipUnavailableActions") {
        return libutil::Options::MarkBool(&_skipUnavailableActions, arg);
    } else if (arg == "-exportLocalizations") {
        return libutil::Options::MarkBool(&_exportLocalizations, arg);
    } else if (arg == "-importLocalizations") {
        return libutil::Options::MarkBool(&_importLocalizations, arg);
    } else if (arg == "-localizationPath") {
        return libutil::Options::NextString(&_localizationPath, args, it);
    } else if (arg == "-exportLanguage") {
        return libutil::Options::NextString(&_exportLanguage, args, it);
    } else if (arg == "-forceImport") {
        return libutil::Options::MarkBool(&_forceImport, arg);
    } else if (arg == "-executor") {
        return libutil::Options::NextString(&_executor, args, it);
    } else if (arg == "-formatter") {
        return libutil::Options::NextString(&_formatter, args, it);
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

