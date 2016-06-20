/**
 Copyright (c) 2016-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/UsageAction.h>
#include <libutil/FSUtil.h>
#include <libutil/SysUtil.h>

using xcdriver::UsageAction;
using libutil::FSUtil;
using libutil::SysUtil;

UsageAction::
UsageAction()
{
}

UsageAction::
~UsageAction()
{
}

int UsageAction::
Run()
{
    std::string executable = FSUtil::GetBaseName(SysUtil::GetExecutablePath());

    fprintf(
        stdout,
        "Usage: %s [-project <projectname>] "
        "[[-target <targetname>]...|-alltargets] "
        "[-configuration <configurationname>] "
        "[-arch <architecture>]... "
        "[-sdk [<sdkname>|<sdkpath>]] "
        "[-showBuildSettings] [<buildsetting>=<value>]... "
        "[-formatter [default]] "
        "[-executor [simple|ninja]] "
        "[-generate] "
        "[<buildaction>]...\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s [-project <projectname>] -scheme <schemeName> "
        "[-destination <destinationspecifier>]... "
        "[-configuration <configurationname>] "
        "[-arch <architecture>]... "
        "[-sdk [<sdkname>|<sdkpath>]] "
        "[-showBuildSettings] "
        "[<buildsetting>=<value>]... "
        "[-formatter [default]] "
        "[-executor [simple|ninja]] "
        "[-generate] "
        "[<buildaction>]...\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -workspace <workspacename> -scheme <schemeName> "
        "[-destination <destinationspecifier>]... "
        "[-configuration <configurationname>] "
        "[-arch <architecture>]... "
        "[-sdk [<sdkname>|<sdkpath>]] "
        "[-showBuildSettings] "
        "[<buildsetting>=<value>]... "
        "[-formatter [default]] "
        "[-executor [simple|ninja]] "
        "[-generate] "
        "[<buildaction>]...\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -version "
        "[-sdk [<sdkfullpath>|<sdkname>] "
        "[<infoitem>] ]\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -list "
        "[[-project <projectname>]|[-workspace <workspacename>]]\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -showsdks\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -exportArchive "
        "-archivePath <xcarchivepath> "
        "-exportPath <destinationpath> "
        "-exportOptionsPlist <plistpath>\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -exportLocalizations "
        "-localizationPath <path> "
        "-project <projectname> "
        "[-exportLanguage <targetlanguage>...]\n",
        executable.c_str());

    fprintf(
        stdout,
        "       %s -importLocalizations "
        "-localizationPath <path> "
        "-project <projectname>\n",
        executable.c_str());

    fprintf(stdout, "\n");

    return 0;
}

