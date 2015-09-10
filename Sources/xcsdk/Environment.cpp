// Copyright 2013-present Facebook. All Rights Reserved.

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
