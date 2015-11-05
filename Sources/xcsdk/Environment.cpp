/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcsdk/Environment.h>
#include <sstream>

using xcsdk::Environment;
using libutil::FSUtil;
using libutil::Subprocess;

std::string Environment::
DeveloperRoot(void)
{
    if (getenv("DEVELOPER_DIR")) {
        return getenv("DEVELOPER_DIR");
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
    if (FSUtil::TestForDirectory(root)) {
        return root;
    }

    return std::string();
}
