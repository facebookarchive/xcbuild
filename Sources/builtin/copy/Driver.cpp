/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/copy/Driver.h>
#include <builtin/copy/Options.h>

using builtin::copy::Driver;
using builtin::copy::Options;
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
    return "builtin-copy";
}

static bool
CopyFile(std::string const &inputPath, std::string const &outputPath)
{
    if (!FSUtil::CreateDirectory(FSUtil::GetDirectoryName(outputPath))) {
        return false;
    }

    Subprocess cp;
    if (!cp.execute("/bin/cp", { inputPath, outputPath }) || cp.exitcode() != 0) {
        return false;
    }

    return true;
}

static int
Run(Options const &options, std::string const &workingDirectory)
{
    if (options.stripDebugSymbols() || options.bitcodeStrip() != Options::BitcodeStripMode::None) {
        // TODO(grp): Implement strip support when copying.
        fprintf(stderr, "warning: strip on copy is not supported\n");
    }

    if (options.preserveHFSData()) {
        fprintf(stderr, "warning: preserve HFS data is not supported\n");
    }

    std::string const &output = FSUtil::ResolveRelativePath(options.output(), workingDirectory);
    auto excludes = std::unordered_set<std::string>(options.excludes().begin(), options.excludes().end());

    for (std::string input : options.inputs()) {
        input = FSUtil::ResolveRelativePath(input, workingDirectory);

        if (options.resolveSrcSymlinks()) {
            input = FSUtil::ResolvePath(input);
        }

        bool isDirectory = FSUtil::TestForDirectory(input);

        if (!isDirectory && !FSUtil::TestForRead(input)) {
            if (options.ignoreMissingInputs()) {
                continue;
            } else {
                fprintf(stderr, "error: missing input '%s'\n", input.c_str());
                return 1;
            }
        }

        if (options.verbose()) {
            printf("verbose: copying %s -> %s\n", input.c_str(), output.c_str());
        }

        if (isDirectory) {
            std::string inputName = FSUtil::GetBaseName(input);

            bool succeeded = true;
            FSUtil::EnumerateRecursive(input, [&](std::string const &path) -> bool {
                if (excludes.find(FSUtil::GetBaseName(path)) != excludes.end()) {
                    if (options.verbose()) {
                        printf("verbose: skipping excluded path %s\n", path.c_str());
                    }
                    return true;
                }

                std::string relative = FSUtil::GetRelativePath(path, input);
                std::string outputFile = output + "/" + inputName + "/" + relative;

                succeeded = CopyFile(path, outputFile);
                return succeeded;
            });
            if (!succeeded) {
                return 1;
            }
        } else {
            std::string outputFile = output + "/" + FSUtil::GetBaseName(input);
            if (!CopyFile(input, outputFile)) {
                return 1;
            }
        }
    }

    return 0;
}

int Driver::
run(std::vector<std::string> const &args, std::unordered_map<std::string, std::string> const &environment, std::string const &workingDirectory)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    return Run(options, workingDirectory);
}
