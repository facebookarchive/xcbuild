/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <dependency/MakefileDependencyInfo.h>
#include <dependency/DependencyInfo.h>

#include <cassert>

using dependency::MakefileDependencyInfo;
using dependency::DependencyInfo;

MakefileDependencyInfo::
MakefileDependencyInfo(std::unordered_multimap<std::string, std::string> const &outputInputs, DependencyInfo const &dependencyInfo) :
    _outputInputs  (outputInputs),
    _dependencyInfo(dependencyInfo)
{
}

ext::optional<MakefileDependencyInfo> MakefileDependencyInfo::
Create(std::string const &contents)
{
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::unordered_multimap<std::string, std::string> outputInputs;

    enum class State {
        Begin,
        Comment,
        Output,
        Inputs,
    };

    State state = State::Begin;

    std::string current;
    std::string currentOutput;
    std::vector<std::string> currentInputs;

    for (auto it = contents.begin(), prev = contents.end(); it != contents.end(); prev = it, ++it) {
        bool escaped = (prev != contents.end() && *prev == '\\');

        if (!escaped && *it == '#') {
            /* Begin comment. */
            state = State::Comment;
        } else if (!escaped && *it == '\n') {
            switch (state) {
                case State::Begin:
                    break;
                case State::Output:
                    /* Output without inputs. */
                    return ext::nullopt;
                case State::Comment:
                case State::Inputs:
                    /* Current input. */
                    if (!current.empty()) {
                        currentInputs.push_back(current);
                        current = std::string();
                    }

                    /* Store this output and inputs. */
                    if (!currentOutput.empty()) {
                        outputs.push_back(currentOutput);
                        inputs.insert(inputs.end(), currentInputs.begin(), currentInputs.end());
                        for (std::string const &input : currentInputs) {
                            outputInputs.insert({ currentOutput, input });
                        }

                        /* Reset for next entry. */
                        currentOutput = std::string();
                        currentInputs = std::vector<std::string>();
                    }

                    state = State::Begin;
                    break;
            }
        } else if ((!escaped && isspace(*it)) || (escaped && *it == '\n')) {
            switch (state) {
                case State::Begin:
                case State::Comment:
                case State::Output:
                    break;
                case State::Inputs:
                    /* Remove escape character. */
                    if (!current.empty() && escaped) {
                        current.resize(current.size() - 1);
                    }

                    /* Next input. */
                    if (!current.empty()) {
                        currentInputs.push_back(current);
                        current = std::string();
                    }
                    break;
            }
        } else if (!escaped && *it == ':') {
            switch (state) {
                case State::Begin:
                    /* Invalid character. */
                    return ext::nullopt;
                case State::Comment:
                    break;
                case State::Output:
                    assert(!current.empty());

                    /* Wait for inputs. */
                    currentOutput = current;
                    current = std::string();
                    state = State::Inputs;
                    break;
                case State::Inputs:
                    /* Invalid character. */
                    return ext::nullopt;
            }
        } else if (*it == '#' || *it == '%' || (escaped && isspace(*it))) {
            switch (state) {
                case State::Begin:
                    /* Invalid character. */
                    return ext::nullopt;
                case State::Comment:
                    break;
                case State::Output:
                case State::Inputs:
                    if (escaped) {
                        /* Unescape; replace backslash. */
                        current[current.size() - 1] = *it;
                        break;
                    } else {
                        /* Invalid character. */
                        return ext::nullopt;
                    }
            }
        } else {
            switch (state) {
                case State::Begin:
                    /* Start of output. */
                    state = State::Output;

                    /* Add character. */
                    current += *it;
                    break;
                case State::Comment:
                    break;
                case State::Output:
                case State::Inputs:
                    /* Add character. */
                    current += *it;
                    break;
            }
        }
    }

    switch (state) {
        case State::Begin:
            break;
        case State::Output:
            /* Output without inputs. */
            return ext::nullopt;
        case State::Comment:
        case State::Inputs:
            /* Current input. */
            if (!current.empty()) {
                currentInputs.push_back(current);
            }

            /* Store this output and inputs. */
            if (!currentOutput.empty()) {
                outputs.push_back(currentOutput);
                inputs.insert(inputs.end(), currentInputs.begin(), currentInputs.end());
                for (std::string const &input : currentInputs) {
                    outputInputs.insert({ currentOutput, input });
                }
            }
            break;
    }

    /* Create dependency info. */
    auto info = DependencyInfo(inputs, outputs);
    auto binaryInfo = MakefileDependencyInfo(outputInputs, info);

    return MakefileDependencyInfo(binaryInfo);
}
