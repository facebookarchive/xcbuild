/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/CompileOutput.h>
#include <acdriver/Options.h>
#include <acdriver/Result.h>
#include <libutil/Filesystem.h>

using acdriver::CompileOutput;
using acdriver::Options;
using acdriver::Result;
using libutil::Filesystem;

CompileOutput::
CompileOutput(std::string const &root, Format format) :
    _root          (root),
    _format        (format),
    _additionalInfo(plist::Dictionary::New())
{
}

bool CompileOutput::
write(Filesystem *filesystem, Options const &options, Result *result) const
{
    bool success = true;

    /*
     * Write out compiled archive.
     */
    if (_car) {
        // TODO: only if the car has contents
        _car->write();
    }

    /*
     * Copy files into output.
     */
    for (std::pair<std::string, std::string> const &copy : _copies) {
        // TODO: copy files
        (void)copy;
    }

    /*
     * Write out partial info plist, if requested.
     */
    if (!options.outputPartialInfoPlist().empty()) {
        auto format = plist::Format::XML::Create(plist::Format::Encoding::UTF8);
        auto serialize = plist::Format::XML::Serialize(_additionalInfo.get(), format);
        if (serialize.first == nullptr) {
            result->normal(Result::Severity::Error, "unable to serialize partial info plist");
            success = false;
        } else {
            if (!filesystem->write(*serialize.first, options.outputPartialInfoPlist())) {
                result->normal(Result::Severity::Error, "unable to write partial info plist");
                success = false;
            }
        }
    }

    /*
     * Write out dependency info, if requested.
     */
    if (!options.exportDependencyInfo().empty()) {
        if (!filesystem->write(_dependencyInfo.serialize(), options.exportDependencyInfo())) {
            result->normal(Result::Severity::Error, "unable to write dependency info");
            success = false;
        }
    }

    return success;
}


