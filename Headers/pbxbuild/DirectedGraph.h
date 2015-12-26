/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_DirectedGraph_h
#define __pbxbuild_DirectedGraph_h

#include <pbxbuild/Base.h>
#include <list>

namespace pbxbuild {

template<typename T>
class DirectedGraph {
private:
    std::unordered_set<T>                        _nodes;
    std::unordered_map<T, std::unordered_set<T>> _adjacency;

public:
    void insert(T const &node, std::unordered_set<T> const &adjacent);

public:
    std::unordered_set<T> const &nodes() const;
    std::unordered_set<T> adjacent(T const &node) const;

public:
    std::pair<bool, std::vector<T>> ordered(void) const;
};

}

#endif // !__pbxbuild_DirectedGraph_h
