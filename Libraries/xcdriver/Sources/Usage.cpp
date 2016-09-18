/**
 Copyright (c) 2016-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/Usage.h>
#include <libutil/FSUtil.h>
#include <libutil/SysUtil.h>

#include <sstream>

using xcdriver::Usage;
using libutil::FSUtil;
using libutil::SysUtil;

std::string Usage::
Text() {
    std::string executable = FSUtil::GetBaseName(SysUtil::GetDefault()->executablePath());

    std::ostringstream result;
    result << "Usage: " << executable << " [-project <projectname>] "
        "[[-target <targetname>]...|-alltargets] "
        "[-configuration <configurationname>] "
        "[-arch <architecture>]... "
        "[-sdk [<sdkname>|<sdkpath>]] "
        "[-showBuildSettings] [<buildsetting>=<value>]... "
        "[-formatter [default]] "
        "[-executor [simple|ninja]] "
        "[-generate] "
        "[<buildaction>]..." << std::endl;

    result << "       " << executable << " "
        "[-project <projectname>] -scheme <schemeName> "
        "[-destination <destinationspecifier>]... "
        "[-configuration <configurationname>] "
        "[-arch <architecture>]... "
        "[-sdk [<sdkname>|<sdkpath>]] "
        "[-showBuildSettings] "
        "[<buildsetting>=<value>]... "
        "[-formatter [default]] "
        "[-executor [simple|ninja]] "
        "[-generate] "
        "[<buildaction>]..." << std::endl;

    result << "       " << executable << " "
        "-workspace <workspacename> -scheme <schemeName> "
        "[-destination <destinationspecifier>]... "
        "[-configuration <configurationname>] "
        "[-arch <architecture>]... "
        "[-sdk [<sdkname>|<sdkpath>]] "
        "[-showBuildSettings] "
        "[<buildsetting>=<value>]... "
        "[-formatter [default]] "
        "[-executor [simple|ninja]] "
        "[-generate] "
        "[<buildaction>]..." << std::endl;

    result << "       " << executable << " -version "
        "[-sdk [<sdkfullpath>|<sdkname>] "
        "[<infoitem>] ]" << std::endl;

    result << "       " << executable << " -list "
        "[[-project <projectname>]|[-workspace <workspacename>]]" << std::endl;

    result << "       " << executable << " -showsdks" << std::endl;

    result << "       " << executable << " -exportArchive "
        "-archivePath <xcarchivepath> "
        "-exportPath <destinationpath> "
        "-exportOptionsPlist <plistpath>" << std::endl;

    result << "       " << executable << " -exportLocalizations "
        "-localizationPath <path> "
        "-project <projectname> "
        "[-exportLanguage <targetlanguage>...]" << std::endl;

    result << "       " << executable << " -importLocalizations "
        "-localizationPath <path> "
        "-project <projectname>" << std::endl;

    result << std::endl;

    return result.str();
}
