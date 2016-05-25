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

static ext::optional<std::string>
SpecifiedDeveloperRoot(Filesystem const *filesystem)
{
    if (char *path = getenv("DEVELOPER_DIR")) {
        return std::string(path);
    }

    if (auto path = filesystem->readSymbolicLink("/var/db/xcode_select_link")) {
        return path;
    }

    if (auto path = filesystem->readSymbolicLink("/usr/share/xcode-select/xcode_dir_path")) {
        return path;
    }

    return ext::nullopt;
}

ext::optional<std::string> Environment::
DeveloperRoot(Filesystem const *filesystem)
{
    if (ext::optional<std::string> specified = SpecifiedDeveloperRoot(filesystem)) {
        /*
         * Support finding the developer directory inside an application directory.
         */
        std::string application = *specified + "/Contents/Developer";
        if (filesystem->isDirectory(application)) {
            return application;
        }

        return specified;
    }

    /*
     * Fall back to a set of known directories.
     */
    std::vector<std::string> defaults = {
        "/Applications/Xcode.app/Contents/Developer",
        "/Developer",
    };
    for (std::string const &path : defaults) {
        if (filesystem->isDirectory(path)) {
            return path;
        }
    }

    return ext::nullopt;
}
