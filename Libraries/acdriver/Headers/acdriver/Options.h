/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
    bool        _version;
    bool        _printContents;
    std::vector<std::string> _inputs;

private:
    std::string _outputFormat;
    bool        _warnings;
    bool        _errors;
    bool        _notices;

private:
    std::string _compile;
    std::string _exportDependencyInfo;

private:
    std::string _optimization;
    bool        _compressPNGs;

private:
    std::string _platform;
    std::string _minimumDeploymentTarget;
    std::vector<std::string> _targetDevice;

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
    bool printContents() const
    { return _printContents; }
    std::string const &compile() const
    { return _compile; }

public:
    std::string const &outputFormat() const
    { return _outputFormat; }
    bool warnings() const
    { return _warnings; }
    bool errors() const
    { return _errors; }
    bool notices() const
    { return _notices; }

public:
    std::string const &exportDependencyInfo() const
    { return _exportDependencyInfo; }
    std::vector<std::string> const &inputs() const
    { return _inputs; }

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
    std::vector<std::string> const &targetDevice() const
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
