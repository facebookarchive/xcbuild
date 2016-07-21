/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <builtin/copy/Driver.h>
#include <builtin/copy/Options.h>

#include <libutil/Filesystem.h>
#include <libutil/FSUtil.h>
#include <libutil/Subprocess.h>

#include <unordered_set>

using builtin::copy::Driver;
using builtin::copy::Options;
using libutil::Filesystem;
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
CopyPath(Filesystem *filesystem, std::string const &inputPath, std::string const &outputPath)
{
    if (!filesystem->createDirectory(FSUtil::GetDirectoryName(outputPath))) {
        return false;
    }

    Subprocess cp;
    if (!cp.execute("/bin/cp", { "-R", inputPath, outputPath }) || cp.exitcode() != 0) {
        return false;
    }

    /* Should preserve permissions but make writable. */
    Subprocess chmod;
    if (!chmod.execute("/bin/chmod", { "-R", "+w", outputPath }) || chmod.exitcode() != 0) {
        return false;
    }

    return true;
}

static int
Run(Filesystem *filesystem, Options const &options, std::string const &workingDirectory)
{
    if (!options.output()) {
        fprintf(stderr, "error: no output path provided\n");
        return 1;
    }

    if (options.stripDebugSymbols() || options.bitcodeStrip() != Options::BitcodeStripMode::None) {
        // TODO(grp): Implement strip support when copying.
#if 0
        fprintf(stderr, "warning: strip on copy is not supported\n");
#endif
    }

    if (options.preserveHFSData()) {
        fprintf(stderr, "warning: preserve HFS data is not supported\n");
    }

    std::string const &output = FSUtil::ResolveRelativePath(*options.output(), workingDirectory);
    auto excludes = std::unordered_set<std::string>(options.excludes().begin(), options.excludes().end());

    for (std::string input : options.inputs()) {
        input = FSUtil::ResolveRelativePath(input, workingDirectory);

        if (options.resolveSrcSymlinks()) {
            input = filesystem->resolvePath(input);
        }

        if (!filesystem->isDirectory(input) && !filesystem->isReadable(input)) {
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

        std::string outputPath = output + "/" + FSUtil::GetBaseName(input);
        if (!CopyPath(filesystem, input, outputPath)) {
            return 1;
        }
    }

    return 0;
}

int Driver::
run(std::vector<std::string> const &args, std::unordered_map<std::string, std::string> const &environment, Filesystem *filesystem, std::string const &workingDirectory)
{
    Options options;
    std::pair<bool, std::string> result = libutil::Options::Parse<Options>(&options, args);
    if (!result.first) {
        fprintf(stderr, "error: %s\n", result.second.c_str());
        return 1;
    }

    return Run(filesystem, options, workingDirectory);
}
