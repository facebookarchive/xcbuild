/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <plist/Operations/Adjustment.h>

plist::Adjustment::
Adjustment(Type type, std::string const &path, std::unique_ptr<plist::Object const> value) :
    _type (type),
    _path (path),
    _value(std::move(value))
{
}
