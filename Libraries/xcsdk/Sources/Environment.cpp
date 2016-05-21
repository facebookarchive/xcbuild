/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/Environment.h>
#include <libutil/Base.h>
#include <libutil/Filesystem.h>
#include <libutil/Subprocess.h>

#include <sstream>

using xcsdk::Environment;
using libutil::Filesystem;
using libutil::Subprocess;

ext::optional<std::string> Environment::
DeveloperRoot(Filesystem const *filesystem)
{
    if (char *path = getenv("DEVELOPER_DIR")) {
        return std::string(path);
    }

#if defined(__APPLE__)
    std::ostringstream oss;
    Subprocess subprocess;
    if (subprocess.execute("/usr/bin/xcode-select", { "--print-path" }, nullptr, &oss, nullptr)) {
        std::string output = oss.str();
        libutil::trim(output);
        if (!output.empty()) {
            return output;
        }
    }
#endif

    std::string root = "/Developer";
    if (filesystem->isDirectory(root)) {
        return root;
    }

    return ext::nullopt;
}
