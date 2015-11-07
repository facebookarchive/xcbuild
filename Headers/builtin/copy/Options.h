/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __builtin_copy_Options_h
#define __builtin_copy_Options_h

#include <builtin/Base.h>

namespace builtin {
namespace copy {

class Options {
public:
    enum class BitcodeStripMode {
        None,
        ReplaceWithMarker,
        All,
    };

private:
    bool                     _verbose;
    bool                     _preserveHFSData;

private:
    std::vector<std::string> _inputs;
    bool                     _ignoreMissingInputs;
    bool                     _resolveSrcSymlinks;
    std::string              _output;
    std::vector<std::string> _excludes;

private:
    bool                     _stripDebugSymbols;
    std::string              _stripTool;
    BitcodeStripMode         _bitcodeStrip;
    std::string              _bitcodeStripTool;

public:
    Options();
    ~Options();

public:
    bool verbose() const
    { return _verbose; }
    bool preserveHFSData() const
    { return _preserveHFSData; }

public:
    std::vector<std::string> const &inputs() const
    { return _inputs; }
    bool ignoreMissingInputs() const
    { return _ignoreMissingInputs; }
    bool resolveSrcSymlinks() const
    { return _resolveSrcSymlinks; }
    std::string const &output() const
    { return _output; }
    std::vector<std::string> const &excludes() const
    { return _excludes; }

public:
    bool stripDebugSymbols() const
    { return _stripDebugSymbols; }
    std::string const &stripTool() const
    { return _stripTool; }
    BitcodeStripMode bitcodeStrip() const
    { return _bitcodeStrip; }
    std::string const &bitcodeStripTool() const
    { return _bitcodeStripTool; }

private:
    friend class libutil::Options;
    std::pair<bool, std::string>
    parseArgument(std::vector<std::string> const &args, std::vector<std::string>::const_iterator *it);
};

}
}

#endif // !__builtin_copy_Options_h
