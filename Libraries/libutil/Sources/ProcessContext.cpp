/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <libutil/ProcessContext.h>

#include <sstream>
#include <unordered_set>

using libutil::ProcessContext;

ProcessContext::
ProcessContext()
{
}

ProcessContext::
~ProcessContext()
{
}

std::vector<std::string> ProcessContext::
executableSearchPaths() const
{
    std::vector<std::string> paths;

    if (ext::optional<std::string> value = environmentVariable("PATH")) {
        std::unordered_set<std::string> seen;

        std::string path;
        std::istringstream is(*value);
        while (std::getline(is, path, ':')) {
            if (seen.find(path) != seen.end()) {
                continue;
            }

            paths.push_back(path);
            seen.insert(path);
        }
    }

    return paths;
}

#include <libutil/DefaultProcessContext.h>

using libutil::DefaultProcessContext;

ProcessContext const *ProcessContext::
GetDefault()
{
    static DefaultProcessContext *defaultProcessContext = nullptr;
    if (defaultProcessContext == nullptr) {
        defaultProcessContext = new DefaultProcessContext();
    }

    return defaultProcessContext;
}

