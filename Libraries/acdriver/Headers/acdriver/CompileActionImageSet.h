/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <ext/optional>
#include <car/Writer.h>

#include <xcassets/Asset/ImageSet.h>

#include <acdriver/Result.h>

#include <string>


bool
CompileContents(ext::optional<car::Writer> &writer,
	std::string ns,
	std::shared_ptr<xcassets::Asset::Asset> const &parent,
	xcassets::Asset::ImageSet::Image const &image,
	acdriver::Result *result);
