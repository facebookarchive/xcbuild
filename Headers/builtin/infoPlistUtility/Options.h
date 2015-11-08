/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __builtin_infoPlistUtility_Options_h
#define __builtin_infoPlistUtility_Options_h

#include <builtin/Base.h>

namespace builtin {
namespace infoPlistUtility {

class Options {
private:
    std::string              _input;
    std::vector<std::string> _additionalContentFiles;
    std::string              _output;

private:
    std::string              _format;
    bool                     _expandBuildSettings;

private:
    std::string              _platform;
    std::vector<std::string> _requiredArchitectures;

private:
    std::string              _genPkgInfo;
    std::string              _resourceRulesFile;

private:
    std::string              _infoFileKeys;
    std::string              _infoFileValues;

public:
    Options();
    ~Options();

public:
    std::string const &input() const
    { return _input; }
    std::vector<std::string> const &additionalContentFiles() const
    { return _additionalContentFiles; }
    std::string const &output() const
    { return _output; }

public:
    std::string const &format() const
    { return _format; }
    bool expandBuildSettings() const
    { return _expandBuildSettings; }

public:
    std::string const &platform() const
    { return _platform; }
    std::vector<std::string> const &requiredArchitectures() const
    { return _requiredArchitectures; }

public:
    std::string const &genPkgInfo() const
    { return _genPkgInfo; }
    std::string const &resourceRulesFile() const
    { return _resourceRulesFile; }

public:
    std::string const &infoFileKeys() const
    { return _infoFileKeys; }
    std::string const &infoFileValues() const
    { return _infoFileValues; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

}
}

#endif // !__builtin_infoPlistUtility_Options_h
