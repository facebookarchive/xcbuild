/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxbuild/BuildGraph.h>
#include <pbxbuild/Tool/Invocation.h>

using pbxbuild::BuildGraph;

template<class T>
void BuildGraph<T>::
insert(T const &node, std::unordered_set<T> const &adjacent)
{
    _nodes.insert(node);
    _nodes.insert(adjacent.begin(), adjacent.end());

    auto it = _adjacency.find(node);
    if (it == _adjacency.end()) {
        _adjacency.insert(std::make_pair(node, adjacent));
    } else {
        it->second.insert(adjacent.begin(), adjacent.end());
    }
}

template<class T>
std::unordered_set<T> const &BuildGraph<T>::
nodes() const
{
    return _nodes;
}

template<class T>
std::unordered_set<T> BuildGraph<T>::
adjacent(T const &node) const
{
    auto it = _adjacency.find(node);
    if (it != _adjacency.end()) {
        return it->second;
    } else {
        return std::unordered_set<T>();
    }
}

template<class T>
std::vector<T> BuildGraph<T>::
ordered(void) const
{
    std::vector<T> result;

    std::list<T> toExplore;
    std::transform(_adjacency.begin(), _adjacency.end(), std::back_inserter(toExplore), [](std::pair<T, std::unordered_set<T>> const &pair) {
        return pair.first;
    });

    std::unordered_set<T> inProgress;
    std::unordered_set<T> explored;

    while (!toExplore.empty()) {
        T node = toExplore.front();
        if (explored.find(node) != explored.end()) {
            toExplore.pop_front();
            continue;
        }

        size_t stack = toExplore.size();
        inProgress.insert(node);

        auto it = _adjacency.find(node);
        if (it != _adjacency.end()) {
            for (T const &child : it->second) {
                if (inProgress.find(child) != inProgress.end()) {
                    fprintf(stderr, "error: dependency cycle detected!\n");
                    return std::vector<T>();
                }

                if (explored.find(child) == explored.end()) {
                    toExplore.push_front(child);
                    break;
                }
            }
        }

        if (stack == toExplore.size()) {
            toExplore.pop_front();
            inProgress.erase(node);
            explored.insert(node);
            result.push_back(node);
        }
    }

    assert(inProgress.empty());
    return result;
}

namespace pbxbuild { template class BuildGraph<pbxproj::PBX::Target::shared_ptr>; }
namespace pbxbuild { template class BuildGraph<pbxproj::PBX::BuildPhase::shared_ptr>; }
namespace pbxbuild { template class BuildGraph<pbxspec::PBX::FileType::shared_ptr>; }
namespace pbxbuild { template class BuildGraph<pbxbuild::Tool::Invocation const *>; }
namespace pbxbuild { template class BuildGraph<int>; } /* For testing. */
