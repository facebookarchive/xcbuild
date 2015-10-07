// Copyright 2013-present Facebook. All Rights Reserved.

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
    static std::pair<Options, std::string>
    Parse(std::vector<std::string> const &arguments);

private:
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

}

#endif // !__xcdriver_Options_h
