/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __acdriver_CompileActionImageSet_h
#define __acdriver_CompileActionImageSet_h

#include <acdriver/CompileOutput.h>
#include <acdriver/Result.h>

#include <xcassets/Asset/Asset.h>
#include <xcassets/Asset/ImageSet.h>

#include <memory>

namespace acdriver {

class CompileOutput;
class Options;
class Result;

class CompileActionImageSet {
private:
    CompileActionImageSet();
    ~CompileActionImageSet();

public:
    static bool Compile(
        std::shared_ptr<xcassets::Asset::ImageSet> const &imageSet,
        libutil::Filesystem *filesystem,
        Options const &options,
        CompileOutput *compileOutput,
        Result *result);

    static bool CompileAsset(
        std::shared_ptr<xcassets::Asset::ImageSet> const &imageSet,
        xcassets::Asset::ImageSet::Image const &image,
        libutil::Filesystem *filesystem,
        Options const &options,
        CompileOutput *compileOutput,
        Result *result);
};

}

#endif // !__acdriver_CompileActionImageSet_h
