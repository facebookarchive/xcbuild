/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/infoPlistUtility/Driver.h>
#include <builtin/infoPlistUtility/Options.h>
#include <plist/plist.h>
#include <pbxsetting/pbxsetting.h>

using builtin::infoPlistUtility::Driver;
using builtin::infoPlistUtility::Options;

Driver::
Driver()
{
}

Driver::
~Driver()
{
}

std::string Driver::
name()
{
    return "builtin-infoPlistUtility";
}

static std::pair<bool, std::string>
WritePkgInfo(plist::Dictionary const *root, std::string const &path)
{
    std::string pkgInfo;

    plist::String const *packageType = root->value<plist::String>("CFBundlePackageType");
    if (packageType != nullptr && packageType->value().size() == 4) {
        pkgInfo += packageType->value();
    } else {
        pkgInfo += "????";
    }

    plist::String const *signature = root->value<plist::String>("CFBundleSignature");
    if (signature != nullptr && signature->value().size() == 4) {
        pkgInfo += signature->value();
    } else {
        pkgInfo += "????";
    }

    std::ofstream pkgInfoFile = std::ofstream(path, std::ios::binary);
    if (pkgInfoFile.fail()) {
        return std::make_pair(false, "could not open output path " + path + " to write");
    }

    std::copy(pkgInfo.begin(), pkgInfo.end(), std::ostream_iterator<char>(pkgInfoFile));
    return std::make_pair(true, std::string());
}

static void
ExpandBuildSettings(plist::Object *value, pbxsetting::Environment const &environment)
{
    /*
     * Recursively expand any strings in the plist. Dictionary keys are not expanded.
     */
    if (auto dictionary = plist::CastTo<plist::Dictionary>(value)) {
        for (size_t n = 0; n < dictionary->count(); n++) {
            ExpandBuildSettings(dictionary->value(n), environment);
        }
    } else if (auto array = plist::CastTo<plist::Array>(value)) {
        for (size_t n = 0; n < array->count(); n++) {
            ExpandBuildSettings(array->value(n), environment);
        }
    } else if (auto string = plist::CastTo<plist::String>(value)) {
        pbxsetting::Value parsed = pbxsetting::Value::Parse(string->value());
        string->setValue(environment.expand(parsed));
    }
}

static void
AddBuildEnvironment(plist::Dictionary *root, pbxsetting::Environment const &environment)
{
    root->set("DTCompiler", plist::String::New(environment.resolve("DEFAULT_COMPILER")));
    root->set("DTXcode", plist::String::New(environment.resolve("XCODE_VERSION_ACTUAL")));
    root->set("DTXcodeBuild", plist::String::New(environment.resolve("XCODE_PRODUCT_BUILD_VERSION")));
    root->set("BuildMachineOSBuild", plist::String::New(environment.resolve("MAC_OS_X_PRODUCT_BUILD_VERSION")));

    root->set("DTPlatformName", plist::String::New(environment.resolve("PLATFORM_NAME")));
    root->set("DTPlatformBuild", plist::String::New(environment.resolve("PLATFORM_PRODUCT_BUILD_VERSION")));
    root->set("DTPlatformVersion", plist::String::New("")); // TODO(grp): Not available through build settings.

    root->set("DTSDKName", plist::String::New(environment.resolve("SDK_NAME")));
    root->set("DTSDKBuild", plist::String::New(environment.resolve("SDK_PRODUCT_BUILD_VERSION")));

    root->set("MinimumOSVersion", plist::String::New(environment.expand(pbxsetting::Value::Parse("$($(DEPLOYMENT_TARGET_SETTING_NAME))"))));
}

static pbxsetting::Environment
CreateBuildEnvironment(std::unordered_map<std::string, std::string> const &environment)
{
    std::vector<pbxsetting::Setting> settings;
    for (auto const &pair : environment) {
        pbxsetting::Setting setting = pbxsetting::Setting::Create(pair.first, pbxsetting::Value::String(pair.second));
        settings.push_back(setting);
    }

    pbxsetting::Level level = pbxsetting::Level(settings);
    pbxsetting::Environment settingsEnvironment = pbxsetting::Environment();
    settingsEnvironment.insertFront(level, false);

    return settingsEnvironment;
}

