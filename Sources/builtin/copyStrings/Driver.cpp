/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/copyStrings/Driver.h>
#include <builtin/copyStrings/Options.h>
#include <plist/plist.h>

using builtin::copyStrings::Driver;
using builtin::copyStrings::Options;
using libutil::FSUtil;
using libutil::Subprocess;

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
    return "builtin-copyStrings";
}

static bool
ParseStringsEncoding(std::string const &string, plist::Format::Any *format)
{
    if (string.empty()) {
        return true;
    } else if (strcasecmp(string.c_str(), "binary") == 0) {
        *format = plist::Format::Any::Create(plist::Format::Binary::Create());
        return true;
    } else if (strcasecmp(string.c_str(), "utf-8") == 0) {
        plist::Format::Encoding encoding = plist::Format::Encoding::UTF8;
        *format = plist::Format::Any::Create(plist::Format::ASCII::Create(true, encoding));
        return true;
    } else if (strcasecmp(string.c_str(), "utf-16") == 0) {
        plist::Format::Encoding encoding = plist::Format::Encoding::UTF16LE;
        *format = plist::Format::Any::Create(plist::Format::ASCII::Create(true, encoding));
        return true;
    } else if (strcasecmp(string.c_str(), "utf-32") == 0) {
        plist::Format::Encoding encoding = plist::Format::Encoding::UTF32LE;
        *format = plist::Format::Any::Create(plist::Format::ASCII::Create(true, encoding));
        return true;
    } else {
        return false;
    }
}

static std::pair<bool, std::string>
ValidateStrings(plist::Object const *root)
{
    if (plist::Dictionary const *dictionary = plist::CastTo<plist::Dictionary>(root)) {
        for (size_t n = 0; n < dictionary->count(); n++) {
            auto key = dictionary->key(n);
            auto value = dictionary->value <plist::String> (key);
            if (value == nullptr) {
                return std::make_pair(false, "strings key '" + key + "' is not a string");
            }
        }

        return std::make_pair(true, std::string());
    } else {
        return std::make_pair(false, "strings file is not a dictionary");
    }
}

static bool
ValidateOptions(Options const &options)
{

    /*
     * It's unclear if an output directory should be required, but require it for
     * now since the behavior without one is also unclear.
     */
    if (options.outputDirectory().empty()) {
        fprintf(stderr, "error: output directory not provided\n");
        return false;
    }

    /*
     * Require at least one input.
     */
    if (options.inputs().empty()) {
        fprintf(stderr, "error: no input files provided\n");
        return false;
    }

    return true;
}

int Driver::
run(std::vector<std::string> const &args, std::unordered_map<std::string, std::string> const &environment, std::string const &workingDirectory)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return -1;
    }

    /*
     * Validate options.
     */
    if (!ValidateOptions(options)) {
        return -1;
    }

    /*
     * Determine output encoding. Default to UTF-16 since that's what strings files should be.
     */
    plist::Format::Any outputFormat = plist::Format::Any::Create(plist::Format::ASCII::Create(true, plist::Format::Encoding::UTF16LE));
    if (!ParseStringsEncoding(options.outputEncoding(), &outputFormat)) {
        fprintf(stderr, "error: invalid output encoding '%s'\n", options.outputEncoding().c_str());
        return -1;
    }

    /*
     * Process each input.
     */
    for (std::string const &inputPath : options.inputs()) {
        /* Read in the input. */
        std::string resolvedInputPath = FSUtil::ResolveRelativePath(inputPath, workingDirectory);
        std::ifstream inputFile = std::ifstream(resolvedInputPath, std::ios::binary);
        if (inputFile.fail()) {
            fprintf(stderr, "error: unable to read input %s\n", inputPath.c_str());
            return 1;
        }
        std::vector<uint8_t> inputContents = std::vector<uint8_t>(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());

        /* Determine the input format. */
        std::unique_ptr<plist::Format::Any> inputFormat = plist::Format::Any::Identify(inputContents);
        if (inputFormat == nullptr) {
            fprintf(stderr, "error: input %s is not a plist\n", inputPath.c_str());
            return 1;
        }

        /* If no input format was specified, use the detected strings encoding. */
        plist::Format::Any resolvedInputFormat = *inputFormat;
        if (!ParseStringsEncoding(options.inputEncoding(), &resolvedInputFormat)) {
            fprintf(stderr, "error: invalid input encoding '%s'\n", options.inputEncoding().c_str());
            return -1;
        }

        /* Deserialize the input. */
        auto deserialize = plist::Format::Any::Deserialize(inputContents, resolvedInputFormat);
        if (!deserialize.first) {
            fprintf(stderr, "error: %s: %s\n", inputPath.c_str(), deserialize.second.c_str());
            return 1;
        }

        /* If requested, validate the strings file is valid. */
        if (options.validate()) {
            auto validation = ValidateStrings(deserialize.first.get());
            if (!validation.first) {
                fprintf(stderr, "error: %s: %s\n", inputPath.c_str(), validation.second.c_str());
                return 1;
            }
        }

        /* Output to the same name as the input, but in the output directory. */
        std::string outputPath = FSUtil::ResolveRelativePath(options.outputDirectory(), workingDirectory) + "/" + FSUtil::GetBaseName(inputPath);

        /* Write out the output. */
        auto serialize = plist::Format::Any::Write(outputPath, deserialize.first.get(), outputFormat);
        if (!serialize.first) {
            fprintf(stderr, "error: %s: %s\n", inputPath.c_str(), serialize.second.c_str());
            return 1;
        }
    }

    return 0;
}
