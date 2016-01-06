/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __dependency_BinaryDependencyInfo_h
#define __dependency_BinaryDependencyInfo_h

#include <dependency/DependencyInfo.h>
#include <dependency/DependencyInfoFormat.h>

#include <memory>
#include <string>
#include <vector>

namespace dependency {

/*
 * The binary dependency info format used by actool and ld64.
 */
class BinaryDependencyInfo {
private:
    std::string              _version;
    std::vector<std::string> _missing;

private:
    DependencyInfo           _dependencyInfo;

public:
    BinaryDependencyInfo(
        std::string const &version,
        std::vector<std::string> const &missing,
        DependencyInfo const &dependencyInfo);
    BinaryDependencyInfo(DependencyInfo const &dependencyInfo);

public:
    /*
     * The version string of the creating tool.
     */
    std::string const &version() const
    { return _version; }

    /*
     * Files that were accessed but did not exist.
     */
    std::vector<std::string> const &missing() const
    { return _missing; }

public:
    /*
     * The encoded dependency info.
     */
    DependencyInfo const &dependencyInfo() const
    { return _dependencyInfo; }

public:
    /*
     * Load dependency info from binary data.
     */
    static std::unique_ptr<BinaryDependencyInfo>
    Create(std::vector<uint8_t> const &contents);

public:
    /*
     * The dependency info format.
     */
    DependencyInfoFormat Format() { return DependencyInfoFormat::Binary; }
};

}

#endif /* __dependency_BinaryDependencyInfo_h */
