// Copyright 2013-present Facebook. All Rights Reserved.

#ifndef __pbxbuild_BuildGraph_h
#define __pbxbuild_BuildGraph_h

#include <pbxbuild/Base.h>
#include <list>

namespace pbxbuild {

template<typename T>
class BuildGraph {
private:
    std::unordered_map<T, std::vector<T>> _contents;

public:
    void
    insert(T const &node, std::vector<T> const &children);

public:
    std::vector<T>
    children(T const &node) const;

public:
    std::vector<T>
    ordered(void) const;
};

}

#endif // !__pbxbuild_BuildGraph_h
