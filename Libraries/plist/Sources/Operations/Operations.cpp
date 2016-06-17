/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Operations.h>
#include <iostream>

std::pair<bool, std::vector<uint8_t>>
plist::Read(libutil::Filesystem const *filesystem, std::string const &path)
{
    std::vector<uint8_t> contents;

    if (path == "-") {
        /* - means read from stdin. */
        contents = std::vector<uint8_t>(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
    } else {
        /* Read from file. */
        if (!filesystem->read(&contents, path)) {
            return std::make_pair(false, std::vector<uint8_t>());
        }
    }

    return std::make_pair(true, std::move(contents));
}

bool
plist::Write(libutil::Filesystem *filesystem, std::vector<uint8_t> const &contents, std::string const &path)
{
    if (path == "-") {
        /* - means write to stdout. */
        std::copy(contents.begin(), contents.end(), std::ostream_iterator<char>(std::cout));
    } else {
        /* Read from file. */
        if (!filesystem->write(contents, path)) {
            return false;
        }
    }

    return true;
}
