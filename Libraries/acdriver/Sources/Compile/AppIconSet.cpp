/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <acdriver/Compile/AppIconSet.h>
#include <acdriver/CompileOutput.h>
#include <acdriver/Result.h>
#include <plist/Array.h>
#include <plist/Boolean.h>
#include <plist/Dictionary.h>
#include <plist/String.h>
#include <libutil/Filesystem.h>

using acdriver::Compile::AppIconSet;
using acdriver::CompileOutput;
using acdriver::Result;
using libutil::Filesystem;

static plist::Dictionary *
IconsDictionary(plist::Dictionary *info, std::string const &key)
{
    if (auto existing = info->value<plist::Dictionary>(key)) {
        return existing;
    } else {
        auto icons = plist::Dictionary::New();
        info->set(key, std::move(icons));
        return info->value<plist::Dictionary>(key);
    }
}

static std::string
IdiomSuffix(xcassets::Slot::Idiom idiom)
{
    switch (idiom) {
        case xcassets::Slot::Idiom::Universal:
            return std::string();
        case xcassets::Slot::Idiom::Phone:
            return std::string();
        case xcassets::Slot::Idiom::Pad:
            return "~ipad";
        case xcassets::Slot::Idiom::Desktop:
            // TODO: no idiom suffix for desktop
            return std::string();
        case xcassets::Slot::Idiom::TV:
            return "~tv";
        case xcassets::Slot::Idiom::Watch:
            return "~watch";
        case xcassets::Slot::Idiom::Car:
            return "~car";
    }

    abort();
}

struct IdiomHash
{
    std::size_t operator()(xcassets::Slot::Idiom idiom) const
    {
        return static_cast<std::size_t>(idiom);
    }
};

bool AppIconSet::
Compile(
    std::shared_ptr<xcassets::Asset::AppIconSet> const &appIconSet,
    Filesystem *filesystem,
    CompileOutput *compileOutput,
    Result *result)
{
    /*
     * Copy the app icon images into the output.
     */
    auto files = std::unordered_map<xcassets::Slot::Idiom, std::vector<std::string>, IdiomHash>();
    if (appIconSet->images()) {
        for (xcassets::Asset::AppIconSet::Image const &image : *appIconSet->images()) {
            /*
             * Verify the image has the required information.
             */
            if (image.unassigned() || !image.fileName() || !image.imageSize() || !image.scale()) {
                result->document(
                    Result::Severity::Warning,
                    appIconSet->path(),
                    { CompileOutput::AssetReference(appIconSet) },
                    "Ambiguous Content",
                    "an icon in \"" + appIconSet->name().name() + "\" is unassigned");
                continue;
            }

            /*
             * Determine information about the icon image.
             */
            xcassets::Slot::ImageSize const &size = *image.imageSize();
            // TODO: verify the dimensions of the image are correct
            std::string sizeSuffix = xcassets::Slot::ImageSize::String(size);

            xcassets::Slot::Scale const &scale = *image.scale();
            std::string scaleSuffix = std::string();
            if (scale.value() != 1.0) {
                scaleSuffix = "@" + xcassets::Slot::Scale::String(scale);
            }

            xcassets::Slot::Idiom idiom = image.idiom().value_or(xcassets::Slot::Idiom::Universal);
            // TODO: skip images with idioms inappropriate for target platform
            std::string idiomSuffix = IdiomSuffix(idiom);

            /*
             * Copy the icon image into the output.
             */
            std::string source = appIconSet->path() + "/" + *image.fileName();
            std::string destination = compileOutput->root() + "/" + appIconSet->name().name() + sizeSuffix + scaleSuffix + idiomSuffix + ".png";
            compileOutput->copies().push_back({ source, destination });

            /*
             * Add the file to the output info.
             */
            std::string name = appIconSet->name().name() + sizeSuffix;
            files[idiom].push_back(name);
        }
    }

    /*
     * Build up the info about the icons.
     */
    for (auto const &pair : files) {
        /*
         * Record details about the icon itself.
         */
        auto primary = plist::Dictionary::New();

        /* List the files for the icon. */
        auto files = plist::Array::New();
        for (std::string const &file : pair.second) {
            files->append(plist::String::New(file));
        }
        primary->set("CFBundleIconFiles", std::move(files));

        /* Record if the icon is pre-rendered. */
        if (appIconSet->preRendered()) {
            primary->set("UIPrerenderedIcon", plist::Boolean::New(true));
        }

        /*
         * Store the icon in the additional info.
         */
        std::string idiomSuffix = IdiomSuffix(pair.first);
        plist::Dictionary *icons = IconsDictionary(compileOutput->additionalInfo(), "CFBundleIcons" + idiomSuffix);
        icons->set("CFBundlePrimaryIcon", std::move(primary));
    }

    return true;
}
