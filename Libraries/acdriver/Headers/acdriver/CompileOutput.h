/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef __acdriver_CompileOutput_h
#define __acdriver_CompileOutput_h

#include <plist/Dictionary.h>
#include <dependency/BinaryDependencyInfo.h>
#include <car/Writer.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <ext/optional>

namespace libutil { class Filesystem; }
namespace xcassets { namespace Asset { class Asset; } }

namespace acdriver {

class Options;
class Result;

/*
 * Prints the contents of an asset catalog.
 */
class CompileOutput {
public:
    enum class Format {
        /*
         * Write out a compiled asset catalog.
         */
        Compiled,
        /*
         * Copy the assets into a folder.
         */
        Folder,
    };

private:
    std::string                        _root;
    Format                             _format;

private:
    ext::optional<car::Writer>         _car;
    std::vector<std::pair<std::string, std::string>> _copies;

private:
    std::unique_ptr<plist::Dictionary> _additionalInfo;
    dependency::BinaryDependencyInfo   _dependencyInfo;

public:
    CompileOutput(std::string const &root, Format format);

public:
    /*
     * The directory the assets should be compiled into.
     */
    std::string const &root() const
    { return _root; }

    /*
     * The output format for the compiled assets.
     */
    Format format() const
    { return _format; }

public:
    /*
     * If the format is compiled, the compiled catalog writer.
     */
    ext::optional<car::Writer> const &car() const
    { return _car; }
    ext::optional<car::Writer> &car()
    { return _car; }

    /*
     * Files to copy into the output directory.
     */
    std::vector<std::pair<std::string, std::string>> const &copies() const
    { return _copies; }
    std::vector<std::pair<std::string, std::string>> &copies()
    { return _copies; }

public:
    /*
     * Additional Info.plist entries to include.
     */
    plist::Dictionary const *additionalInfo() const
    { return _additionalInfo.get(); }
    plist::Dictionary *additionalInfo()
    { return _additionalInfo.get(); }

    /*
     * Dependency info for the files used during the compilation.
     */
    dependency::BinaryDependencyInfo const &dependencyInfo() const
    { return _dependencyInfo; }
    dependency::BinaryDependencyInfo &dependencyInfo()
    { return _dependencyInfo; }

public:
    /*
     * Write the output to the filesystem.
     */
    bool write(
        libutil::Filesystem *filesystem,
        ext::optional<std::string> const &partialInfoPlist,
        ext::optional<std::string> const &dependencyInfo,
        Result *result) const;

public:
    /*
     * The identifier of an asset for use in results.
     */
    static std::string AssetReference(std::shared_ptr<xcassets::Asset::Asset> const &asset);
};

}

#endif // !__acdriver_CompileOutput_h
