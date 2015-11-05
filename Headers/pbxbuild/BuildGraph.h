/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

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
