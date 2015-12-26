/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <pbxbuild/DirectedGraph.h>

using pbxbuild::DirectedGraph;

TEST(DirectedGraph, Nodes)
{
    DirectedGraph<int> graph;
    graph.insert(4, std::unordered_set<int>({ 2, 3, 5 }));
    graph.insert(2, std::unordered_set<int>({ 5, 6, 1 }));
    graph.insert(7, std::unordered_set<int>({ }));

    EXPECT_EQ(graph.nodes(), std::unordered_set<int>({ 1, 2, 3, 4, 5, 6, 7 }));
}

TEST(DirectedGraph, Adjacent)
{
    DirectedGraph<int> graph;
    graph.insert(4, std::unordered_set<int>({ 2, 3, 5 }));
    graph.insert(2, std::unordered_set<int>({ 5, 6, 1 }));
    graph.insert(7, std::unordered_set<int>({ }));

    EXPECT_EQ(graph.adjacent(1), std::unordered_set<int>({ }));
    EXPECT_EQ(graph.adjacent(4), std::unordered_set<int>({ 2, 3, 5 }));
    EXPECT_EQ(graph.adjacent(7), std::unordered_set<int>({ }));
    EXPECT_EQ(graph.adjacent(8), std::unordered_set<int>({ }));
}

TEST(DirectedGraph, Ordered)
{
    DirectedGraph<int> graph;
    graph.insert(4, std::unordered_set<int>({ 2, 3, 5 }));
    graph.insert(2, std::unordered_set<int>({ 5, 1 }));
    graph.insert(5, std::unordered_set<int>({ 1 }));

    EXPECT_EQ(graph.ordered(), std::vector<int>({ 1, 5, 2, 3, 4 }));
}

