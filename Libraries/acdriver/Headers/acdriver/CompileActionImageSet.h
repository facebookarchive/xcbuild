/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/CompileOutput.h>
#include <acdriver/Result.h>

#include <ext/optional>
#include <car/Writer.h>

#include <xcassets/Asset/Catalog.h>
#include <xcassets/Asset/ImageSet.h>

#include <string>

bool
CompileAsset(
    xcassets::Asset::ImageSet::Image const &image,
    std::shared_ptr<xcassets::Asset::Asset> const &parent,
    libutil::Filesystem *filesystem,
    acdriver::Options const &options,
    acdriver::CompileOutput *compileOutput,
    acdriver::Result *result);
