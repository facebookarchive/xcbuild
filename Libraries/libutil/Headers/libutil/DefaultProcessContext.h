/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __libutil_DefaultProcessContext_h
#define __libutil_DefaultProcessContext_h

#include <libutil/ProcessContext.h>

namespace libutil {

/*
 * A process context for the current process.
 */
class DefaultProcessContext : public ProcessContext {
public:
    explicit DefaultProcessContext();
    virtual ~DefaultProcessContext();

public:
    virtual std::string executablePath() const;
    virtual std::string currentDirectory() const;

public:
    virtual std::vector<std::string> commandLineArguments() const;
    virtual std::unordered_map<std::string, std::string> environmentVariables() const;
    virtual ext::optional<std::string> environmentVariable(std::string const &variable) const;

public:
    virtual int32_t userID() const;
    virtual int32_t groupID() const;
    virtual std::string userName() const;
    virtual std::string groupName() const;
};

}

#endif  // !__libutil_DefaultProcessContext_h
