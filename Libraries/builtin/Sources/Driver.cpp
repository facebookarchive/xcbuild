/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/Driver.h>
#include <libutil/DefaultFilesystem.h>
#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <libutil/SysUtil.h>

using builtin::Driver;
using libutil::DefaultFilesystem;
using libutil::Filesystem;
using libutil::FSUtil;
using libutil::SysUtil;

Driver::
Driver()
{
}

Driver::
~Driver()
{
}

int Driver::
runc(int argc, char **argv, char **envp)
{
    DefaultFilesystem filesystem = DefaultFilesystem();
    std::vector<std::string> args = std::vector<std::string>(argv + 1, argv + argc);

    std::unordered_map<std::string, std::string> environment;
    for (char **env = envp; *env != NULL; env++) {
        std::string value = *env;
        std::string::size_type offset = value.find('=');

        if (offset != std::string::npos) {
            environment.insert({ value.substr(0, offset), value.substr(offset + 1) });
        } else {
            environment.insert({ value, std::string() });
        }
    }

    return run(args, environment, &filesystem, SysUtil::GetDefault()->currentDirectory());
}
