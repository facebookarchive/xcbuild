/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <process/MemoryContext.h>

using process::MemoryContext;

MemoryContext::
MemoryContext(
    std::string const &executablePath,
    std::string const &currentDirectory,
    std::vector<std::string> const &commandLineArguments,
    std::unordered_map<std::string, std::string> const &environmentVariables,
    int32_t userID,
    int32_t groupID,
    std::string const &userName,
    std::string const &groupName) :
    Context       (),
    _executablePath      (executablePath),
    _currentDirectory    (currentDirectory),
    _commandLineArguments(commandLineArguments),
    _environmentVariables(environmentVariables),
    _userID              (userID),
    _groupID             (groupID),
    _userName            (userName),
    _groupName           (groupName)
{
}

MemoryContext::
MemoryContext(Context const *context) :
    MemoryContext(
        context->executablePath(),
        context->currentDirectory(),
        context->commandLineArguments(),
        context->environmentVariables(),
        context->userID(),
        context->groupID(),
        context->userName(),
        context->groupName())
{
}

MemoryContext::
~MemoryContext()
{
}

ext::optional<std::string> MemoryContext::
environmentVariable(std::string const &variable) const
{
    auto it = _environmentVariables.find(variable);
    if (it != _environmentVariables.end()) {
        return it->second;
    } else {
        return ext::nullopt;
    }
}

