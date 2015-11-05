/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __pbxbuild_Base_h
#define __pbxbuild_Base_h

#include <xcsdk/xcsdk.h>
#include <pbxproj/pbxproj.h>
#include <pbxspec/pbxspec.h>
#include <xcscheme/xcscheme.h>
#include <xcworkspace/xcworkspace.h>

namespace pbxbuild {

using string_vector = libutil::string_vector;
using string_set = libutil::string_set;
using string_map = libutil::string_map;

}

#endif  // !__pbxbuild_Base_h
