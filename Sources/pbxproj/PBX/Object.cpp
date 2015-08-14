// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/PBX/Object.h>

using pbxproj::PBX::Object;

Object::Object(std::string const &isa) :
    _isa(isa)
{
}
