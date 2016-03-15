/* Copyright 2013-present Facebook. All Rights Reserved. */

#ifndef __acdriver_Options_h
#define __acdriver_Options_h

#include <libutil/Options.h>

#include <string>
#include <vector>

namespace acdriver {

/*
 * Options for actool.
 */
class Options {
private:
    bool _version;

private:
    std::string _outputFormat;
    bool        _warnings;
    bool        _errors;
    bool        _notices;
    bool        _printContents;

private:
    std::string _exportDependencyInfo;

private:
    std::string _compile;
    std::string _input;

private:
    std::string _optimization;
    bool        _compressPNGs;

private:
    std::string _platform;
    std::string _minimumDeploymentTarget;
    std::string _targetDevice;

private:
    std::string _outputPartialInfoPlist;
    std::string _appIcon;
    std::string _launchImage;

private:
    bool        _enableOnDemandResources;
    bool        _enableIncrementalDistill;
    std::string _targetName;
    std::string _filterForDeviceModel;
    std::string _filterForDeviceOsVersion;

public:
    Options();
    ~Options();

public:
    bool version() const
    { return _version; }

public:
    std::string const &outputFormat() const
    { return _outputFormat; }
    bool warnings() const
    { return _warnings; }
    bool errors() const
    { return _errors; }
    bool notices() const
    { return _notices; }
    bool printContents() const
    { return _printContents; }

public:
    std::string const &exportDependencyInfo() const
    { return _exportDependencyInfo; }

public:
    std::string const &compile() const
    { return _compile; }
    std::string const &input() const
    { return _input; }

public:
    std::string const &optimization() const
    { return _optimization; }
    bool compressPNGs() const
    { return _compressPNGs; }

public:
    std::string const &platform() const
    { return _platform; }
    std::string const &minimumDeploymentTarget() const
    { return _minimumDeploymentTarget; }
    std::string const &targetDevice() const
    { return _targetDevice; }

public:
    std::string const &outputPartialInfoPlist() const
    { return _outputPartialInfoPlist; }
    std::string const &appIcon() const
    { return _appIcon; }
    std::string const &launchImage() const
    { return _launchImage; }

public:
    bool enableOnDemandResources() const
    { return _enableOnDemandResources; }
    bool enableIncrementalDistill() const
    { return _enableIncrementalDistill; }
    std::string const &targetName() const
    { return _targetName; }
    std::string const &filterForDeviceModel() const
    { return _filterForDeviceModel; }
    std::string const &filterForDeviceOsVersion() const
    { return _filterForDeviceOsVersion; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

}

#endif // !__acdriver_Options_h