int Driver::
run(std::vector<std::string> const &args, std::unordered_map<std::string, std::string> const &environment)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    pbxsetting::Environment settingsEnvironment = CreateBuildEnvironment(environment);

    /* Read in the input. */
    std::ifstream inputFile = std::ifstream(options.input(), std::ios::binary);
    if (inputFile.fail()) {
        fprintf(stderr, "error: unable to read input %s\n", options.input().c_str());
        return 1;
    }

    std::vector<uint8_t> inputContents = std::vector<uint8_t>(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());

    /* Determine the input format. */
    std::unique_ptr<plist::Format::Any> inputFormat = plist::Format::Any::Identify(inputContents);
    if (inputFormat == nullptr) {
        fprintf(stderr, "error: input %s is not a plist\n", options.input().c_str());
        return 1;
    }

    /* Deserialize the input. */
    auto deserialize = plist::Format::Any::Deserialize(inputContents, *inputFormat);
    if (!deserialize.first) {
        fprintf(stderr, "error: %s: %s\n", options.input().c_str(), deserialize.second.c_str());
        return 1;
    }

    plist::Dictionary *root = plist::CastTo<plist::Dictionary>(deserialize.first.get());
    if (root == nullptr) {
        fprintf(stderr, "error: info plist root is not a dictionary\n");
        return 1;
    }

    /*
     * Expand all build settings in the plist. Build settings can be in dictionary keys
     * and strings. The resolved build setting values are passed through the environment.
     */
    if (options.expandBuildSettings()) {
        ExpandBuildSettings(root, settingsEnvironment);
    }

    /*
     * Process additional content files. These are plists that get merged with the
     * main Info.plist at the top level.
     */
    for (std::string const &additionalContentFile : options.additionalContentFiles()) {
        auto additionalContent = plist::Format::Any::Read(additionalContentFile);
        if (additionalContent.first == nullptr) {
            fprintf(stderr, "error: unable to open additional content file %s: %s\n", additionalContentFile.c_str(), additionalContent.second.c_str());
            return 1;
        }

        if (plist::Dictionary *dictionary = plist::CastTo<plist::Dictionary>(additionalContent.first.get())) {
            /* Pass true to replace existing entries. */
            root->merge(dictionary, true);
        }
    }

    /*
     * Info file keys/values: it's unknown what these are used for. Just warn.
     */
    if (!options.infoFileKeys().empty() && !options.infoFileValues().empty()) {
        // TODO(grp): Handle info file keys and values.
        fprintf(stderr, "warning: info file keys and values are not yet implemented\n");
    }

    /*
     * Platform and required architectures: it's unknown what these are used for. Just warn.
     */
    if (!options.platform().empty() || !options.requiredArchitectures().empty()) {
        // TODO(grp): Handle platform and required architectures.
        fprintf(stderr, "warning: platform and required architectures are not yet implemented\n");
    }

    /*
     * Add entries to the Info.plist from the build environment.
     */
    AddBuildEnvironment(root, settingsEnvironment);

    /*
     * Write the PkgInfo file. This is just the package type and signature.
     */
    if (!options.genPkgInfo().empty()) {
        auto result = WritePkgInfo(root, options.genPkgInfo());
        if (!result.first) {
            fprintf(stderr, "error: %s\n", result.second.c_str());
            return 1;
        }
    }

    /*
     * Copy the resource rules file. This is used by code signing.
     */
    if (!options.resourceRulesFile().empty()) {
        std::string resourceRulesInputPath = settingsEnvironment.resolve("CODE_SIGN_RESOURCE_RULES_PATH");
        if (!resourceRulesInputPath.empty()) {
            std::ifstream resourceRulesInput = std::ifstream(resourceRulesInputPath, std::ios::binary);
            if (resourceRulesInput.fail()) {
                fprintf(stderr, "error: unable to read input %s\n", resourceRulesInputPath.c_str());
                return 1;
            }

            std::ofstream resourceRulesOutput = std::ofstream(options.resourceRulesFile(), std::ios::binary);
            if (resourceRulesOutput.fail()) {
                fprintf(stderr, "error: could not open output path %s to write\n", options.resourceRulesFile().c_str());
                return 1;
            }

            std::copy(std::istreambuf_iterator<char>(resourceRulesInput), std::istreambuf_iterator<char>(), std::ostream_iterator<char>(resourceRulesOutput));
        }
    }

    if (options.output().empty()) {
        fprintf(stderr, "error: no output file specified\n");
        return 1;
    }

    /*
     * Determine the output format. By default, use the same as the input format.
     */
    plist::Format::Any outputFormat = *inputFormat;
    if (!options.format().empty()) {
        if (options.format() == "binary") {
            outputFormat = plist::Format::Any::Create(plist::Format::Binary::Create());
        } else if (options.format() == "xml") {
            outputFormat = plist::Format::Any::Create(plist::Format::XML::Create(plist::Format::Encoding::UTF8));
        } else if (options.format() == "ascii" || options.format() == "openstep") {
            outputFormat = plist::Format::Any::Create(plist::Format::ASCII::Create(plist::Format::Encoding::UTF8));
        } else {
            fprintf(stderr, "error: unknown output format %s\n", options.format().c_str());
            return 1;
        }
    }

    /* Serialize the output. */
    auto serialize = plist::Format::Any::Serialize(root, outputFormat);
    if (serialize.first == nullptr) {
        fprintf(stderr, "error: %s\n", serialize.second.c_str());
        return 1;
    }

    /* Write out the output. */
    std::ofstream outputFile = std::ofstream(options.output(), std::ios::binary);
    if (outputFile.fail()) {
        fprintf(stderr, "error: could not open output path %s to write\n", options.output().c_str());
        return 1;
    }

    std::copy(serialize.first->begin(), serialize.first->end(), std::ostream_iterator<char>(outputFile));

    return 0;
}
