// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxbuild/BuildGraph.h>

using pbxbuild::BuildGraph;
using pbxproj::PBX::Target;

void BuildGraph::
insert(Target::shared_ptr node, std::vector<Target::shared_ptr> const &children)
{
    auto it = _contents.find(node);
    if (it == _contents.end()) {
        _contents.insert(std::make_pair(node, children));
    } else {
        std::vector<Target::shared_ptr> &existing = it->second;
        existing.insert(existing.end(), children.begin(), children.end());
    }
}

std::vector<Target::shared_ptr> BuildGraph::
children(Target::shared_ptr node) const
{
    auto it = _contents.find(node);
    if (it != _contents.end()) {
        return it->second;
    } else {
        return std::vector<Target::shared_ptr>();
    }
}

std::vector<Target::shared_ptr> BuildGraph::
ordered(void) const
{
    std::vector<Target::shared_ptr> result;

    std::list<Target::shared_ptr> toExplore;
    std::transform(_contents.begin(), _contents.end(), std::back_inserter(toExplore), [](auto const &pair) {
        return pair.first;
    });

    std::unordered_set<Target::shared_ptr> inProgress;
    std::unordered_set<Target::shared_ptr> explored;

    while (!toExplore.empty()) {
        Target::shared_ptr node = toExplore.front();
        if (explored.find(node) != explored.end()) {
            toExplore.pop_front();
            continue;
        }

        inProgress.insert(node);

        size_t stack = toExplore.size();
        for (Target::shared_ptr const &child : _contents.find(node)->second) {
            if (inProgress.find(child) != inProgress.end()) {
                fprintf(stderr, "Cycle detected!\n");
                return std::vector<Target::shared_ptr>();
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
