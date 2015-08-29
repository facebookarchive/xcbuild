// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildGraph.h>

using pbxbuild::BuildGraph;

template<class T>
void BuildGraph<T>::
insert(T const &node, std::vector<T> const &children)
{
    auto it = _contents.find(node);
    if (it == _contents.end()) {
        _contents.insert(std::make_pair(node, children));
    } else {
        std::vector<T> &existing = it->second;
        existing.insert(existing.end(), children.begin(), children.end());
    }
}

template<class T>
std::vector<T> BuildGraph<T>::
children(T const &node) const
{
    auto it = _contents.find(node);
    if (it != _contents.end()) {
        return it->second;
    } else {
        return std::vector<T>();
    }
}

template<class T>
std::vector<T> BuildGraph<T>::
ordered(void) const
{
    std::vector<T> result;

    std::list<T> toExplore;
    std::transform(_contents.begin(), _contents.end(), std::back_inserter(toExplore), [](auto const &pair) {
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

        inProgress.insert(node);

        size_t stack = toExplore.size();
        for (T const &child : _contents.find(node)->second) {
            if (inProgress.find(child) != inProgress.end()) {
                fprintf(stderr, "Cycle detected!\n");
                return std::vector<T>();
            }

            if (explored.find(child) == explored.end()) {
                toExplore.push_front(child);
                break;
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
