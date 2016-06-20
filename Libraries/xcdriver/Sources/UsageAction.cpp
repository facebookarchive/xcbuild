/**
 Copyright (c) 2016-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <xcdriver/UsageAction.h>

using xcdriver::UsageAction;
using xcdriver::Options;
using libutil::Filesystem;

UsageAction::
UsageAction()
{
}

UsageAction::
~UsageAction()
{
}

int UsageAction::
Run(Filesystem const *filesystem, Options const &options)
{
    // TODO: Perhaps have xcdriver/Tools/xcbuild.cpp pass the name of the
    //       executable (`argv[0]`) to `xcdriver::Driver::Run()`? That way,
    //       the `UsageAction` could display the name of the program, instead
    //       of hardcoding "xcbuild".
    printf("Usage: xcbuild [-project <projectname>] "
               "[[-target <targetname>]...|-alltargets] "
               "[-configuration <configurationname>] "
               "[-arch <architecture>]... "
               "[-sdk [<sdkname>|<sdkpath>]] "
               "[-showBuildSettings] [<buildsetting>=<value>]... "
               "[-formatter [default]] "
               "[-executor [simple|ninja]] "
               "[-generate] "
               "[<buildaction>]...\n"

           "       xcbuild [-project <projectname>] -scheme <schemeName> "
               "[-destination <destinationspecifier>]... "
               "[-configuration <configurationname>] "
               "[-arch <architecture>]... "
               "[-sdk [<sdkname>|<sdkpath>]] "
               "[-showBuildSettings] "
               "[<buildsetting>=<value>]... "
               "[-formatter [default]] "
               "[-executor [simple|ninja]] "
               "[-generate] "
               "[<buildaction>]...\n"

           "       xcbuild -workspace <workspacename> -scheme <schemeName> "
               "[-destination <destinationspecifier>]... "
               "[-configuration <configurationname>] "
               "[-arch <architecture>]... "
               "[-sdk [<sdkname>|<sdkpath>]] "
               "[-showBuildSettings] "
               "[<buildsetting>=<value>]... "
               "[-formatter [default]] "
               "[-executor [simple|ninja]] "
               "[-generate] "
               "[<buildaction>]...\n"

           "       xcbuild -version "
               "[-sdk [<sdkfullpath>|<sdkname>] "
               "[<infoitem>] ]\n"

           "       xcbuild -list "
               "[[-project <projectname>]|[-workspace <workspacename>]]\n"

           "       xcbuild -showsdks\n"

           "       xcbuild -exportArchive "
               "-archivePath <xcarchivepath> "
               "-exportPath <destinationpath> "
               "-exportOptionsPlist <plistpath>\n"

           "       xcbuild -exportLocalizations "
               "-localizationPath <path> "
               "-project <projectname> "
               "[-exportLanguage <targetlanguage>...]\n"

           "       xcbuild -importLocalizations "
               "-localizationPath <path> "
               "-project <projectname>\n\n");

    return 0;
}

