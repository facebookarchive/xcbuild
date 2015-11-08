/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __builtin_copyPlist_Options_h
#define __builtin_copyPlist_Options_h

#include <builtin/Base.h>

namespace builtin {
namespace copyPlist {

class Options {
private:
    std::vector<std::string> _inputs;
    std::string              _outputDirectory;

public:
    bool                     _validate;

public:
    std::string              _convertFormat;

public:
    Options();
    ~Options();

public:
    std::vector<std::string> const &inputs() const
    { return _inputs; }
    std::string const &outputDirectory() const
    { return _outputDirectory; }

public:
    bool validate() const
    { return _validate; }

public:
    std::string const &convertFormat() const
    { return _convertFormat; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

}
}

#endif // !__builtin_copyPlist_Options_h
