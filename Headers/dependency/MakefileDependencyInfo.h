/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __dependency_MakefileDependencyInfo_h
#define __dependency_MakefileDependencyInfo_h

#include <dependency/DependencyInfo.h>
#include <dependency/DependencyInfoFormat.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace dependency {

/*
 * The Makefile-based dependency info format used by clang and swiftc.
 */
class MakefileDependencyInfo {
private:
    std::unordered_multimap<std::string, std::string> _outputInputs;

private:
    DependencyInfo                                    _dependencyInfo;

public:
    MakefileDependencyInfo(
        std::unordered_multimap<std::string, std::string> const &outputInputs,
        DependencyInfo const &dependencyInfo);

public:
    /*
     * The map from output to the output's inputs.
     */
    std::unordered_multimap<std::string, std::string> const &outputInputs() const
    { return _outputInputs; }

public:
    /*
     * The encoded dependency info.
     */
    DependencyInfo const &dependencyInfo() const
    { return _dependencyInfo; }

public:
    /*
     * Create the dependency info from the Makefile contents.
     */
    static std::unique_ptr<MakefileDependencyInfo>
    Create(std::string const &contents);

public:
    /*
     * The dependency info format.
     */
    DependencyInfoFormat Format() { return DependencyInfoFormat::Makefile; }
};

}

#endif /* __dependency_MakefileDependencyInfo_h */
