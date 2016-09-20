/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_ProcessContext_h
#define __libutil_ProcessContext_h

#include <string>
#include <vector>
#include <unordered_map>
#include <ext/optional>

namespace libutil {

class ProcessContext {
protected:
    ProcessContext();
    virtual ~ProcessContext();

public:
    /*
     * The path to the running executable.
     */
    virtual std::string executablePath() const = 0;

    /*
     * The current directory of the process.
     */
    virtual std::string currentDirectory() const = 0;

public:
    /*
     * Arguments to the process.
     */
    virtual std::vector<std::string> commandLineArguments() const = 0;

    /*
     * All environment variables.
     */
    virtual std::unordered_map<std::string, std::string> environmentVariables() const = 0;

    /*
     * Single environment variable.
     */
    virtual ext::optional<std::string> environmentVariable(std::string const &variable) const = 0;

public:
    /*
     * Active user ID.
     */
    virtual int32_t userID() const = 0;

    /*
     * Active group ID.
     */
    virtual int32_t groupID() const = 0;

    /*
     * Active user name.
     */
    virtual std::string userName() const = 0;

    /*
     * Active group name.
     */
    virtual std::string groupName() const = 0;

public:
    /*
     * The default environment search paths.
     */
    std::vector<std::string> executableSearchPaths() const;

    /*
     * The home directory from the environment.
     */
    std::string userHomeDirectory() const;

public:
    /*
     * Get the system instance.
     */
    static ProcessContext const *GetDefaultUNSAFE();
};

}

#endif  // !__libutil_ProcessContext_h
