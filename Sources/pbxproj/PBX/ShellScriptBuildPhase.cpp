// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/ShellScriptBuildPhase.h>

using pbxproj::PBX::ShellScriptBuildPhase;

ShellScriptBuildPhase::ShellScriptBuildPhase() :
    _showEnvVarsInLog(true),
    BuildPhase       (Isa(), kTypeShellScript)
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
    auto SE = dict->value <plist::Boolean> ("showEnvVarsInLog");

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
                _inputPaths.push_back(P->value());
            }
        }
    }

    if (OP != nullptr) {
        for (size_t n = 0; n < OP->count(); n++) {
            auto P = OP->value <plist::String> (n);
            if (P != nullptr) {
                _outputPaths.push_back(P->value());
            }
        }
    }

    if (SE != nullptr) {
        _showEnvVarsInLog = SE->value();
    }

    return true;
}
