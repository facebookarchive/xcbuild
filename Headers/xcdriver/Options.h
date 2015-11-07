/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __xcdriver_Options_h
#define __xcdriver_Options_h

#include <xcdriver/Base.h>

namespace xcdriver {

class Options {
private:
    bool        _usage;
    bool        _help;
    bool        _verbose;
    bool        _license;
    bool        _checkFirstLaunchStatus;
    bool        _version;

private:
    std::string _project;
    std::string _target;
    bool        _allTargets;
    std::string _workspace;
    std::string _scheme;
    std::string _configuration;
    std::string _arch;
    std::string _sdk;
    std::string _toolchain;
    std::string _destination;
    std::string _destinationTimeout;

private:
    bool        _parallelizeTargets;
    int         _jobs;
    bool        _dryRun;
    bool        _hideShellScriptEnvironment;

private:
    bool        _list;
    bool        _showSDKs;
    bool        _showBuildSettings;

private:
    std::string _xcconfig;
    std::vector<pbxsetting::Setting> _settings;

private:
    std::string _findExecutable;
    std::string _findLibrary;

private:
    bool        _enableAddressSanitizer;
    bool        _enableCodeCoverage;

private:
    std::string _resultBundlePath;
    std::string _derivedDataPath;

private:
    bool        _exportArchive;
    std::string _archivePath;
    std::string _exportPath;
    std::string _exportOptionsPlist;

private:
    bool        _skipUnavailableActions;
    std::vector<std::string> _actions;

private:
    bool        _exportLocalizations;
    bool        _importLocalizations;
    std::string _localizationPath;
    std::string _exportLanguage;
    bool        _forceImport;

public:
    Options();
    ~Options();

public:
    bool usage() const
    { return _usage; }
    bool help() const
    { return _help; }
    bool verbose() const
    { return _verbose; }
    bool license() const
    { return _license; }
    bool checkFirstLaunchStatus() const
    { return _checkFirstLaunchStatus; }
    bool version() const
    { return _version; }

public:
    std::string const &project() const
    { return _project; }
    std::string const &target() const
    { return _target; }
    bool allTargets() const
    { return _allTargets; }
    std::string const &workspace() const
    { return _workspace; }
    std::string const &scheme() const
    { return _scheme; }
    std::string const &configuration() const
    { return _configuration; }
    std::string const &arch() const
    { return _arch; }
    std::string const &sdk() const
    { return _sdk; }
    std::string const &toolchain() const
    { return _toolchain; }
    std::string const &destination() const
    { return _destination; }
    std::string const &destinationTimeout() const
    { return _destinationTimeout; }

public:
    bool parallelizeTargets() const
    { return _parallelizeTargets; }
    int jobs() const
    { return _jobs; }
    bool dryRun() const
    { return _dryRun; }
    bool hideShellScriptEnvironment() const
    { return _hideShellScriptEnvironment; }

public:
    bool list() const
    { return _list; }
    bool showSDKs() const
    { return _showSDKs; }
    bool showBuildSettings() const
    { return _showBuildSettings; }

public:
    std::string const &xcconfig() const
    { return _xcconfig; }
    std::vector<pbxsetting::Setting> const &settings() const
    { return _settings; }

public:
    std::string const &findExecutable() const
    { return _findExecutable; }
    std::string const &findLibrary() const
    { return _findLibrary; }

public:
    bool enableAddressSanitizer() const
    { return _enableAddressSanitizer; }
    bool enableCodeCoverage() const
    { return _enableCodeCoverage; }

public:
    std::string const &resultBundlePath() const
    { return _resultBundlePath; }
    std::string const &derivedDataPath() const
    { return _derivedDataPath; }

public:
    bool exportArchive() const
    { return _exportArchive; }
    std::string const &archivePath() const
    { return _archivePath; }
    std::string const &exportPath() const
    { return _exportPath; }
    std::string const &exportOptionsPlist() const
    { return _exportOptionsPlist; }

public:
    bool skipUnavailableActions() const
    { return _skipUnavailableActions; }
    std::vector<std::string> const &actions() const
    { return _actions; }

public:
    bool exportLocalizations() const
    { return _exportLocalizations; }
    bool importLocalizations() const
    { return _importLocalizations; }
    std::string const &localizationPath() const
    { return _localizationPath; }
    std::string const &exportLanguage() const
    { return _exportLanguage; }
    bool forceImport() const
    { return _forceImport; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

}

#endif // !__xcdriver_Options_h
