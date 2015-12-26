/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_SearchPaths_h
#define __pbxbuild_SearchPaths_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class Context;

class SearchPaths {
private:
    std::vector<std::string> _headerSearchPaths;
    std::vector<std::string> _userHeaderSearchPaths;
    std::vector<std::string> _frameworkSearchPaths;
    std::vector<std::string> _librarySearchPaths;

public:
    SearchPaths();
    ~SearchPaths();

public:
    std::vector<std::string> const &headerSearchPaths(void) const
    { return _headerSearchPaths; }
    std::vector<std::string> const &userHeaderSearchPaths(void) const
    { return _userHeaderSearchPaths; }
    std::vector<std::string> const &frameworkSearchPaths(void) const
    { return _frameworkSearchPaths; }
    std::vector<std::string> const &librarySearchPaths(void) const
    { return _librarySearchPaths; }

public:
    std::vector<std::string> &headerSearchPaths(void)
    { return _headerSearchPaths; }
    std::vector<std::string> &userHeaderSearchPaths(void)
    { return _userHeaderSearchPaths; }
    std::vector<std::string> &frameworkSearchPaths(void)
    { return _frameworkSearchPaths; }
    std::vector<std::string> &librarySearchPaths(void)
    { return _librarySearchPaths; }

public:
    static void
    Resolve(Tool::Context *toolContext, pbxsetting::Environment const &environment);

public:
    static std::vector<std::string>
    ExpandRecursive(pbxsetting::Environment const &environment, std::vector<std::string> const &paths, std::string const &workingDirectory);
};

}
}

#endif // !__pbxbuild_SearchPaths_h
