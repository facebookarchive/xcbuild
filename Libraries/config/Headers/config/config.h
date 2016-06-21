/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __config_config_h
#define __config_config_h

#include <libutil/Filesystem.h>
#include <plist/Object.h>

namespace config {

std::unique_ptr<plist::Object> readDefaults(libutil::Filesystem const *filesystem);

}

#endif  // !__config_config_h
