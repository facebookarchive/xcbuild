/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/ShellScriptBuildPhase.h>

using pbxproj::PBX::ShellScriptBuildPhase;

ShellScriptBuildPhase::ShellScriptBuildPhase() :
    BuildPhase       (Isa(), kTypeShellScript),
    _showEnvVarsInLog(true)
{
}

bool ShellScriptBuildPhase::
parse(Context &context, plist::Dictionary const *dict)
{
    if (!BuildPhase::parse(context, dict))
        return false;

    auto N  = dict->value <plist::String> ("name");
    auto SP = dict->value <plist::String> ("shellPath");
    auto SS = dict->value <plist::String> ("shellScript");
    auto IP = dict->value <plist::Array> ("inputPaths");
    auto OP = dict->value <plist::Array> ("outputPaths");
    auto SE = dict->value <plist::String> ("showEnvVarsInLog");

    if (N != nullptr) {
        _name = N->value();
    }

    if (SP != nullptr) {
        _shellPath = SP->value();
    }

    if (SS != nullptr) {
        _shellScript = SS->value();
    }

    if (IP != nullptr) {
        for (size_t n = 0; n < IP->count(); n++) {
            auto P = IP->value <plist::String> (n);
            if (P != nullptr) {
                pbxsetting::Value V = pbxsetting::Value::Parse(P->value());
                _inputPaths.push_back(V);
            }
        }
    }

    if (OP != nullptr) {
        for (size_t n = 0; n < OP->count(); n++) {
            auto P = OP->value <plist::String> (n);
            if (P != nullptr) {
                pbxsetting::Value V = pbxsetting::Value::Parse(P->value());
                _outputPaths.push_back(V);
            }
        }
    }

    if (SE != nullptr) {
        _showEnvVarsInLog = pbxsetting::Type::ParseInteger(SE->value());
    }

    return true;
}
