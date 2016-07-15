/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/Compile/DataSet.h>
#include <acdriver/CompileOutput.h>
#include <acdriver/Result.h>
#include <libutil/Filesystem.h>

using acdriver::Compile::DataSet;
using acdriver::CompileOutput;
using acdriver::Result;
using libutil::Filesystem;

bool DataSet::
Compile(
    std::shared_ptr<xcassets::Asset::DataSet> const &dataSet,
    Filesystem *filesystem,
    CompileOutput *compileOutput,
    Result *result)
{
    result->document(
        Result::Severity::Warning,
        dataSet->path(),
        { CompileOutput::AssetReference(dataSet) },
        "Not Implemented",
        "data set not yet supported");

    return false;
}
