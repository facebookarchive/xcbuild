// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_SearchPaths_h
#define __pbxbuild_SearchPaths_h

#include <pbxbuild/Base.h>

namespace pbxbuild {
namespace Tool {

class SearchPaths {
private:
    std::vector<std::string> _headerSearchPaths;
    std::vector<std::string> _userHeaderSearchPaths;
    std::vector<std::string> _frameworkSearchPaths;
    std::vector<std::string> _librarySearchPaths;

public:
    SearchPaths(std::vector<std::string> const &headerSearchPaths, std::vector<std::string> const &userHeaderSearchPaths, std::vector<std::string> const &frameworkSearchPaths, std::vector<std::string> const &librarySearchPaths);
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
    static SearchPaths
    Create(std::string const &workingDirectory, pbxsetting::Environment const &environment);

public:
    static std::vector<std::string>
    ExpandRecursive(pbxsetting::Environment const &environment, std::vector<std::string> const &paths, std::string const &workingDirectory);
};

}
}

#endif // !__pbxbuild_SearchPaths_h
