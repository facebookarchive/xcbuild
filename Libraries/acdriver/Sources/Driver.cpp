/* Copyright 2013-present Facebook. All Rights Reserved. */

#include <acdriver/Driver.h>
#include <acdriver/Options.h>
#include <acdriver/Output.h>
#include <acdriver/Result.h>

#include <iostream>

using acdriver::Driver;
using acdriver::Options;
using acdriver::Output;
using acdriver::Result;

Driver::
Driver()
{
}

Driver::
~Driver()
{
}

static void
RunVersion(Options const &options, Output *output, Result *result)
{
    std::unique_ptr<plist::Dictionary> dict = plist::Dictionary::New();
    dict->set("bundle-version", plist::String::New("1"));
    dict->set("short-bundle-version", plist::String::New("1"));

    std::string text;
    text += "bundle-version: 1\n";
    text += "short-bundle-version: 1\n";

    output->add("com.apple.actool.version", std::move(dict), text);
}

static void
RunInternal(Options const &options, Output *output, Result *result)
{
    if (options.version()) {
        RunVersion(options, output, result);
    } else {
        // TODO: Implement actool operations.
        result->normal(Result::Severity::Warning, "actool not yet implemented");
    }
}

static Output::Format
OptionsOutputFormat(Options const &options, Result *result)
{
    if (options.outputFormat().empty() || options.outputFormat() == "xml1") {
        return Output::Format::XML;
    } else if (options.outputFormat() == "binary1") {
        return Output::Format::Binary;
    } else if (options.outputFormat() == "human-readable-text") {
        return Output::Format::Text;
    } else {
        result->normal(Result::Severity::Error, "unknown output format");
        return Output::Format::XML;
    }
}

int Driver::
Run(std::vector<std::string> const &args)
{
    Result result;
    Output output;

    /*
     * Parse input options.
     */
    Options options;
    std::pair<bool, std::string> optionsResult = libutil::Options::Parse<Options>(&options, args);
    if (!optionsResult.first) {
        result.normal(Result::Severity::Error, optionsResult.second, std::string("unknown option"));
    }

    /*
     * Validate output format. Do this first so errors are caught early.
     */
    Output::Format outputFormat = OptionsOutputFormat(options, &result);

    if (result.success()) {
        /*
         * Perform actions specified by options.
         */
        RunInternal(options, &output, &result);
    }

    /*
     * Serialize the result into the output.
     */
    result.write(&output);

    /*
     * Write out the output in the specified output format.
     */
    ext::optional<std::vector<uint8_t>> outputContents = output.serialize(outputFormat);
    if (outputContents) {
        std::copy(outputContents->begin(), outputContents->end(), std::ostream_iterator<char>(std::cout));
    } else {
        fprintf(stderr, "error: unable to serialize output\n");
    }

    return (result.success() ? 0 : 1);
}
