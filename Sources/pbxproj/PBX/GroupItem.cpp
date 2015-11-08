/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <pbxproj/PBX/GroupItem.h>

using pbxproj::PBX::GroupItem;
using pbxsetting::Value;

GroupItem::GroupItem(std::string const &isa, Type type) :
    Object (isa),
    _type  (type),
    _parent(nullptr)
{
}

Value GroupItem::
resolve(void) const
{
    std::string path = _path;
    if (_type != kTypeGroup && _type != kTypeVariantGroup) {
        path = path.empty() ? _name : path;
    }
    path = path.empty() ? path : "/" + path;

    if (_sourceTree.empty() || _sourceTree == "<absolute>") {
        return Value::String(path);
    } else if (_sourceTree == "<group>") {
        if (_parent != nullptr) {
            return _parent->resolve() + Value::String(path);
        } else {
            return Value::Variable("SOURCE_ROOT") + Value::String(path);
        }
    } else {
        return Value::Variable(_sourceTree) + Value::String(path);
    }
}

bool GroupItem::
parse(Context &context, plist::Dictionary const *dict)
{
    auto N  = dict->value <plist::String> ("name");
    auto ST = dict->value <plist::String> ("sourceTree");
    auto P  = dict->value <plist::String> ("path");

    if (N != nullptr) {
        _name = N->value();
    }

    if (ST != nullptr) {
        _sourceTree = ST->value();
    }

    if (P != nullptr) {
        _path = P->value();
    }

    return true;
}
